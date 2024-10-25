#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"
#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wunreachable-code-return"

#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

#include "Context.h"
#include "CreateSessionDescriptionObserver.h"
#include "Fifo.h"
#include "Logger.h"
#include "SetLocalDescriptionObserver.h"
#include "SetRemoteDescriptionObserver.h"
#include "rtc_base/strings/json.h"
#include "video.h"

using namespace webrtc;
using namespace unity::webrtc;

#define UNITY_INTERFACE_EXPORT
#define JLogPrint unity::webrtc::JLogPrint

#define VIDEO_MIN_BITRATE 14999999
#define VIDEO_MAX_BITRATE 15000000
#define FPS 60

Fifo fifo;
std::unique_ptr<webrtc::Clock> s_clock;
std::vector<std::string> ice_map;
bool got_answer = false;
std::mutex ice_map_mtx;

enum SignalingType {
  TYPE_SDP = 1,
  TYPE_ICE = 2,
  START = 3,
  END = 4,
};

namespace webrtc {
PeerConnectionInterface::PeerConnectionState PeerConnectionState;
}

std::vector<std::string> Split(const std::string& str,
                               const std::string& delimiter) {
  std::vector<std::string> dst;
  std::string s = str;
  size_t pos = 0;

  if (str.empty())
    return dst;

  while (true) {
    pos = s.find(delimiter);
    size_t length = pos;
    if (pos == std::string::npos)
      length = str.length();
    dst.push_back(s.substr(0, length));
    if (pos == std::string::npos)
      break;
    s.erase(0, pos + delimiter.length());
  }
  return dst;
}

std::map<std::string, std::string> ConvertSdp(const std::string& src) {
  std::map<std::string, std::string> map;
  std::vector<std::string> vec = Split(src, ";");

  for (const auto& str : vec) {
    std::vector<std::string> pair = Split(str, "=");
    map.emplace(pair[0], pair[1]);
  }
  return map;
}

void SendAll_ICEs() {
  if (ice_map.size() > 0) {
    for (auto&& ice : ice_map) {
      JLogPrint(LoggingSeverity::LS_INFO, "Send ICE: %s", ice.c_str());
      fifo.Write(ice);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      if (webrtc::PeerConnectionState >
          PeerConnectionInterface::PeerConnectionState::kConnecting)
        break;
    }
  }
  ice_map.clear();
  JLogPrint(LoggingSeverity::LS_INFO, "ALL ICEs SENT");
}

void OnDatachannel(DataChannelInterface*, const uint8_t* msg, int32_t size) {
  JLogPrint(LoggingSeverity::LS_INFO, "OnDatachannel");
}

DataChannelInterface* datachannel;
void OnDatachannelOpen(DataChannelInterface*) {
  if (datachannel) {
    datachannel->Send(DataBuffer("VIRTUAL_KEYBOARD"));
  }
}

PeerConnectionObject* ContextCreatePeerConnection(Context* context) {
  PeerConnectionInterface::RTCConfiguration config;
  config.sdp_semantics = SdpSemantics::kUnifiedPlan;
  config.enable_implicit_rollback = true;
  config.set_suspend_below_min_bitrate(false);

  webrtc::PeerConnectionInterface::IceServer server;
  server.urls.push_back("stun:stun.l.google.com:19302");
  config.servers.push_back(server);

  config.port_allocator_config.min_port = 3005;
  config.port_allocator_config.max_port = 3005 + 3;
  return context->CreatePeerConnection(config);
}

void ContextDeletePeerConnection(Context* context, PeerConnectionObject* obj) {
  obj->Close();
  context->DeletePeerConnection(obj);
}

UNITY_INTERFACE_EXPORT DataChannelInterface* ContextCreateDataChannel(
    Context* ctx,
    PeerConnectionObject* obj,
    const char* label) {
  DataChannelInit _options;

  return ctx->CreateDataChannel(obj, label, _options);
}

UNITY_INTERFACE_EXPORT void ContextDeleteDataChannel(
    Context* ctx,
    DataChannelInterface* channel) {
  ctx->DeleteDataChannel(channel);
}

