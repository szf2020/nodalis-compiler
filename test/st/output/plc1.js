
// Global variable declarations
let SW1 = createReference("%IX0.0");
let SW2 = createReference("%IX0.1");
class PLS { // FUNCTION_BLOCK:PLS
  constructor() {
    this.EN = null;
    this.PT = null;
    this.Q = null;
    this.Timer = newStatic("PLS.Timer", TP);
    this.Time = null;
  }
  call() {
    this.Timer.call();
    this.Timer.IN = resolve(this.EN);
    this.Timer.PT = resolve(this.PT);
    this.Q = ( resolve(this.Timer.Q) );
  }
}
function PLC_LD() { // PROGRAM:PLC_LD
let PLS1 = newStatic("PLS1", TP);
false
PLC_LD
PLS1.call();
PLS1.IN = resolve(SW1);
PLS1.PT = 1000;
writeBit("%QX0.0", ( ( resolve(SW2) ) || resolve(PLS1.Q) ));
}

async function setup(){
    mapIO("{\"ModuleID\":\"192.168.9.17\",\"ModulePort\":\"5502\",\"Protocol\":\"MODBUS-TCP\",\"RemoteAddress\":\"0\",\"RemoteSize\":\"1\",\"InternalAddress\":\"%IX0.0\",\"Resource\":\"PLC1\",\"PollTime\":\"500\",\"ProtocolProperties\":\"{}\"}");
mapIO("{\"ModuleID\":\"192.168.9.17\",\"ModulePort\":\"5502\",\"Protocol\":\"MODBUS-TCP\",\"RemoteAddress\":\"16\",\"RemoteSize\":\"1\",\"InternalAddress\":\"%QX0.0\",\"Resource\":\"PLC1\",\"PollTime\":\"500\",\"ProtocolProperties\":\"{}\"}");
mapIO("{\"ModuleID\":\"opc.tcp://localhost:4334/UA/imperium\",\"ModulePort\":\"0\",\"Protocol\":\"OPCUA\",\"RemoteAddress\":\"Input1\",\"RemoteSize\":\"1\",\"InternalAddress\":\"%IX0.1\",\"Resource\":\"PLC1\",\"PollTime\":\"1000\",\"ProtocolProperties\":\"{}\"}");


    
    log("PLC1 is running!");
}

function run(){
     
    
    
        PLC_LD();

    

    
}
