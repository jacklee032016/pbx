REM @echo off

REM build all program for LCD based on STC C51
REM Li Zhijie,2007.05.21

echo build All C modules
c51 src\main.c
c51 src\lcdIsr.c
c51 src\lcdTimerTask.c

c51 src\hw\lcdInit.c
c51 src\hw\lcdHwCtrl.c

c51 src\draw\ascii5x8.c
c51 src\draw\ascii12x16.c
c51 src\draw\arrow.c
c51 src\draw\strings.c
c51 src\draw\lcdDraw.c

c51 src\state\lcdStateMachine.c
c51 src\state\lcdStates.c

c51 src\uart\lcdUartRx.c
c51 src\uart\lcdUartTx.c
