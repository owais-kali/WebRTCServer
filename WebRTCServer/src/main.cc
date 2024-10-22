#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-result"
#pragma clang diagnostic ignored "-Wunreachable-code"

#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>


#include "Context.h"
#include "Fifo.h"
#include "Logger.h"

using namespace webrtc;
using namespace unity::webrtc;

Fifo fifo;
std::unique_ptr<webrtc::Clock> s_clock;

namespace webrtc {
	PeerConnectionInterface::PeerConnectionState PeerConnectionState;
}

PeerConnectionObject* ContextCreatePeerConnection(Context* context) {
  PeerConnectionInterface::RTCConfiguration config;
  config.sdp_semantics = SdpSemantics::kUnifiedPlan;
  config.enable_implicit_rollback = true;
  config.set_suspend_below_min_bitrate(false);
  webrtc::PeerConnectionInterface::IceServer server;

  return context->CreatePeerConnection(config);
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