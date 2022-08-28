//
// Created by owais on 25/8/22.
//
#pragma clang diagnostic ignored "-Wunused-variable"
#include "WebRTCPlugin.h"
#include "Context.h"
#include "SSDO.h"

namespace webrtc {
PeerConnectionObject* WebRTCPlugin::_ContextCreatePeerConnection(
    Context* context,
    const PeerConnectionInterface::RTCConfiguration& config) {
    const auto obj = context->CreatePeerConnection(config);
  if (obj == nullptr)
    return nullptr;
  const auto observer =
      SSDO::Create(obj);
  context->AddObserver(obj->connection.get(), observer);
  return obj;
}

PeerConnectionObject* WebRTCPlugin::ContextCreatePeerConnection(
    Context* context) {
  PeerConnectionInterface::RTCConfiguration config;
  config.sdp_semantics = SdpSemantics::kUnifiedPlan;
  config.enable_implicit_rollback = true;
  return _ContextCreatePeerConnection(context, config);
}
}  // namespace webrtc