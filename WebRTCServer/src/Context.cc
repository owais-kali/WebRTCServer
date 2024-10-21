#pragma clang diagnostic ignored "-Wunreachable-code"
#pragma clang diagnostic ignored "-Wunused-variable"
#include "Context.h"

#include <iostream>

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/task_queue/default_task_queue_factory.h"

#include <memory>

#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"

namespace webrtc {

using namespace webrtc;

Context::Context()
    : m_workerThread(rtc::Thread::CreateWithSocketServer()),
      m_signalingThread(rtc::Thread::CreateWithSocketServer()),
      m_taskQueueFactory(CreateDefaultTaskQueueFactory()) {
  m_workerThread->Start();
  m_signalingThread->Start();

  rtc::InitializeSSL();

  m_audioDevice = m_workerThread->BlockingCall(
	[&]() { return rtc::make_ref_counted<unity::webrtc::DummyAudioDevice>(m_taskQueueFactory.get()); });

  m_peerConnectionFactory = webrtc::CreatePeerConnectionFactory(
      nullptr /* network_thread */, nullptr /* worker_thread */,
      m_signalingThread.get(), m_audioDevice /* default_adm */,
      webrtc::CreateBuiltinAudioEncoderFactory(),
      webrtc::CreateBuiltinAudioDecoderFactory(),
      std::make_unique<webrtc::VideoEncoderFactoryTemplate<
          webrtc::LibvpxVp8EncoderTemplateAdapter,
          webrtc::LibvpxVp9EncoderTemplateAdapter,
          webrtc::OpenH264EncoderTemplateAdapter,
          webrtc::LibaomAv1EncoderTemplateAdapter>>(),
      std::make_unique<webrtc::VideoDecoderFactoryTemplate<
          webrtc::LibvpxVp8DecoderTemplateAdapter,
          webrtc::LibvpxVp9DecoderTemplateAdapter,
          webrtc::OpenH264DecoderTemplateAdapter,
          webrtc::Dav1dDecoderTemplateAdapter>>(),
      nullptr /* audio_mixer */, nullptr /* audio_processing */);

  if (!m_peerConnectionFactory) {
    std::cout << "Failed to initialize PeerConnectionFactory" << std::endl;
  }
}

webrtc::SdpType ConvertSdpType(RTCSdpType type) {
  switch (type) {
    case RTCSdpType::Offer:
      return webrtc::SdpType::kOffer;
    case RTCSdpType::PrAnswer:
      return webrtc::SdpType::kPrAnswer;
    case RTCSdpType::Answer:
      return webrtc::SdpType::kAnswer;
    case RTCSdpType::Rollback:
      return webrtc::SdpType::kRollback;
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

SetSessionDescriptionObserver* Context::GetObserver(
    webrtc::PeerConnectionInterface* connection) {
      return m_mapSetSessionDescriptionObserver[connection].get();
    }

PeerConnectionObject* Context::CreatePeerConnection(
    const webrtc::PeerConnectionInterface::RTCConfiguration& config) {
  rtc::scoped_refptr<PeerConnectionObject> obj =
      rtc::scoped_refptr<PeerConnectionObject>(
          new rtc::RefCountedObject<PeerConnectionObject>());

  PeerConnectionDependencies dependencies(obj.get());
  auto connection = m_peerConnectionFactory->CreatePeerConnectionOrError(
      config, std::move(dependencies));

  if (!connection.ok()) {
    std::cout << "error!" << std::endl;
    RTC_LOG(LS_ERROR) << connection.error().message();
    return nullptr;
  }
  obj->connection = std::move(connection.value());
  const PeerConnectionObject* ptr = obj.get();
  m_mapClients[ptr] = std::move(obj);

  printf("m_mapClients.count: %lu \n", m_mapClients.size());

  return m_mapClients[ptr].get();
}

void Context::AddTracks() {
  for (auto&& kv : m_mapClients) {
    kv.first->AddTracks(m_peerConnectionFactory.get());
  }
}

}  // namespace webrtc