# TFLuna-I2C
### An Arduino library for the Benewake TFLuna LiDAR device using the I2C communication interface

The **TFLuna** in I2C communication mode is unique among the Benewake family of LiDAR products in at least two ways:
1) The communications mode (UART/U2C) is set by the voltage level of Pin #5 rather than a command; and
2) In I2C mode, the internal device registers of the device can be addressed directly.

This library is *not compatible* with any other Benwake LiDAR device in I2C mode. In serial (UART) mode, however, the **TFLuna** is largely compatible with the **TFMini-Plus** and is therefore able to use that library.

This library calls the Arduino standard I2C/Two-Wire Library.
<hr />

### Arduino Library Commands

The `getData( dist, flux, temp, addr)` function passes back three, signed, 16-bit measurement data values and sends an unsigned, 8-bit I2C address in the range of `0x08` to `0x77` (8 to 119).  A correct `addr` value must always be sent.  If the function completes without error, it returns 'True' and sets the public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' code to a library defined error code.

Measurement data values are passed-back in three, 16-bit, signed integer variables:
<br />&nbsp;&nbsp;&#9679;&nbsp; `dist` Distance to target in centimeters. Range: 0 - 1200
<br />&nbsp;&nbsp;&#9679;&nbsp; `flux` Strength or quality of return signal or error. Range: -1, 0 - 32767
<br />&nbsp;&nbsp;&#9679;&nbsp; `temp` Temperature in hundreths of degrees Celsius. Range: -25.00°C to 125.00°C

For convenience and simplicity, a `getData( dist, addr)` function is included. This function passes back distance data only.

Other commands are explicitly defined and are broadly separated into "Set" that modify a device parameter value amd and "Get" commands that examine a parmeter value.  All commands take the form of a function name followed by one or two parameters that are always the unsigned, 8-bit I2C address of the device and sometimes a register value before the address.  If the function completes without error, it returns 'True' and sets a public, one-byte 'status' code to zero.  Otherwise, it returns 'False' and sets the 'status' to a Library defined error code.
<hr>

In **I2C** mode, the TFMini-Plus functions as an I2C slave device.  The default address is `0x10` (16 decimal), but is user-programable by sending the `Set_I2C_Addr` command and a parameter in the range of `0x07` to `0x77` (7 to 119).  The new address requires a `Soft_Reset` command to take effect.  A `Hard_Reset` command (Restore Factory Settings) will reset the device to the default address of `0x10`.

Some commands that modify internal parameters are processed within 1 millisecond.  But some commands that require the MCU to communicate with other chips may take several milliseconds.  And some commands that erase the flash memory of the MCU, such as `Save_Settings` and `Hard_Reset`, may take several hundred milliseconds.

Frame-rate and most other parameter changes should be followed by a `Save_Settings` command or the values may be lost when power is removed.  With the TFLuna, commands are available to examine the value of various device paramters such as frame rate, trigger mode, power mode, threshold values, internal timer, error and production code.

<hr>

Also included in the repository are:
<br />&nbsp;&nbsp;&#9679;&nbsp; An Arduino sketch "TFLuna_example.ino" in the Example folder.
<br />&nbsp;&nbsp;&#9679;&nbsp; Recent copies of manufacturer's Datasheet and Product Manual in Documents.

All of the code for this Library is richly commented to assist with understanding and in problem solving.
