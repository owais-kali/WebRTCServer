#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"

#include <unistd.h>

#include <cstdio>
#include <iostream>

#include "Context.h"
#include "PeerConnectionObject.h"
#include "WebRTCPlugin.h"

using namespace webrtc;

void OnSuccess(PeerConnectionObject* pco, RTCSdpType type, const char* sdp){
  std::cout << "callback: \n" << sdp << std::endl;
}

    int main() {
  WebRTCPlugin plugin;
  Context ctx;
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  const RTCOfferAnswerOptions options{false, true};

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);

  plugin.PeerConnectionCreateOffer(pco, &options);

  // plugin.AddTracks(&ctx);

  std::cout << "Press Enter to Continue!" << std::endl;
  int age;
  scanf("%d", &age);
}