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
  const auto observer = SSDO::Create(obj);
  context->AddObserver(obj->connection.get(), observer);
  return obj;
}

PeerConnectionObject* WebRTCPlugin::ContextCreatePeerConnection(
    Context* context) {
  PeerConnectionInterface::RTCConfiguration config = {};
  config.sdp_semantics = SdpSemantics::kUnifiedPlan;
  // config.enable_implicit_rollback = true;
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = GetPeerConnectionString();
  config.servers.push_back(server);
  return _ContextCreatePeerConnection(context, config);
}

void WebRTCPlugin::PeerConnectionRegisterCallbackCreateSD(
    PeerConnectionObject* obj,
    DelegateCreateSDSuccess onSuccess,
    DelegateCreateSDFailure onFailure) {
  obj->RegisterCallbackCreateSD(onSuccess, onFailure);
}

void WebRTCPlugin::PeerConnectionCreateOffer(
    PeerConnectionObject* obj,
    const RTCOfferAnswerOptions* options) {
  obj->CreateOffer(*options);
}

RTCErrorType WebRTCPlugin::PeerConnectionSetLocalDescription(
    Context* context,
    PeerConnectionObject* obj,
    const RTCSessionDescription* desc,
    std::string& error) {
  
  RTCErrorType errorType = obj->SetLocalDescription(
      *desc, context->GetObserver(obj->connection.get()), error);

  return errorType;
}

char* WebRTCPlugin::ConvertString(const std::string str) {
  const size_t size = str.size();
  char* ret = static_cast<char*>(CoTaskMemAlloc(size + sizeof(char)));
  str.copy(ret, size);
  ret[size] = '\0';
  return ret;
}

void WebRTCPlugin::AddTracks(Context* context) {
  context->AddTracks();
}

}  // namespace webrtc