/* File Name: TFLI2C.cpp
 * Developer: Bud Ryerson
 * Date:      11 SEP 2020
 * Version:   0.0.1
 * Described: Arduino Library for the Benewake TFLI2C Lidar sensor
 *            configured for the I2C interface
 *
 * Default settings for the TF-Luna are:
 *    0x10  -  slave device address
 *    400kHz - bus clock speed
 *    100Hz  - data frame-rate
 *    Centimeter - distance measurement format
 *    Celsius - temperature measurement scale
 *
 *  There are is one important function: 'getData'.
 *  There are several explicit commands
 *
 *  getData( dist, flux, temp, addr)
 *  Reads the disance measured, return signal strenght and temperature of the chip.
 *  PARAMETERS  - dist : uint16_t : distance measured by the device, in cm.
 *              - flux : uint16_t : confidence about the distance measured.
                If the confidence value is too low, an error will occur.
                - temp : uint16_t : temperature of the chip in 0.01 degrees C
                - addr : unsigned 8 bits : address of slave device.
 *    RETURN   If true, no error occured.
               If false, the error is defined by a status code, which can be
               displayed using 'printReply()' function.
 * NOTE : If you only want to read distance, use getData( dist, addr)
 *
 *  If the function completes without error it returns 'True' and sets
 *  a public one-byte 'tfStatus' code to zero.  Otherwise it returns 'False'
 *  and sets the 'tfStatus' code to a library defined error code.
 *  The error status can be monitored using the function 'printReply()'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *
 *
 * -------- regCommand( regName, addr, operation, data) --------  *
 * This allows you to read or write into an internal register of the TF-Luna.
 * All registers you can access are defined and explained in TFLI2C.h.
 * All parameters are unsigned 8-bit values
 *    - regName : the register you're dealing with.
 *    - addr : address of slave device.
 *    - operation : commands are WRITE_REG, READ_REG and WRITE_READ_REG.
 *    - data : The data you want to write.
 * Returns a boolean bit.
 *    If true, no error occured.
 *    If false, the error is defined by a status code,
 *    which then can be displayed using 'printReply()' function.
 *
 * NOTE : If you only want to read a register's value, use sendCommand( regName, addr).
 *
 */

#include <TFLI2C.h>        //  TFLI2C library header
#include <Wire.h>          //  Arduino I2C/Two-Wire Library

// Constructor/Destructor
TFLI2C::TFLI2C(){}
TFLI2C::~TFLI2C(){}

