#pragma clang diagnostic ignored "-Wunreachable-code"
#include "Context.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include <iostream>
namespace webrtc {

using namespace webrtc;

Context::Context()
    : m_workerThread(rtc::Thread::CreateWithSocketServer()),
      m_signalingThread(rtc::Thread::CreateWithSocketServer()),
      m_taskQueueFactory(CreateDefaultTaskQueueFactory()) {
  m_workerThread->Start();
  m_signalingThread->Start();

  rtc::InitializeSSL();

  m_peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
      m_workerThread.get() /* network_thread */, m_workerThread.get() /* worker_thread */,
      m_signalingThread.get(), nullptr /* default_adm */,
      CreateBuiltinAudioEncoderFactory(),
      CreateBuiltinAudioDecoderFactory(),
      CreateBuiltinVideoEncoderFactory(),
      CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
      nullptr /* audio_processing */);
  
  if (!m_peerConnectionFactory) {
    std::cout << "Failed to initialize PeerConnectionFactory" << std::endl;
  }
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