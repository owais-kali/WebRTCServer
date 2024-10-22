#pragma once
#include <iostream>
#include <fstream>

class Fifo
{
private:  
std::ofstream webrtc_write;
std::ifstream webrtc_read;

public:
    Fifo(/* args */);
    ~Fifo();

    std::string Read();
    void Write(std::string& data);
    void Write(std::string data);
};
