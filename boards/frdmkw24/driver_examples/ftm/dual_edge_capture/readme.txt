Overview
========
The FPM project is a demonstration program of the SDK FPM driver's dual-edge capture feature.
This feature is available only on certain SoC's.
The example sets up a FPM channel-pair for dual-edge capture. Once the input signal is received,
this example will print the capture values and period of the input signal on the terminal window.

Toolchain supported
===================
- IAR embedded Workbench 7.60.1
- Keil MDK 5.18a
- GCC ARM Embedded 2015-4.9-q3
- Kinetis Development Studio IDE 3.2.0
- Atollic TrueSTUDIO 5.4.2

Hardware requirements
=====================
- Mini/micro USB cable
- FRDM-KW24 board
- Personal Computer

Board settings
==============
The connection should be set as following:
- Connect input signal to J41-1

Prepare the Demo
================
1.  Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
================
When the example runs successfully, the following message is displayed in the terminal:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FTM dual-edge capture example
Once the input signal is received the input capture values are printed
The input signal's pulse width is calculated from the capture values & printed
Capture value C(n)V=3
Capture value C(n+1)V=5a8
Input signals pulse width=24 us
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Customization options
=====================


