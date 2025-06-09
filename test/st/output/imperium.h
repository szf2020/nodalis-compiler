#pragma once
#include <cstdint>
#include <string>
#include <chrono>

extern uint64_t PROGRAM_COUNT;
extern std::chrono::steady_clock::time_point PROGRAM_START;

uint64_t elapsed();

class TP{
    public:
        bool Q;
        bool IN;
        uint64_t PT;
        uint64_t ET;
    

    void operator()(){
        Q = false;
        if(!lastIN && IN){
            lastIN = IN;
            ET = 0;
            startTime = 0;
        }
        if(IN){
            Q = true;
        }
        else if(lastIN && !IN){
            if(startTime == 0){
                startTime = elapsed();
            }
            ET = elapsed() - startTime;
            if(PT <= ET){
                Q = true;
            }
        }
    }
    private:
        bool lastIN = false;
        uint64_t startTime = 0;
};


void gatherInputs();
void handleOutputs();
uint64_t readAddress(std::string address);
void writeAddress(std::string address, uint64_t value);
bool getBit(uint64_t var, int bit);
void setBit(uint64_t* var, int bit, bool value);