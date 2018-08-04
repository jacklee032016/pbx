===============
600 version PBX
===============

Big-Endian ARM System(Intel ixp422-xscale chipsets)
PTSN from independent C51 board with call audio switch matrix chips through PCM 2M bus;

Contains:

---------
Modules
---------

^^^^^
ICE
^^^^^

* ICE program by ARM Development Suites and ARM Multi-ICE;
* First piece of code for bare metal;
* Programming bootloader, kernel and file system into flash;


^^^^^^^
Drivers
^^^^^^^

* Driver for Zarlink echo cancel chip;
* Driver for SLIC device (FXS/FXO);
* Driver for 2M PCM bus device;
* Driver for u/a law generating and detecting;
* DTMF algorithms;
* Miscelleous devices:

  - WatchDog;
  - 93LC66 I2C EEPROM;
  - RTC;
  - Button;
  - Beep;


^^^^^^
u-boot
^^^^^^

* Add hardware related code for our board;


^^^^^^
Kernel
^^^^^^

* Add hardware related code and some modifications for our PBX requirements;

^^^^^^^^^^^^^^^
Shared Library
^^^^^^^^^^^^^^^

* Shared library for whole systems and make all components can be easily controlled:
  
  - PBX core functions;
  - Web Admin;

* Some utilities used in system;


^^^^^^^^^^^^^
VoIP Library 
^^^^^^^^^^^^^

* C++ VoIP library, supports:

  - SIP;
  - SDP;
  - RTP;
  
^^^^^^
PBX
^^^^^  

* Calls, States, Operations, Events, and FSM(Finite State Machine);
* Support VoIP;
* Support PSTN:

   - Legacy FXS/FXO device;
   - PBX connected through PCM bus;
   - Call Rule;
   - Signal control,
   - etc.

^^^^^^^^^
WebAdmin
^^^^^^^^^

* Web administration for whole;

  - CGI library;
  - Wizards;
  - Network Management (VPN, route, NAT, wifi, mesh, etc.);
  - System Management: disk, backup/restore, system upgrade (HTTP/TFTP);
  - DDNS;
  - Services Management;
  - Hardware Monitoring and Controlling;
  - PBX Management: 
  
    = Extension Setup, Trunck Setup, Night Service,
    = Alert, CDR, Recording, Black List;
    = Call Forward, Ext Transfer,
    = SIP Server configuration, Party Server,
    = etc.
    
     
^^^^^^^^
STC PBX
^^^^^^^^

* PBX based on STC C51 board;
* With audio switch matrix;
* Connect to ARM board through PCM 2M bus;


^^^^^^^^
STD LCD
^^^^^^^^

* LCD displayer for PBX based on STC C51 board;
* Control LCD to display calling and configuration data;
* Connect to ARM board through RS232 port;
  
