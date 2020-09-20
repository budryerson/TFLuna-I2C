/* File Name: TFLI2C_simple.ino
 * Developer: Bud Ryerson
 * Date:      19 SEP 2020
 * Version:   0.0.1
 * Described: Simplified Arduino example sketch for the Benewake TF-Luna 
 *            Lidar sensor configured for the I2C interface
 */
 
#include <Arduino.h>     // Every sketch needs this
#include <Wire.h>        // Instantiate the Wire library
#include <TFLI2C.h>      // TFLuna I2C Library v.0.0.1

TFLI2C tflI2C;

int16_t  tfDist = 0 ;    // distance in centimeters
int16_t  tfAddr = 0x10;  // I2C device address

void setup()
{
    Serial.begin( 115200);
    Wire.begin();  // Initalize Wire library
    // Say "hello!"
    Serial.println( "TFLI2C simplified example code");
    Serial.println( "19 Sep 2020");
}

void loop()
{
    // If data is read without error
    if( tflI2C.getData( tfDist, tfAddr))
    {
        Serial.print("Dist: ");
        Serial.println(tfDist);        // ...print the distance.
    }
    else tflI2C.printFrame();        // else, print error status and data frame
    
    delay( 50);
}
