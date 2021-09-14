/*******************************************************************************

 Bare Conductive MPR121 library
 ------------------------------
 
 

*******************************************************************************/

#include <MPR121.h>
#include <MPR121_Datastream.h>
#include <Wire.h>

#define numElectrodes 12
#define DELIMITER ","

const uint32_t BAUD_RATE = 115200;
const uint8_t MPR121_ADDR = 0x5C;   // 0x5C is the MPR121 I2C address on the Bare Touch Board
const uint8_t MPR121_INT = 4;   // pin 4 is the MPR121 interrupt on the Bare Touch Board

const bool MPR121_DATASTREAM_ENABLE = false;
  
void setup()
{ 
  Serial.begin(BAUD_RATE);
  pinMode(LED_BUILTIN, OUTPUT);  
  
  if(!MPR121.begin(MPR121_ADDR)){ 
    Serial.println("error setting up MPR121");  
    switch(MPR121.getError()){
      case NO_ERROR:
        Serial.println("no error");
        break;  
      case ADDRESS_UNKNOWN:
        Serial.println("incorrect address");
        break;
      case READBACK_FAIL:
        Serial.println("readback failure");
        break;
      case OVERCURRENT_FLAG:
        Serial.println("overcurrent on REXT pin");
        break;      
      case OUT_OF_RANGE:
        Serial.println("electrode out of range");
        break;
      case NOT_INITED:
        Serial.println("not initialised");
        break;
      default:
        Serial.println("unknown error");
        break;      
    }
    while(1);
  }
  
  MPR121.setInterruptPin(MPR121_INT);
  if (MPR121_DATASTREAM_ENABLE) 
  {
    MPR121.restoreSavedThresholds();
//    MPR121_Datastream.begin(&Serial);
  } 
  else 
  {
// this is the touch threshold - setting it low makes it more like a proximity trigger
// default value is 40 for touch    
    MPR121.setTouchThreshold(6); 
// this is the release threshold - must ALWAYS be smaller than the touch threshold
// default value is 20 for touch    
    MPR121.setReleaseThreshold(3);
  }
  
  MPR121.setFFI(FFI_10);
  MPR121.setSFI(SFI_10);
  MPR121.setGlobalCDT(CDT_4US);
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  MPR121.autoSetElectrodes();
  digitalWrite(LED_BUILTIN, LOW);
// initial data update
  MPR121.updateAll();  
}

void loop()
{   
//  if(MPR121.touchStatusChanged())
//  {
    MPR121.updateAll();
    String signalStr;              
    for (int iElectrode =0; iElectrode<numElectrodes; iElectrode++)
    {
//          MPR121.updateFilteredData();
      if (MPR121.isNewRelease(iElectrode))
      {
        signalStr += String(0);        
      } 
      else
      {
        int signalVal = MPR121.getBaselineData(iElectrode) - MPR121.getFilteredData(iElectrode);
  
        if (signalVal > 3)
        {
          signalStr += String(signalVal);  
        }
        else
        {
          signalStr += String(0);
        }
      }
      if ((iElectrode+1) < numElectrodes)
      { // Printing delimiter between each value.
        signalStr += String(DELIMITER);        
        // Serial.print(DELIMITER);
      }
    }
    Serial.print(signalStr);    
    Serial.println();
    delay(0);    
//  }  
}
