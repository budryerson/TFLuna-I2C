/* File Name: TFLI2C_example.ino
 * Developer: Bud Ryerson
 * Date:      25 SEP 2020
 * Version:   0.1.0
 * Described: Arduino example sketch for the Benewake TF-Luna
 *            Lidar sensor configured for the I2C interface
 */
 
#include <Arduino.h>
#include <Wire.h>
#include <TFLI2C.h>  // TFLuna-I2C Library v.0.1.0

TFLI2C tflI2C;

// program variables
int16_t  tfAddr = TFL_DEF_ADR;    // default I2C address
uint16_t tfFrame = TFL_DEF_FPS;   // default frame rate
// device variables passed back by getData
int16_t  tfDist = 0 ;   // distance in centimeters
int16_t  tfFlux = 0 ;   // signal quality in arbitrary units
int16_t  tfTemp = 0 ;   // temperature in 0.01 degree Celsius
// other device variables
uint16_t tfTime = 0;    // devie clock in milliseconds
uint8_t  tfVer[3];      // device version number
uint8_t  tfCode[14];    // device serial number
// sub-loop counter for Time display
uint8_t tfCount = 0;

//  Several sample commands
//  run at seetup as examples
void sampleCommands( uint8_t adr)
{
    Serial.print( "Device Address: ");
    Serial.println( adr);

    Serial.print("System Reset: ");
    if( tflI2C.Soft_Reset( adr))
    {
        Serial.println( "Passed");
    }
    else tflI2C.printStatus();  // 'printStatus()' is for troubleshooting
    delay(500);                 //  are is not usually necessary

    Serial.print( "Get Firmware Version: ");
    if( tflI2C.Get_Firmware_Version( tfVer, adr))
    {
      Serial.print( tfVer[2]);
      Serial.print( ".");
      Serial.print( tfVer[1]);
      Serial.print( ".");
      Serial.println( tfVer[0]);
    }
    else tflI2C.printStatus();    
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
    else tflI2C.printStatus();
    delay(500);

    Serial.print( "Get Time: ");
    if( tflI2C.Get_Time( tfTime, adr))
    {
      Serial.println(  tfTime);
    }
    else tflI2C.printStatus();
    delay(500);

    Serial.print( "Set Frame Rate to: ");
    if( tflI2C.Set_Frame_Rate( tfFrame, adr))
    {
      Serial.println(  tfFrame);
    }
    else tflI2C.printStatus();
    delay(500);
    
    Serial.print( "Get Frame Rate: ");
    if( tflI2C.Get_Frame_Rate( tfFrame, adr))
    {
      Serial.println(  tfFrame);  //  Read frame rate from device
    }
    else tflI2C.printStatus();
    delay(500);

}

void setup()
{
    Serial.begin( 115200);  // Initialize Serial port
    Wire.begin();           // Initalize Wire library

    Serial.println( "TFLI2C example code: 25 Sep 2020");

    sampleCommands( tfAddr);  //  execute some sample commands
}


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
    else tflI2C.printStatus();        // else, print error status

    // Every ten loops this prints device time
    // in milliseconds and resets the counter.
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
