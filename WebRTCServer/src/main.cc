#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"
#pragma clang diagnostic ignored "-Wunreachable-code"

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "Context.h"
#include "PeerConnectionObject.h"
#include "WebRTCPlugin.h"

using namespace webrtc;

WebRTCPlugin plugin;
Context ctx;

void OnSuccess(PeerConnectionObject* pco, RTCSdpType type, const char* sdp) {
  switch (type) {
    case RTCSdpType::Offer: {
      RTCSessionDescription desc = {};
      desc.sdp = sdp;
      desc.type = type;
      std::string error;
      auto errorType =
          plugin.PeerConnectionSetLocalDescription(&ctx, pco, &desc, error);

      if (errorType != webrtc::RTCErrorType::NONE) {
        std::cout << "error: " << error << std::endl;
      }

      std::cout << "Offer: \n" << sdp << std::endl;
    } break;

    case RTCSdpType::Answer: {
      RTCSessionDescription desc = {};
      desc.sdp = sdp;
      desc.type = type;
      std::string error;

      plugin.PeerConnectionSetLocalDescription(&ctx, pco, &desc, error);
      LogPrint("Answer SDP: \n%s", sdp);
    } break;
    default:
      // TODO:
      break;
  }
}

void OnIceCandidate(PeerConnectionObject* pco,
                    const char* candidate,
                    const char* sdpMid,
                    const int sdpMlineIndex) {
  LogPrint(
      "OnIceCandidate:\n candidate: %s\n, sdpMid: %s\n, sdpMlineIndex: %d\n",
      candidate, sdpMid, sdpMlineIndex);
}

std::string GetInput() {
  std::string line;
  std::vector<std::string> v;

  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      break;
    }
    v.push_back(line);
  }

  std::stringstream ss;
  for (std::vector<std::string>::const_iterator itr = v.begin(); itr != v.end();
       ++itr) {
    ss << *itr;
    ss << std::endl;
  }

  return ss.str();
}

void CreateOffer(PeerConnectionObject* pco) {
  const RTCOfferAnswerOptions options{false, true};
  plugin.PeerConnectionCreateOffer(pco, &options);
}

void SetRemoteOffer(PeerConnectionObject* pco) {
  const RTCOfferAnswerOptions options{false, true};

  std::cout << "Enter Remote Offer \n" << std::endl;
  auto RemoteOffer = GetInput();

  RTCSessionDescription OfferSD = {};
  OfferSD.type = RTCSdpType::Offer;
  OfferSD.sdp = RemoteOffer.c_str();

  char* error[1000];
  auto errotTyp =
      plugin.PeerConnectionSetRemoteDescription(&ctx, pco, &OfferSD, error);

  LogPrint("Create Answer");
  plugin.PeerConnectionCreateAnswer(pco, &options);
}

void SetICE(PeerConnectionObject* pco) {
  std::string sdp_mid="1";
  int sdp_mlineindex = 1;

  LogPrint("Enter ICE Candidate");
  std::string sdp = GetInput();

  webrtc::SdpParseError error;
  std::unique_ptr<webrtc::IceCandidateInterface> candidate(
      webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
  if (!candidate.get()) {
    RTC_LOG(LS_WARNING) << "Can't parse received candidate message. "
                           "SdpParseError was: "
                        << error.description;
    return;
  }
  if (!pco->connection->AddIceCandidate(candidate.get())) {
    RTC_LOG(LS_WARNING) << "Failed to apply the received candidate";
    return;
  }

  LogPrint("SetICE Complete");
}

void StartServer() {
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);
  plugin.PeerConnectionRegisterOnIceCandidate(pco, OnIceCandidate);

  //plugin.AddTracks(&ctx);

  SetRemoteOffer(pco);
  SetICE(pco);

  sleep(1000);
}

int main() {
  StartServer();
}