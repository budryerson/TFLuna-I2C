/* File Name: TFLI2C.h
 * Developer: Bud Ryerson
 * Date:      05 SEP 2020
 * Version:   0.0.1
 * Described: Arduino Library for the Benewake TF-Luna Lidar sensor
 *            configured for the I2C interface
 *
 * Default settings for the TF-Luna are:
 *    0x10  -  slave device address
 *    400kHz - bus clock speed
 *    100Hz  - data frame-rate
 *    Centimeter - distance measurement format
 *    Celsius - temperature measurement scale
 *
 *  There are only two important functions: 'sendCommand' and 'getMeasures'.
 *
 *  * -------- sendCommand( regName, addr, operation, data) --------  *
 *    SUMMARY     : This is a basic I2C function. It allows you to read or write into an internal register of the TF-Luna.
 *                  All registers you can access are defined and explained in TFLuna.h.
 *    PARAMETERS  : all parameters are unsigned 8-bit values
 *                - regName : the register you're dealing with.
 *                - addr : address of slave device.
 *                - operation : Choose between write, read, and write then read : commands are WRITE_REG, READ_REG and WRITE_READ_REG.
 *                - data : The data you want to write.
 *    RETURN      : A boolean bit. If true, no error occured.
 *                  If false, the error is defined by a status code, which then can be displayed using 'printReply()' function.
 * NOTE : If you only want to read a register's value, use sendCommand( regName, addr).
 *
 * - - - - - - -  getData( dist, flux, temp, addr)  - - - - - - - - *
 *  Get disance, luminosity or quality of returned signal and internal chip temperature
 *  The function uses 'sendCommand()' to propmt the TF-Luna for six register values
 *  and interpret the results.
 *    PARAMETERS  : - dist : int16_t : distance measured by the device, in cm.
 *                  - flux : int16_t : quality of the returned signal or error
 *                  - temp : int16_t : temperature of the chip in arbbitrary units
 *                  - addr : uint8_t : address of device.
 *    RETURN      : Identical to 'sendCommand()' : A boolean bit. If true, no error occured.
 *                  If false, the error is defined by a status code, which then can be displayed using 'printReply()' function.
 * NOTE : If you only want to read distance, use getMeasures( *dist, addr)

 *  If the function completes without error it returns 'True' and sets
 *  a public one-byte 'status' code to zero.  Otherwise it returns 'False'
 *  and sets the 'status' code to a library defined error code.
 *  The error status can be monitored using the function 'printReply()'
 *
 *  NOTE: By default the I2C device address is set to 0x10. If you need
 *  to address multiple devices or need to change the default address for
 *  any reason, your code must thereafter include an unsigned, 8-bit
 *  'addr' value at the end of every call to 'getData()' or 'sendCommand()'.
 *
 */

#include <Arduino.h>    // Always include this. It's important.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Defines to use with 'sendCommand()' function
// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define TFL_DEFAULT_ADDR     0x10   // default I2C slave address

// Buffer size definitions
#define TFL_FRAME_SIZE         6   // Size of data frame = 9 bytes
#define TFL_REPLY_SIZE         6   // Longest command reply = 8 bytes
#define TFL_COMMAND_MAX        6   // Longest command = 8 bytes

// - - - -   Register Names and Numbers   - - - -
#define TFL_DIST_LO          0x00  //R Unit: cm
#define TFL_DIST_HI          0x01  //R
#define TFL_FLUX_LO          0x02  //R
#define TFL_FLUX_HI          0x03  //R
#define TFL_TEMP_LO          0x04  //R Unit: 0.01 Celsius
#define TFL_TEMP_HI          0x05  //R
#define TFL_TICK_LO          0x06  //R Timestamp
#define TFL_TICK_HI          0x07  //R
#define TFL_ERR_LO           0x08  //R
#define TFL_ERR_HI           0x09  //R
#define TFL_VER_REV          0x0A  //R
#define TFL_VER_MIN          0x0B  //R
#define TFL_VER_MAJ          0x0C  //R

#define TFL_SAVE_SETTINGS    0x20  //W -- Write 0x01 to save
#define TFL_SOFT_RESET       0x21  //W -- Write 0x02 to reboot.
                       // Lidar not accessible during few seconds,
                       // then register value resets automatically
#define TFL_SET_I2C_ADDR     0x22  //W/R -- Range 0x08,0x77.
                       // Must reboot to take effect.