// - - - - - - - - - - - - - - - - - - - - - - - - - -
//             GET DATA FROM THE DEVICE
// - - - - - - - - - - - - - - - - - - - - - - - - - -
bool TFLI2C::getData( int16_t &dist, int16_t &flux, int16_t &temp, uint8_t addr)
{
    tfStatus = TFL_READY;    // clear status of any error condition

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 1 - Use the function 'regCommand' to fill the table 'frame'
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    for (uint8_t reg = TFL_DIST_LO; reg <= TFL_TEMP_HI; reg++)
    {
      if( !readReg( reg, addr)) return false;
          else frame[ reg] = regReply;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Step 2 - Interpret the frame data
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    dist = frame[ 0] + ( frame[ 1] << 8);
    flux = frame[ 2] + ( frame[ 3] << 8);
    temp = frame[ 4] + ( frame[ 5] << 8);

    // Convert temperature from hundreths
    // of a degree to a whole number
    temp = int16_t( temp / 100);

    // Convert Celsius to degrees Farenheit
    // temp = uint8_t( temp * 9 / 5) + 32;

    // - - Evaluate Abnormal Data Values - -
    // Signal strength <= 100
    if( flux < (int16_t)100)
    {
      tfStatus = TFL_WEAK;
      return false;
    }
    // Signal Strength saturation
    else if( flux == (int16_t)0xFFFF)
    {
      tfStatus = TFL_STRONG;
      return false;
    }
    else
    {
      tfStatus = TFL_READY;
      return true;
    }
}

// Get Data short version
bool TFLI2C::getData( int16_t &dist, uint8_t addr)
{
  static int16_t flux, temp;
  return getData( dist, flux, temp, addr);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - -
//              EXPLICIT COMMANDS
// - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//  = = = =    GET FIRMWARE VERSION   = = = =
// When you pass an array (ver) as a parameter to a function
// it decays into a pointer to the first element of the array.
bool TFLI2C::Get_Firmware_Version( uint8_t * ver, uint8_t adr)
{
    for (uint8_t i = 0; i < 3; ++i)
    {
      if( !readReg( ( 0x0A + i), adr)) return false;
        else ver[ i] = regReply;  // Read into version array

//      if( !regCommand( ( 0x0A + i), adr)) return false;
//          else ver[ i] = regReply;  // Read into version array
    }
    return true;
}

//  = = = = = =    SAVE SETTINGS   = = = = = =
bool TFLI2C::Save_Settings( uint8_t adr)
{
    return( writeReg( 0x20, adr, 1));
}

//  = = = =   SOFT (SYSTEM) RESET   = = = =
bool TFLI2C::Soft_Reset( uint8_t adr)
{
    return( writeReg( 0x21, adr, 2));
}

//  = = = = = =    SET I2C ADDRESS   = = = = = =
// Range: 0x08, 0x77. Must reboot to take effect.
bool TFLI2C::Set_I2C_Addr( uint8_t adrNew, uint8_t adr)
{
    return( writeReg( 0x22, adr, adrNew));
}

//  = = = = = =   SET ENABLE   = = = = = =
bool TFLI2C::Set_Enable( uint8_t adr)
{
    return( writeReg( 0x25, adr, 0));
}

//  = = = = = =   SET DISABLE   = = = = = =
bool TFLI2C::Set_Disable( uint8_t adr)
{
    return( writeReg( 0x25, adr, 1));
}

//  = = = = = =    SET FRAME RATE   = = = = = =
bool TFLI2C::Set_Frame_Rate( uint16_t &frm, uint8_t adr)
{
    // Recast the address of the unsigned integer 'frm'
    // as a pointer 'p_frm' to an unsigned byte ...
    uint8_t * p_frm = (uint8_t *) &frm;

    // ... then address the pointer as an array.
    if( !writeReg( ( 0x26), adr, p_frm[ 0])) return false;
    if( !writeReg( ( 0x27), adr, p_frm[ 1])) return false;
    return true;
}

//  = = = = = =    GET FRAME RATE   = = = = = =
bool TFLI2C::Get_Frame_Rate( uint16_t &frm, uint8_t adr)
{
    uint8_t * p_frm = (uint8_t *) &frm;
    if( !readReg( 0x26, adr)) return false;
        else p_frm[ 0] = regReply;  // Read into version array
    if( !readReg( 0x27, adr)) return false;
        else p_frm[ 1] = regReply;  // Read into version array
    return true;
}

//  = = = =   HARD RESET to Factory Defaults  = = = =
bool TFLI2C::Hard_Reset( uint8_t adr)
{
    return( writeReg( 0x29, adr, 1));
}

//  = = = = = =   SET CONTINUOUS MODE   = = = = = =
// Device will sample continuously at Frame Rate
bool TFLI2C::Set_Cont_Mode( uint8_t adr)
{
    return( writeReg( 0x23, adr, 0));
}

//  = = = = = =   SET TRIGGER MODE   = = = = = =
// Device will sample once when triggered
bool TFLI2C::Set_Trig_Mode( uint8_t adr)
{
    return( writeReg( 0x23, adr, 1));
}

//  = = = = = =   SET TRIGGER   = = = = = =
// Trigger device to sample once
bool TFLI2C::Set_Trigger( uint8_t adr)
{
    return( writeReg( 0x24, adr, 1));
}

// = = = = = = = = = = = = = = = = = = = = = = = =



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//       WRITE OR READ A GIVEN REGISTER OF THE SLAVE DEVICE
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool TFLI2C::readReg( uint8_t nmbr, uint8_t addr)
{
  Wire.beginTransmission( addr);
  Wire.write( nmbr);

  if( Wire.endTransmission() != 0)  // If write error...
  {
    tfStatus = TFL_I2CWRITE;        // then set satus code...
    return false;                   // and return "false."
  }
  // Request 1 byte from the device
  // and release bus when finished.
  Wire.requestFrom( ( int)addr, 1, true);
    if( Wire.peek() == -1)            // If read error...
    {
      tfStatus = TFL_I2CREAD;         // then set satus code...
      return false;
    }
  regReply = ( uint8_t)Wire.read(); // Read the recieved data...
  return true;
}

bool TFLI2C::writeReg( uint8_t nmbr, uint8_t addr, uint8_t data)
{
  Wire.beginTransmission( addr);
  Wire.write( nmbr);
  Wire.write( data);
  if( Wire.endTransmission( true) != 0)  // If write error...
  {
    tfStatus = TFL_I2CWRITE;        // then set satus code...
    return false;                   // and return "false."
  }
  else return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - - - - -    The following is for testing purposes    - - - -
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Called by either 'printFrame()' or 'printReply()'
// Print status condition either 'READY' or error type
void TFLI2C::printStatus()
{
    Serial.print("Status: ");
    if( tfStatus == TFL_READY)          Serial.print( "READY");
    else if( tfStatus == TFL_SERIAL)    Serial.print( "SERIAL");
    else if( tfStatus == TFL_HEADER)    Serial.print( "HEADER");
    else if( tfStatus == TFL_CHECKSUM)  Serial.print( "CHECKSUM");
    else if( tfStatus == TFL_TIMEOUT)   Serial.print( "TIMEOUT");
    else if( tfStatus == TFL_PASS)      Serial.print( "PASS");
    else if( tfStatus == TFL_FAIL)      Serial.print( "FAIL");
    else if( tfStatus == TFL_I2CREAD)   Serial.print( "I2C-READ");
    else if( tfStatus == TFL_I2CWRITE)  Serial.print( "I2C-WRITE");
    else if( tfStatus == TFL_I2CLENGTH) Serial.print( "I2C-LENGTH");
    else if( tfStatus == TFL_WEAK)      Serial.print( "Signal weak");
    else if( tfStatus == TFL_STRONG)    Serial.print( "Signal strong");
    else if( tfStatus == TFL_FLOOD)     Serial.print( "Ambient light");
    else if( tfStatus == TFL_INVALID)   Serial.print( "No Command");
    else Serial.print( "OTHER");
}


// Print error type and HEX values
// of each byte in the data frame
void TFLI2C::printFrame()
{
    printStatus();
    // Print the Hex value of each byte of data
    Serial.print(" Data:");
    for( uint8_t i = 0; i < TFL_FRAME_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( frame[ i] < 16 ? "0" : "");
      Serial.print( frame[ i], HEX);
    }
    Serial.println();
}


// Print error type and HEX values of
// each byte in the command response frame
void TFLI2C::printReply()
{
    printStatus();
    // Print the Hex value of each byte
    for( uint8_t i = 0; i < TFL_REPLY_SIZE; i++)
    {
      Serial.print(" ");
      Serial.print( reply[ i] < 16 ? "0" : "");
      Serial.print( reply[ i], HEX);
    }
    Serial.println();
}

// Prints the status and the last reply recieved
void TFLI2C::printRegReply()
{
    printStatus();
    Serial.print("REPLY : ");
    Serial.println(regReply);
    Serial.println("");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
