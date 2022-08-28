#pragma once
#include "WebRTCPlugin.h"

namespace webrtc{
    class SetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
    {
    public:
        static rtc::scoped_refptr<SetSessionDescriptionObserver> Create(PeerConnectionObject* connection);
    };
}