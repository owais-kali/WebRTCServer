#pragma once
#include <iostream>
#include <fstream>

class Fifo
{
private:  
int sock;

public:
    Fifo(/* args */);
    ~Fifo();

    std::string Read();
    void Write(std::string data);
};