#define TFL_SET_TRIG_MODE    0x23  //W/R -- 0-continuous, 1-trigger
#define TFL_TRIGGER          0x24  //W  --  1-trigger once
#define TFL_DISABLE          0x25  //W/R -- 0-enable, 1-disable
#define TFL_FPS_LO           0x26  //W/R -- lo byte
#define TFL_FPS_HI           0x27  //W/R -- hi byte
#define TFL_SET_LO_PWR       0x28  //W/R -- 0-normal, 1-low power
#define TFL_HARD_RESET       0x29  //W  --  1-restore factory settings

///////// Read or write ? //////////
#define WRITE_REG               0
#define READ_REG                1
#define WRITE_READ_REG          2
/////// TFL_POWER_MODE //////
#define TFL_PWR_LO            1
#define TFL_PWR_HI            0

/////// FPS (Low Power Mode) ///////
#define FPS_1                   0x01
#define FPS_2                   0x02
#define FPS_3                   0x03
#define FPS_4                   0x04
#define FPS_5                   0x05
#define FPS_6                   0x06
#define FPS_7                   0x07
#define FPS_8                   0x08
#define FPS_9                   0x09
#define FPS_10                  0x0A

////// FPS (High Power Mode) /////
#define FPS_35                  0x23
#define FPS_50                  0x32
#define FPS_100                 0x64
#define FPS_125                 0x7D
#define FPS_250                 0xFA

// Error Status Condition definitions
#define TFL_READY            0  // no error
#define TFL_SERIAL           1  // serial timeout
#define TFL_HEADER           2  // no header found
#define TFL_CHECKSUM         3  // checksum doesn't match
#define TFL_TIMEOUT          4  // I2C timeout
#define TFL_PASS             5  // reply from some system commands
#define TFL_FAIL             6  //           "
#define TFL_I2CREAD          7
#define TFL_I2CWRITE         8
#define TFL_I2CLENGTH        9
#define TFL_WEAK            10  // Signal Strength ≤ 100
#define TFL_STRONG          11  // Signal Strength saturation
#define TFL_FLOOD           12  // Ambient Light saturation
#define TFL_MEASURE         13
#define TFL_INVALID         14  // Invalid operation sent to sendCommand()


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//                 OBJECT CLASS DEFINITIONS
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

class TFLI2C
{
  public:
    TFLI2C();
    ~TFLI2C();

/*    // = = = = = =  TF-Luna Data Structure  = = = = = = =
    //  A three dimensional structure of bytes and
    //  integers used by the TF-Luna I2C Library
    struct bytBuf_s {
      union {
        struct datBuf_s {
          int16_t dst; // distance
          int16_t flx; // signal strength or quality
          int16_t tmp; // temperature
          int16_t tck; // time
          int16_t err; // error
          uint8_t[3] ver; // version
        };
        uint8_t[26]; dat  // data buffer array
      };
    };
    // - - - -  End of Big Data Structure  - - - - -
*/

//    uint8_t tfAddr;          // slave device I2C address
    uint8_t tfVersion[ 3];   // three digit firmware version
    uint8_t tfStatus;        // system error status: READY = 0

    // Get data
    bool getData( int16_t &dist, int16_t &flux, int16_t &temp, uint8_t addr);
    // Get data short version
    bool getData( int16_t &dist, uint8_t addr);

    // Send an I2C register command
		bool readReg( uint8_t nmbr, uint8_t addr);
  	bool writeReg( uint8_t nmbr, uint8_t addr, uint8_t data);

    // Explicit Device Commands
    bool Get_Firmware_Version( uint8_t ver[], uint8_t adr);
    bool Get_Frame_Rate( uint16_t &frm, uint8_t adr);
    bool Set_Frame_Rate( uint16_t &frm, uint8_t adr);
    bool Set_I2C_Addr( uint8_t adrNew, uint8_t adr);
    bool Set_Enable( uint8_t adr);
    bool Set_Disable( uint8_t adr);
    bool Soft_Reset( uint8_t adr);  // Reset and reboot
    bool Hard_Reset( uint8_t adr);  // Restore factory defaults
    bool Save_Settings( uint8_t adr);
    bool Set_Trig_Mode( uint8_t adr);
    bool Set_Cont_Mode( uint8_t adr);
    bool Set_Trigger( uint8_t adr);  // false = continuous

    //  For testing purposes: print reply data and status
    void printFrame();
    void printReply();
    void printRegReply();

  private:
    uint8_t frame[ TFL_FRAME_SIZE + 1];
    uint8_t reply[ TFL_REPLY_SIZE + 1];

//    uint16_t chkSum;       // calculate the check sum byte.
//    uint8_t replyLen;      // store reply data length
//    uint8_t cmndLen;       // store command data length
//    uint8_t cmndData[ TFL_COMMAND_MAX]; // store command data

    void printStatus();

    uint8_t regFrame[6];
    uint8_t regReply;
};

