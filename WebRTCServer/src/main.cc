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
  RTCSessionDescription desc = {};
  desc.sdp = sdp;
  desc.type = type;
  std::string error;
  plugin.PeerConnectionSetLocalDescription(&ctx, pco, &desc, error);
  std::cout << "error: "<<error << std::endl;
}

int main() {
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);

  const RTCOfferAnswerOptions options{false, true};

  plugin.PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);

  plugin.PeerConnectionCreateOffer(pco, &options);

  // plugin.AddTracks(&ctx);

  std::cout << "Press Enter to Continue!" << std::endl;
  int age;
  scanf("%d", &age);
}