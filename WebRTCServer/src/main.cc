#pragma clang diagnostic ignored "-Wunused-variable"
#include <iostream>

#include "WebRTCPlugin.h"
#include "Context.h"
#include "PeerConnectionObject.h"

using namespace webrtc;
int main() {
  WebRTCPlugin plugin;
    Context ctx;
  PeerConnectionObject* pco = plugin.ContextCreatePeerConnection(&ctx);
}