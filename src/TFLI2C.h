/* File Name: TFLI2C.h
 * Developer: Bud Ryerson
 * Date:      19 SEP 2020
 * Version:   0.0.1
 * Described: Arduino Library for the Benewake TF-Luna Lidar sensor
 *            configured for the I2C interface
 *
 */

#include <Arduino.h>    // Always include this. It's important.

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//      Definitions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define TFL_DEFAULT_ADDR     0x10   // default I2C address = 16
#define TFL_DEFAULT_FPS      0x64   // default frame-rate = 100fps

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

    // Get data
    bool getData( int16_t &dist, int16_t &flux, int16_t &temp, uint8_t addr);
    // Get data short version
    bool getData( int16_t &dist, uint8_t addr);

    // Read From or Write To an I2C register
    bool readReg( uint8_t nmbr, uint8_t addr);
    bool writeReg( uint8_t nmbr, uint8_t addr, uint8_t data);

    // Explicit Device Commands
    bool Get_Firmware_Version( uint8_t ver[], uint8_t adr);
    bool Get_Frame_Rate( uint16_t &frm, uint8_t adr);
    bool Get_Prod_Code( uint8_t cod[], uint8_t adr);
    bool Get_Time( uint16_t &tim, uint8_t adr);

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
    void printDataArray();

  private:

    uint8_t tfStatus;        // system error status: READY = 0
    uint8_t dataArray[ 6];
    uint8_t regReply;
    void printStatus();
};

