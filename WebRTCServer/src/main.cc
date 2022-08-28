#pragma clang diagnostic ignored "-Wunused-variable" 
#pragma clang diagnostic ignored "-Wunused-result"

#include <iostream>
#include <cstdio>

#include "WebRTCPlugin.h"
#include "Context.h"
#include "PeerConnectionObject.h"

using namespace webrtc;
int main() {
  WebRTCPlugin plugin;
  Context ctx;
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);
  plugin.AddTracks(&ctx);
  
  const RTCOfferAnswerOptions options{false, true};
  // plugin.PeerConnectionCreateOffer(pco, &options);


  std::cout << "Press Enter to Continue!" << std::endl;
  int age;
  scanf("%d", &age);
}