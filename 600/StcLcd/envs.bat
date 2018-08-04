@echo off

REM build all program for LCD based on STC C51
REM Li Zhijie,2007.05.21

set C51_HOME=c:\Keil\C51\
set MY_HOME=d:\stc\as600_StcLcd\

set PATH=%C51_HOME%BIN;%PATH%
set C51INC=%C51_HOME%INC;%MY_HOME%include
set C51LIB=%C51_HOME%LIB
set tmp=D:\stc
