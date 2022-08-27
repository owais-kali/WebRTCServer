#include <iostream>

#include "PeerConnectionObject.h"

using namespace webrtc;

int main() {
  rtc::scoped_refptr<PeerConnectionObject> obj =
      rtc::scoped_refptr<PeerConnectionObject>(
          new rtc::RefCountedObject<PeerConnectionObject>());

          std::cout << "hello world" std::endl;
}