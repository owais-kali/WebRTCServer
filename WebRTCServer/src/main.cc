#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"



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

    default:
      // TODO:
      break;
  }
}

int main() {
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  const RTCOfferAnswerOptions options{false, true};

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);

  plugin.AddTracks(&ctx);

  plugin.PeerConnectionCreateOffer(pco, &options);

  std::cout << "Press Enter to Continue!" << std::endl;
  int age;
  scanf("%d", &age);
}