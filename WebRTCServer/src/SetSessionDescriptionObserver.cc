#include "SetSessionDescriptionObserver.h"

#include "PeerConnectionObject.h"
#include "pch.h"

using namespace webrtc;

namespace server {
rtc::scoped_refptr<SetSessionDescriptionObserver>
SetSessionDescriptionObserver::Create(PeerConnectionObject* connection) {
  return rtc::scoped_refptr<SetSessionDescriptionObserver>(
      new rtc::RefCountedObject<SetSessionDescriptionObserver>(connection));
}
}  // namespace server