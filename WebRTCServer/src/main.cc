#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"
#pragma clang diagnostic ignored "-Wunreachable-code"

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "Context.h"
#include "PeerConnectionObject.h"
#include "WebRTCPlugin.h"
#include "json/json.h"

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

void SetICE() {}

void StartServer() {
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);
  plugin.PeerConnectionRegisterOnIceCandidate(pco, OnIceCandidate);

  plugin.AddTracks(&ctx);
  CreateOffer(pco);
  // SetRemoteOffer(pco);

  sleep(1000);
}

int TestJson() {
  const std::string rawJson = R"({"Age": 20, "Name": "colin"})";
  const auto rawJsonLength = static_cast<int>(rawJson.length());
  constexpr bool shouldUseOldWay = false;
  JSONCPP_STRING err;
  Json::Value root;

  if (shouldUseOldWay) {
    Json::Reader reader;
    reader.parse(rawJson, root);
  } else {
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
                       &err)) {
      std::cout << "error" << std::endl;
      return EXIT_FAILURE;
    }
  }
  const std::string name = root["Name"].asString();
  const int age = root["Age"].asInt();

  std::cout << name << std::endl;
  std::cout << age << std::endl;

  return EXIT_SUCCESS;
}

int main() {
  
  return 0;
}