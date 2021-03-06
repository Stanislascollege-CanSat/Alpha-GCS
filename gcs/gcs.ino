// INCLUDES
#include <ArduinoSTL.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

// RADIO CHANNELS
const unsigned short int RH_CHANNEL_GS_ALPHA = 1;   //
const unsigned short int RH_CHANNEL_GS_DELTA = 2;   //
const unsigned short int RH_CHANNEL_MU = 3;         // Available radio-network-channels
const unsigned short int RH_CHANNEL_BETA = 4;       //
const unsigned short int RH_CHANNEL_RHO = 5;        //

const unsigned short int RH_CHANNEL_LOCAL = RH_CHANNEL_GS_ALPHA; // Set local channel, used by the programme

// PIN DEFINITIONS
const unsigned short int PIN_RH_RST = 2;    //
const unsigned short int PIN_RH_CS = 4;     // Setting: RHDriver pins
const unsigned short int PIN_RH_INT = 3;    //

const float RHDriverFreq = 868.0;   // RHDriver Frequency

// RADIO DECLARATION
RH_RF95 RHDriver(PIN_RH_CS, PIN_RH_INT);
RHReliableDatagram RHNetwork(RHDriver, RH_CHANNEL_LOCAL);

bool reading;
String readCommand;

//
// SETUP FUNCTION
//

void setup(){
  // --------------- Set RF reset HIGH -------------------- //
  pinMode(PIN_RH_RST, OUTPUT);
  digitalWrite(PIN_RH_RST, HIGH);

  // --------------- Starting serial @ 115200 -------------------- //
  Serial.begin(115200);
  while(!Serial);
  Serial.print("{SGS:2;F:LOG,[GCS] Alpha started on @RHchannel " + String(RH_CHANNEL_LOCAL)+";}");

  // --------------- Force RFM95W reset -------------------- //
  digitalWrite(PIN_RH_RST, LOW);
  delay(10);
  digitalWrite(PIN_RH_RST, HIGH);
  delay(10);

  // --------------- Initializing RH_Datagram -------------------- //
  if(!RHNetwork.init()){
    Serial.print("{SGR:0;}");
    while(1);
  }
  Serial.print("{SGR:2;}");

  // --------------- Setting RH_Driver frequency -------------------- //

  if(!RHDriver.setFrequency(RHDriverFreq)){
    //Serial.println("ERR: 12 -> RHDriver setFrequency failed. Check the connection with the radio chip.");
    Serial.print("{SGF:0;}");
    while(1);
  }
  Serial.print("{SGF:2;F:LOG,[GCS] Frequency set to: "); Serial.print(RHDriverFreq); Serial.print("MHz;}");

  // --------------- Setting RH_Driver TxPower to 23 (maximum) -------------------- //

  RHDriver.setTxPower(23, false);
  Serial.print("{SGP:2;}");

  // --------------- Setting #retries for RH_Datagram -------------------- //

  RHNetwork.setRetries(2);

  // --------------- Setting duration timeout for RH_Datagram -------------------- //

  RHNetwork.setTimeout(0);


  reading = false;
  readCommand = "";

}

//
// LOOP FUNCTION
//

void loop(){
  // RECEIVE COMMAND FROM COMPUTER
  if(Serial.available()){
    String reader = Serial.readString();  
    std::vector<String> commandList;
    for(int i = 0; i < reader.length(); ++i){
      switch(reader.charAt(i)){
        case '[':
          readCommand = "";
          reading = true;
          break;
        case ']':
          reading = false;
          commandList.push_back(readCommand);
          break;
        default:
          if(reading){
            readCommand += reader.charAt(i);
          }
          break;
      }
    }
    // GATHERED COMMANDS FROM COMPUTER (format: [command_name]) in std::vector<String> commandList.

    // CYCLING THROUGH commandList AND EXECUTING ALL COMMANDS
    for(String a : commandList){
      String s = "[";
      s += a;
      s += "]";
      //Serial.print("{F:LOG,Received... " + s + ";}");
      if (s.equals("[testcom]")){
        //Serial.println("testcom");
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_BETA);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_RHO);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[SAD]")) {
        //Serial.print("{F:LOG,"+ s + ";}");
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_BETA);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_RHO);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[DEP]")) {
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[OPR]")) {
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[CLR]")) {
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[OPP]")) {
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[CLP]")) {
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.waitPacketSent();
      } else if (s.equals("[FLIGHT_MODE]")){
        //Serial.print("{F:LOG,YES;}");
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_MU);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_BETA);
        RHNetwork.sendtoWait((uint8_t*)s.c_str(), s.length(), RH_CHANNEL_RHO);
        RHNetwork.waitPacketSent();
      }
    }
  }//End: if(Serial.available()){

  uint8_t BUF[RH_RF95_MAX_MESSAGE_LEN] = "";
  uint8_t LEN = sizeof(BUF);
  uint8_t FROM_ADDRESS;
  uint8_t TO_ADDRESS;

  if(RHNetwork.recvfromAck(BUF, &LEN, &FROM_ADDRESS, &TO_ADDRESS)){
    Serial.print((char*) BUF);
  }

  //Serial.print("{CAN:" + String(RH_CHANNEL_LOCAL) + ";RS:" + String(RHDriver.lastRssi()) + ";}");
}
