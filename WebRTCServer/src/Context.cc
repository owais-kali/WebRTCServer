
#include "Context.h"

namespace webrtc
{

    RTCSdpType ConvertSdpType(webrtc::SdpType type)
    {
        switch (type)
        {
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

    // PeerConnectionObject* Context::CreatePeerConnection(
    //         const webrtc::PeerConnectionInterface::RTCConfiguration& config)
    // {
    //     rtc::scoped_refptr<PeerConnectionObject> obj =
    //             new rtc::RefCountedObject<PeerConnectionObject>(*this);
    //     PeerConnectionDependencies dependencies(obj);
    //     auto connection = m_peerConnectionFactory->CreatePeerConnectionOrError(
    //             config, std::move(dependencies));
    //     if (!connection.ok())
    //     {
    //         RTC_LOG(LS_ERROR) << connection.error().message();
    //         return nullptr;
    //     }
    //     obj->connection = connection.MoveValue();
    //     const PeerConnectionObject* ptr = obj.get();
    //     m_mapClients[ptr] = std::move(obj);
    //     return m_mapClients[ptr].get();
    // }
}