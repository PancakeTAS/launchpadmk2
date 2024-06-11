# launchpadmk2
Single header library for the Novation Launchpad MK2

This c library is a single header file that allows you to fully interact with the Novation Launchpad MK2. It follows the specifications of the [Launchpad MK2 Programmer's Reference Manual](https://fael-downloads-prod.focusrite.com/customer/prod/s3fs-public/downloads/Launchpad%20MK2%20Programmers%20Reference%20Manual%20v1.03.pdf) closely.

## Features
With this library you can:
- Read button presses
- Control LEDs through Note On and Control Change messages
- Pulse and flash LEDs
- Set LEDs through RGB values
- Change between various modes of the Launchpad MK2
- Setup virtual sliders
- Modify the bpm of the Launchpad MK2
- Scroll text on the Launchpad MK2
- Obtain device information through device inquiry
- Enter the bootloader

## Usage
To use this library, simply include the header file in your project and specify `LAUNCHPAD_IMPL` in one of your source files.

If you wish to enable error and trace messages, define `LAUNCHPAD_LOG_ERROR` and `LAUNCHPAD_LOG_TRACE` respectively.

You can find a usage example in `src/helloworld.c`.