int StartNewStream(Context* ctx) {
  auto pco = ContextCreatePeerConnection(ctx);

  // RegisterConnectionStateChange
  pco->RegisterConnectionStateChange(
      [](PeerConnectionObject* pco,
         PeerConnectionInterface::PeerConnectionState state) {
        webrtc::PeerConnectionState = state;
        JLogPrint(LoggingSeverity::LS_INFO, "PeerConnectionState: %d",
                  webrtc::PeerConnectionState);
      });

  // ICECandidate
  pco->RegisterIceCandidate([](PeerConnectionObject* pco, const char* sdp,
                               const char* sdp_mid, const int mline) {
    // Send ICE
    std::lock_guard<std::mutex> lock(ice_map_mtx);

    Json::Value json;
    json["data"]["type"] = SignalingType::TYPE_ICE;
    json["data"]["ice"]["Candidate"] = sdp;
    json["data"]["ice"]["SDPMid"] = sdp_mid;
    json["data"]["ice"]["SDPMLineIndex"] = mline;

    std::string jsonString = Json::FastWriter().write(json);

    if (got_answer) {
      fifo.Write(jsonString);
    } else {
      ice_map.push_back(jsonString);
    }

    JLogPrint(LoggingSeverity::LS_INFO, "SENT ICE: \n%s", sdp);
  });

  // Datachannel
  datachannel = ContextCreateDataChannel(ctx, pco, "hid");
  ctx->GetDataChannelObject(datachannel)->RegisterOnMessage(OnDatachannel);
  ctx->GetDataChannelObject(datachannel)->RegisterOnOpen(OnDatachannelOpen);

  ///////////// VIDEO ////////////////////////////////////
  rtc::scoped_refptr<VideoTrackSourceInterface> video_source;
  rtc::scoped_refptr<VideoTrackInterface> video_track;

  video_source = ctx->CreateVideoSource();
  ctx->AddRefPtr(video_source);

  // Create VideoTrack
  video_track = ctx->CreateVideoTrack("video", video_source.get());
  ctx->AddRefPtr(video_track);

  {
    // Create Video Transceiver
    RtpTransceiverInit TransceiverInit;
    TransceiverInit.direction = RtpTransceiverDirection::kSendOnly;
    RtpEncodingParameters encoding_param1;
    encoding_param1.active = true;
    encoding_param1.max_bitrate_bps = VIDEO_MAX_BITRATE;
    encoding_param1.min_bitrate_bps = VIDEO_MIN_BITRATE;
    encoding_param1.max_framerate = FPS;
    TransceiverInit.send_encodings.push_back(encoding_param1);
    auto transceiver_or_error =
        pco->connection->AddTransceiver(video_track, TransceiverInit);
    if (!transceiver_or_error.error().ok()) {
      JLogPrint(LoggingSeverity::LS_ERROR, "Cannot add Transceiver");
    }
    auto transceiver = transceiver_or_error.value().get();

    // Set Codec Preferences
    RtpCodecCapability codec_capability;
    codec_capability.clock_rate = 90000;
    codec_capability.kind = cricket::MediaType::MEDIA_TYPE_VIDEO;
    codec_capability.name = "H264";

    // For PC
    // implementation_name=NvCodec;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=420033
    // For Iphone Safari
    // implementation_name=NvCodec;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e033
codec_capability.parameters = ConvertSdp(
    "implementation_name=NvCodec;"
    "level-asymmetry-allowed=1;"
    "packetization-mode=1;"
    "profile-level-id=42e033;"
    "flexfec-03=enabled;repair-window=2000000"
    );
    std::vector<RtpCodecCapability> _codecs = {codec_capability};
    transceiver->SetCodecPreferences(_codecs);
  }

  ///////////// VIDEO END ////////////////////////////////////

  // Create and Send Offer
  SetLocalDescriptionObserver::RegisterCallback(
      [](PeerConnectionObject* pco, SetLocalDescriptionObserver* sldo,
         RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
          JLogPrint(LoggingSeverity::LS_ERROR,
                    "SetLocalDescriptionObserver error: %s", error);
        } else {
          JLogPrint(LoggingSeverity::LS_INFO, "Local Description Set");
        }
      });

  unity::webrtc::CreateSessionDescriptionObserver::RegisterCallback(
      [](PeerConnectionObject* peerConnectionObject,
         unity::webrtc::CreateSessionDescriptionObserver* observer,
         RTCSdpType sdpType, std::string sdp, RTCErrorType errorType,
         const char* error) {
        // SetLocaldesc
        const RTCSessionDescription desc = {RTCSdpType::Offer,
                                            (char*)sdp.c_str()};
        std::string error_;
        auto sldo = SetLocalDescriptionObserver::Create(peerConnectionObject);
        peerConnectionObject->SetLocalDescription(desc, sldo, error_);

        Json::Value json;

        json["data"]["type"] = SignalingType::TYPE_SDP;
        json["data"]["sdp"]["SDPData"] = sdp;
        json["data"]["sdp"]["Type"] = "offer";

        // Convert JSON object to string
        std::string jsonString = Json::FastWriter().write(json);
        fifo.Write(jsonString);
        JLogPrint(LoggingSeverity::LS_INFO, "SENT OFFER: \n%s\n%s", sdp.c_str(),
                  error);

        std::ofstream file("offer.json");
        file << jsonString;
      });

  auto csdo = unity::webrtc::CreateSessionDescriptionObserver::Create(pco);
  pco->CreateOffer(RTCOfferAnswerOptions(), csdo.get());

  std::string answer = fifo.Read();
  {
    std::ofstream answer_file("answer.json");
    answer_file << answer;
    answer_file.close();
  }
  // Read and Set Answer
  SetRemoteDescriptionObserver::RegisterCallback(
      [](PeerConnectionObject* pco, SetRemoteDescriptionObserver* srdo,
         RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
          JLogPrint(LoggingSeverity::LS_ERROR,
                    "SetRemoteDescriptionObserver error: %s", error);
        } else {
          std::lock_guard<std::mutex> lock(ice_map_mtx);
          got_answer = true;
          JLogPrint(LoggingSeverity::LS_INFO, "Remote Description Set");
        }
      });
  Json::Reader reader;
  Json::Value root;

  // Parse the JSON string
  bool parsingSuccessful = reader.parse(answer, root);
  auto sdpData = root["sdp"]["SDPData"].asString();
  const RTCSessionDescription desc = {RTCSdpType::Answer,
                                      (char*)sdpData.c_str()};
  std::string error_;

  auto observer = SetRemoteDescriptionObserver::Create(pco);
  pco->SetRemoteDescription(desc, observer, error_);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  SendAll_ICEs();

  while (true) {
    std::string data = fifo.Read();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (webrtc::PeerConnectionState ==
        PeerConnectionInterface::PeerConnectionState::kConnected) {
      break;
    }
    JLogPrint(LoggingSeverity::LS_INFO, "Data: %s", data.c_str());
    Json::Reader reader;
    Json::Value root;

    // Parse the JSON string
    bool parsingSuccessful = reader.parse(data, root);

    std::string sdp_mid = root["SDPMid"].asString();
    int sdp_mlineindex = 0;
    std::string sdp = root["Candidate"].asString();
    webrtc::SdpParseError error;
    std::unique_ptr<webrtc::IceCandidateInterface> candidate(
        webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));

    JLogPrint(LoggingSeverity::LS_INFO, "AddIceCandidate: sdp:%s", sdp.c_str());
    pco->connection->AddIceCandidate(candidate.get());
  }

  datachannel->Send(DataBuffer("Hello world"));
  video vid(video_source.get());
  vid.Start();

  datachannel->Close();
  pco->connection->Close();
  return 0;
}

int main() {
  ContextManager* ctx_manager = ContextManager::GetInstance();
  ContextDependencies dep{};
  auto ctx = ctx_manager->CreateContext(1, dep);

  s_clock.reset(Clock::GetRealTimeClock());

  int ret = StartNewStream(ctx);

  return 0;
}