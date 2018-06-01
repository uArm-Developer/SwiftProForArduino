## Swift Pro Firmware V4.1.0 (Base on Grbl v0.9j https://github.com/grbl/grbl)
***

# Update Summary for v4.1.0
* Transplant Grbl framework
* Optimize stepper motor performance
* Fix end servo jitter

# Flashing Firmware to Swift Pro
To flash  hex to Swift Pro:
* Download and extract XLoader.
* Open XLoader and select your Arduino's COM port from the drop down menu on the lower left.
* Select the appropriate device from the dropdown list titled "Device".
* Check that Xloader set the correct baud rate for the device: 57600 for Duemilanove/Nano (ATmega 328) or 115200 for Uno (ATmega 328).
* Now use the browse button on the top right of the form to browse to your grbl hex file.
* Once your grbl hex file is selected, click "Upload"
After clicking upload, you'll see the RX/TX lights going wild on your Arduino. The upload process generally takes about 10 seconds to finish. Once completed, a message will appear in the bottom left corner of XLoader telling you how many bytes were uploaded. If there was an error, it would show instead of the total bytes uploaded. Steps should be similar and may be done through the command prompt.
