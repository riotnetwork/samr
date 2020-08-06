
R34 stamp is designed around Microchip’s ATSAMR34J18 SiP,  a 32-bit Arm Cortex-M0+ MCU (256Kb of Flash, 40Kb of RAM, 8kb of RWW flash) with an internal SX1276 LoRa transceiver. The SiP has a maximum power output of 20.00dBm, a frequency range of 862 to 1020MHz, and a 790nA sleep current.

This core is an arduino port for the Microchip ATSAMR34 Series microcontrollers - ATSAML21 with integrated SX1276 LoRa Tranceivers

## RIOT R34 Stamp setup

Board definition for Arduino IDE to install packages :

JSON URL : https://raw.githubusercontent.com/riotnetwork/samr-json/master/package_riot_index.json

* Open your Arduino IDE (V1.6.4 or newer.)
* In Arduino, Click File and then Preferences
* Add the JSON URL to the Additional Boards Manager text box. These are separated by a comma if you are using more than one added board link.
* Click Ok to close and save the preferences
* Then click Tools > Board > Board Manager
* You should see the Manager update on the first time opening after changing the preferences.
* Once updated you should find Riot boards listed in the Board Manager window.
* Find the install button next to this Board class and install.

* Restart Arduino IDE and look under Tools > Boards and you should see the addition of the Riot boards in your board list.

## RIOT R34 Stamp
This core is an arduino port for the Microchip ATSAMR34 and R35 Series microcontrollers - ATSAML21 with integrated LoRa Tranceivers

![R34 pinout](https://github.com/riotnetwork/samr-json/blob/master/Pinouts/PNG/R34-Stamp.png)

## License and credits

This core has been developed by Arduino LLC in collaboration with Atmel.

```
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
```
