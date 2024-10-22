#include "Fifo.h"

Fifo::Fifo()
:webrtc_write("/tmp/webrtc_write"),webrtc_read("/tmp/webrtc_read")
{

}

Fifo::~Fifo(){
    webrtc_read.close();
    webrtc_write.close();
}

std::string Fifo::Read(){
    std::string data;
    std::getline(webrtc_read, data);
    return data;
}

void Fifo::Write(std::string& data){
    webrtc_write.write(data.c_str(), data.length());
}

void Fifo::Write(std::string data){
    webrtc_write.write(data.c_str(), data.length());
}