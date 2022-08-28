#pragma once
#include "WebRTCPlugin.h"

namespace server{
    class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
    {
    public:
        static rtc::scoped_refptr<SetSessionDescriptionObserver> Create(PeerConnectionObject* connection);
    };
}