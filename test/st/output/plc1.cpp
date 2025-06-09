#include "imperium.h"
#include "modbus.h"
#include <chrono>
#include <thread>
#include <cstdint>
// Global variable declarations
uint64_t SW0001;

class PLS {//FUNCTION_BLOCK:PLS
public:
bool EN;
int16_t PT;
bool Q;
static TP Timer;
int16_t Time;
  void operator()() {
    Timer();
    Timer.IN = EN;
    Timer.PT = PT;
    Q = ( Timer.Q );
  }
};

void PLC_LD() { //PROGRAM:PLC_LD
static TP TP00010;
TP00010();
TP00010.IN = ! getBit(IL0001, 0) && ( ( getBit(SW0001, 0) ) );
TP00010.PT = 1000;
Time = ( TP00010.ET );
writeAddress("%Q0.0", ! ( ( ( Time > 2000 ) ) || TP00010.Q ));
}


int main() {
  uint cycleCount = 0;
  while (true) {
    gatherInputs();
    
    if(cycleCount % 100 == 0){
        PLC_LD();

    }

    handleOutputs();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    cycleCount++;
    if(cycleCount >= 604800000){
        cycleCount = 0;
    }
   }
  return 0;
}