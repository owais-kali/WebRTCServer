#pragma clang diagnostic ignored "-Wunreachable-code"
#include "Context.h"

namespace webrtc {

using namespace webrtc;

Context::Context(){
  
}

RTCSdpType ConvertSdpType(webrtc::SdpType type) {
  switch (type) {
    case webrtc::SdpType::kOffer:
      return RTCSdpType::Offer;
    case webrtc::SdpType::kPrAnswer:
      return RTCSdpType::PrAnswer;
    case webrtc::SdpType::kAnswer:
      return RTCSdpType::Answer;
    case webrtc::SdpType::kRollback:
      return RTCSdpType::Rollback;
    default:
      // throw std::invalid_argument("Unknown SdpType");
      return RTCSdpType::Rollback;
  }
}

void Context::AddObserver(
    const webrtc::PeerConnectionInterface* connection,
    const rtc::scoped_refptr<SetSessionDescriptionObserver>& observer) {
  m_mapSetSessionDescriptionObserver[connection] = observer;
}

PeerConnectionObject* Context::CreatePeerConnection(
    const webrtc::PeerConnectionInterface::RTCConfiguration& config) {
  rtc::scoped_refptr<PeerConnectionObject> obj =
      rtc::scoped_refptr<PeerConnectionObject>(
          new rtc::RefCountedObject<PeerConnectionObject>());

  PeerConnectionDependencies dependencies(obj.get());
  // auto connection = m_peerConnectionFactory->CreatePeerConnectionOrError(
  //     config, std::move(dependencies));


  // if (!connection.ok()) {
  //   std::cout << "error!" << std::endl;
  //   RTC_LOG(LS_ERROR) << connection.error().message();
  //   return nullptr;
  // }
  // obj->connection = connection.MoveValue();
  // const PeerConnectionObject* ptr = obj.get();
  // m_mapClients[ptr] = std::move(obj);
  // return m_mapClients[ptr].get();
  return nullptr;
}
}  // namespace webrtc