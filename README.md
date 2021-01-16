# lume1-fireflies-anduril
Anduril 2 firmware (Atmel Studio 7) for Lume1-Fireflies Driver
For Hardware Revision B (lastest driver PCB has date 11/20)

- Latest revision: 29 Dec 2020
- HWID model number: 0443

The Lume1-Fireflies driver is an advanced electronic-switch, single cell, 6A fully regulated dual-phase synchronus buck LED driver, with turbo FET capabilities, and integrated on-board intelligent 2A USB-C battery charging. It adopts the open-sourced Anduril2 firmware by Toykeeper, with a few modifications to suit the requests of FireFlies/FireflyLite. 

The driver uses the Atmel ATTINY1634 MCU, and features a calibrated external temperature sensor, AUX LED support, button LED support for battery charging indication, as well as community standard 4+2 pogo-pads for programming.  The driver can be re-flashed with the firmware in this repository and customized to the users' desires such as changing default ramping styles, thermal handling, as well as modes selection including a new Beacon-tower mode. 

This driver was designed to the requested specifications of FireFlies/FireflyLite, and intended for use in a few flashlights to be released at the end of 2020. They include the Fireflies E12R flashlight and NOV-MU flashlight. It may also be used for other flashlights which unfortunately have not been disclosed to me.

For more information: www.loneceans.com/labs/

Anduril2 by TK: http://toykeeper.net/torches/fsm/anduril2/anduril-manual.txt
Anduril2 community discussion: https://budgetlightforum.com/node/73470
