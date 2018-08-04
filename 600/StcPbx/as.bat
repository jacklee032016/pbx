@echo off

REM build all program for Analog PBX based on STC C51
REM Li Zhijie,2007.04.04

echo build All Asemble modules

REM 
a51 new/hw/asm/asm.asm
REM 
a51 new/hw/asm/startup.asm

REM bl51  TO a.abs  RAMSIZE(256)
REM bl51  @link.lst
REM oh51 a.abs HEXFILE (pbx.hex)
