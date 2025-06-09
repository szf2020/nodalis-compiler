#include "imperium.h"
#include <iostream>
#include <map>

uint64_t PROGRAM_COUNT = 0;
std::chrono::steady_clock::time_point PROGRAM_START = std::chrono::steady_clock::now();
uint64_t elapsed() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - PROGRAM_START
    ).count();
}

void gatherInputs(){

}

void handleOutputs(){

}

void writeAddress(std::string address, uint64_t value){

}

uint64_t readAddress(std::string address){
    return 0;
}

bool getBit(uint64_t var, int bit){
    return true;
}
void setBit(uint64_t* var, int bit, bool value){

}