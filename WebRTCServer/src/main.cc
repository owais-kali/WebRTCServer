#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "Context.h"
#include "PeerConnectionObject.h"
#include "WebRTCPlugin.h"

#include <thread>
#include <chrono>

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

    default:
      // TODO:
      break;
  }
}

void OnIceCandidate(PeerConnectionObject* pco,
                    const char* candidate,
                    const char* sdpMid,
                    const int sdpMlineIndex) {
  LogPrint("OnIceCandidate\n");
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
  for (std::vector<std::string>::const_iterator itr = v.begin();
       itr != v.end(); ++itr) {
    ss << *itr;
    ss << std::endl;
  }

  return ss.str();
}

int main() {
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  const RTCOfferAnswerOptions options{false, true};

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);
  plugin.PeerConnectionRegisterOnIceCandidate(pco, OnIceCandidate);

  plugin.AddTracks(&ctx);

  // plugin.PeerConnectionCreateOffer(pco, &options);

	auto csdo = unity::webrtc::CreateSessionDescriptionObserver::Create(pco);
	pco->CreateOffer(RTCOfferAnswerOptions(), csdo.get());

  std::this_thread::sleep_for(std::chrono::seconds(60));
  return 0;
}