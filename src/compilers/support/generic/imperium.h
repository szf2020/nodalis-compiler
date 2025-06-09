#pragma once
#include <cstdint>
#include <string>

class TP{
    public:
        bool Q;
        bool IN;
        int16_t PT;
        int16_t ET;
    void operator()(){

    }
};


void gatherInputs();
void handleOutputs();
uint64_t readAddress(std::string address);
void writeAddress(std::string address, uint64_t value);
