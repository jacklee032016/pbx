REM @echo off
REM build all program for Analog PBX based on STC C51, Li Zhijie,2007.07.29

c51 new\main.c

c51 new\utils\stcUtils.c
c51 new\utils\pbxUtils.c

c51 new\hw\hwCtrl.c
c51 new\hw\eepromFm24c08.c
c51 new\hw\realTimeHt1381.c
c51 new\hw\selfTest.c

c51 new\cfg\uart\cfgUartRx.c
c51 new\cfg\uart\cfgUartTx.c
c51 new\cfg\program\cfgProgramCmds.c
c51 new\cfg\program\cfgProgramHandlers.c
c51 new\cfg\cfgNvramSave.c
c51 new\cfg\cfgDefaults.c
c51 new\cfg\cfgPbx.c

c51 new\hal\halChannels.c
c51 new\hal\halPstn.c

c51 new\event\timerTasks.c
c51 new\event\pbxIsrs.c
c51 new\event\devIp.c
c51 new\event\devPstn.c
c51 new\event\devExt.c

c51 new\state\stateMachine.c
c51 new\state\stateBusy.c
c51 new\state\stateDialing.c
c51 new\state\stateIdle.c
c51 new\state\stateIncall.c
c51 new\state\statePrograming.c
c51 new\state\stateRinging.c
c51 new\state\stateTrying.c
c51 new\state\stateWait.c
