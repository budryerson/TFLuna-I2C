/* File Name: TFLI2C_example.ino
 * Developer: Bud Ryerson
 * Date:      19 SEP 2020
 * Version:   0.0.1
 * Described: Arduino example sketch for the Benewake TF-Luna Lidar sensor
 *            configured for the I2C interface
 */
 
#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>

TFLI2C tflI2C;

int16_t  tfDist = 0 ;    // distance in centimeters
int16_t  tfFlux = 0 ;    // signal quality in arbitrary units
int16_t  tfTemp = 0 ;    // device chip temperature in Celsius
int16_t  tfAddr = 0x10;  // I2C device address
uint16_t tfFrame = 50;   // frame rate
uint16_t tfTime = 0;
uint8_t  tfVer[3]; // = { 0,0,0};   // version number
uint8_t  tfCode[14];



void exampleCommands( uint8_t adr)
{
    Serial.print( "Device Address: ");
    Serial.println( adr);

    Serial.print("System Reset: ");
    if( tflI2C.Soft_Reset( adr))
    {
        Serial.println( "Passed");
    }
    delay(500);

    Serial.print( "Get Firmware Version: ");
    if( tflI2C.Get_Firmware_Version( tfVer, adr))
    {
      Serial.print( tfVer[2]);
      Serial.print( ".");
      Serial.print( tfVer[1]);
      Serial.print( ".");
      Serial.println( tfVer[0]);
    }
    delay(500);

    Serial.print( "Get Serial Number: ");
    if( tflI2C.Get_Prod_Code( tfCode, adr))
    {
      for( uint8_t i = 0; i < 14; ++i)
      {
        Serial.print( char( tfCode[i]));
      }
      Serial.println();
    }
    delay(500);

    Serial.print( "Get Time: ");
    if( tflI2C.Get_Time( tfTime, adr))
    {
      Serial.println(  tfTime);
    }
    delay(500);

    Serial.print( "Set Frame Rate to: ");
    if( tflI2C.Set_Frame_Rate( tfFrame, adr))
    {
      Serial.println(  tfFrame);
    }
    delay(500);
    
    Serial.print( "Get Frame Rate: ");
    if( tflI2C.Get_Frame_Rate( tfFrame, adr))
    {
      Serial.println(  tfFrame);  //  Read frame rate from device
    }
    delay(500);
}

void setup()
{
    Serial.begin( 115200);
    Wire.begin();  // Initalize Wire library

    Serial.println( "TFLI2C example code: 19 Sep 2020");

    exampleCommands( tfAddr);  //  execute sample commands above
}

uint8_t tfCount = 0;
void loop()
{
    // If data is read without error
    if( tflI2C.getData( tfDist, tfFlux, tfTemp, tfAddr))
    {
        Serial.print("Dist: ");     // ...print distance.
        Serial.print(tfDist);
        Serial.print(" | Flux: ");     // ...print quality
        Serial.print(tfFlux);

        // Convert temperature from hundreths
        // of a degree to a whole number
        tfTemp = int16_t( tfTemp / 100);

        Serial.print(" | Temp: ");     // ...print temperature
        Serial.println( tfTemp);
    }
    else tflI2C.printDataArray();        // else, print error status, and reply byte.

    if( tfCount < 10) ++tfCount;
    else
    {
        Serial.print( "Get Time: ");
        tflI2C.Get_Time( tfTime, tfAddr);
        Serial.println(  tfTime);
        tfCount = 0;
    }

    delay( 50);
}
