@echo off

REM build all program for Analog PBX based on STC C51
REM Li Zhijie,2007.04.04

set C51_HOME=c:\Keil\C51\
set MY_HOME=d:\stc\as600_StcPbx\

set PATH=%C51_HOME%BIN;%PATH%
set C51INC=%C51_HOME%INC;%MY_HOME%new\include
set C51LIB=%C51_HOME%LIB
set tmp=D:\stc

