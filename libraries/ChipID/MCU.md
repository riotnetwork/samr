
# Atmel SAMR ARM Microcontroller

Atmel SAMR34 / 35 ( and SAML series ) is used in RIOT Stamp

Each device has a unique 128-bit serial number which is a concatenation of four 32-bit words contained at the following addresses: 

* Word 0: 0x0080A00C 
* Word 1: 0x0080A040
* Word 2: 0x0080A044
* Word 3: 0x0080A048

The uniqueness of the serial number is guaranteed only when using all 128 bits. [SAML21 Datasheet Section 11.6]