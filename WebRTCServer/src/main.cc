#include <iostream>

#include "WebRTCPlugin.h"
#include "Context.h"

using namespace webrtc;
int main() {
  WebRTCPlugin plugin;
  plugin.Print();
    Context ctx;
  plugin.ContextCreatePeerConnection(&ctx);
}