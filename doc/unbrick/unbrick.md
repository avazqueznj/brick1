

>>> FOR WIFI RUN SKETCH STM32H747 --> wifi and cert update



Connect your GIGA R1 WiFi to your computer using a USB cable.



* Find the two push buttons labeled BOOT0 and RST on the board:
* Press and hold BOOT0 button
* Keep the BOOT0 button pressed down, and press the RST button once.
* Release both buttons. GIGA R1 WiFi will briefly disconnect and reconnect to your computer.



then in cube



Open STM32CubeProgrammer and click on Open file.

Select the bootloader from one of the following locations:

C:\\Users\\{username}\\AppData\\Local\\Arduino15\\packages/arduino/hardware/mbed\_giga/4.0.6/bootloaders/GIGA/bootloader.elf

If you get a “Warning: File corrupted. Two or more segments defines the same memory zone” message, ignore it.



* Set the programmer selection (default: ST-LINK) to USB
* For the Port selection, select your board. To refresh the list, click the update button.
* Click on Connect button. The connection status should change from “Not connected” to “Connected”.



* Click full erase, left bottom



* Click on the Download button. If you get another warning message, close it by selecting OK.
* Click on the left part of the button where it says “Download” (not the arrow, which opens a context menu)
* The message “File download complete” will appear as a pop-up notification when the download is complete.
* Disconnect and reconnect GIGA R1 WiFi to your computer.





!!>>>

* 5\. with ARDU IDE!! download sketch it will finish downloading what is needed.



\- test

