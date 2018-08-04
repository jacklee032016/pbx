
; ------------------------------ 宏晶科技   2005/01/17 --------------------------
; --------------  Tel: 0755-82948409        Fax:0755-82944243 -------------------
; --------------  Mobile:13922805190  (姚永平) ---------------------------------- 
;			13828749381
; ------   Website:www.mcu-memory.com    Email: support@dsp-memory.com   --------
; 未经深圳市宏晶科技书面同意, 不得将本程序泄露、公开给第三方。
; 不得将本程序(或修改后的程序)使用在非深圳市宏晶科技销售的产品上。
; 客户产品上使用本程序时，客户产品的源程序中必须注明使用了深圳市宏晶科技的程序，
; 并保留如下内容:
;********************************************************************************
; ------------------------------ 宏晶科技   2005/01/09 --------------------------
; --------------  Tel: 0755-82948409        Fax:0755-82944243 -------------------
; --------------  Mobile:13922805190  (姚永平) ---------------------------------- 
; ------   Website:www.mcu-memory.com    Email: support@dsp-memory.com   --------
;********************************************************************************
;       STC ISP DEMO 程序为以下型号的 MCU 编制，通讯协议在程序的后面。
;STC89LE51RC, STC89LE52RC, STC89LE53RC, STC89LE54RD+, STC89LE58RD+, STC89LE516RD+
;STC89C51RC, STC89C52RC, STC89C53RC, STC89C54RD+, STC89C58RD+, STC89C516RD+

; 晶振频率:11.0592MHz, 波特率:38400, 可以修改程序换用其它的晶振频率和波特率。

;     STC-ISP (2.8 Beta3 版本以上)下载软件提供了STC ISP DEMO 程序的上位机
; 功能，该软件可到 MCU-Memory.COM 网站下载。
;--------------------------------------------------------------------------------
;定义特殊功能寄存器

;与 RS232口、TIMER2 有关的特殊功能寄存器
T2CON                   EQU 0C8H
TR2                     EQU  02H      ;TR2 是 T2CON 特殊功能寄存器的第 2 位

RCAP2L                  EQU 0CAH
RCAP2H                  EQU 0CBH
TH2                     EQU 0CDH
TL2                     EQU 0CCH

;和 IAP 功能有关的特殊功能寄存器
ISP_DATA                EQU 0E2H      ;ISP/IAP Flash Data Register
ISP_ADDRH               EQU 0E3H      ;ISP/IAP Flash Address High
ISP_ADDRL               EQU 0E4H      ;ISP/IAP Flash Address Low
ISP_CMD                 EQU 0E5H      ;ISP/IAP Flash Command Register
ISP_TRIG                EQU 0E6H      ;ISP/IAP Flash Command Trigger
ISP_CONTR               EQU 0E7H      ;ISP/IAP Control Register

WDT_CONTR               EQU 0E1H      ;看门狗控制寄存器

;-----------------------------------------------------------
;定义常数(全部大写)

FIRMWARE_VERSION        EQU  13H      ;版本号1.3, 中间隐含一个小数点.



;以下参数需要用户设置---------------------------------------------------------------
                                      ;SCON SM0/FE SM1 SM2 REN TB8 RB8 Tl  Rl 
                                      ;Bit:   7     6   5   4   3   2   1  0 
;无偶校验要将下行注释释放, 使用偶校验时要将下行注释
SCON_CONFIGURE          EQU  50H     ; 0101,0000 8位可变波特率, 无奇偶校验
;使用偶校验要将下行注释释放, 不使用偶校验时要将下行注释
;SCON_CONFIGURE          EQU 0D0H     ; 1101,0000 9位可变波特率, 有奇偶校验


;设置波特率重载数(计算重载数见文件后面的说明)
RELOAD_COUNT_HIGH       EQU  0FFH

;使用以下参数必须将 RELOAD_COUNT_HIGH 设置为 0FFH
RELOAD_COUNT_LOW        EQU  0FAH    ;Fosc = 22.1184MHz, Baud = 115200
;RELOAD_COUNT_LOW        EQU  0B8H    ;Fosc = 22.1184MHz, Baud = 9600

;RELOAD_COUNT_LOW        EQU  0EEH    ;Fosc = 22.1184MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F0H    ;Fosc = 20.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F6H    ;Fosc = 12.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F7H    ;Fosc = 11.059MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F8H    ;Fosc = 10.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0FBH    ;Fosc = 6.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0FCH    ;Fosc = 5.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  070H    ;Fosc = 11.059MHz, Baud = 2400


;设置检测串口是否有信号的超时时间。波特率为2400时，超时时间要延长。
;应用程序对延时不敏感时可适当延长。
;请注意: 定时常数 ENABLE_ISP_IAP, T0_WAIT_MAX, 可能需要修改。
;TEST_P30_TIME           EQU   1       ;时钟频率 <= 4Hz
;TEST_P30_TIME           EQU   2       ;时钟频率 <= 6MHz
;TEST_P30_TIME           EQU   3       ;时钟频率 <= 12MHz
;TEST_P30_TIME           EQU   4       ;时钟频率 <= 18MHz
TEST_P30_TIME           EQU   5       ;时钟频率 <= 24MHz
;TEST_P30_TIME           EQU   7       ;时钟频率 <= 30MHz
;TEST_P30_TIME           EQU   8       ;时钟频率 <= 36MHz
;TEST_P30_TIME           EQU   9       ;时钟频率 <= 42MHz

;打开 IAP 功能、MCU 等待 ISP/IAP 操作时间
ENABLE_ISP_IAP          EQU  80H      ;晶体频率<= 40MHz, 1000,0000
;ENABLE_ISP_IAP          EQU  81H      ;晶体频率<= 20MHz, 1000,0001
;ENABLE_ISP_IAP          EQU  82H      ;晶体频率<= 10MHz, 1000,0010
;ENABLE_ISP_IAP          EQU  83H      ;晶体频率<= 5MHz, 1000,0011

;计算T0_WAIT_MAX: T0_WAIT_MAX 决定了最大超时时间，
;    超时时间 =  65535 * 12 * T0_WAIT_MAX / 晶体频率
;    T0_WAIT_MAX = 超时时间 * 晶体频率 / 65535 / 12
;T0_WAIT_MAX             EQU  1CH ;超时时间=2S, 晶体频率=11.0592MHz
T0_WAIT_MAX             EQU  38H ;超时时间=2S, 晶体频率=22.1184MHz
;T0_WAIT_MAX            EQU 0F0H ;调试时使用, 超时时间=8.5S, 晶体频率=22.1184MHz
;--------------------------------------------
;定义 MCU 型号的代号, 以下只可选择一种, 其余的注释掉
;MCU_TYPE_HIGH          EQU  00H      ;STC 89LE51RC, 89C51RC 的代号 
;MCU_TYPE_LOW           EQU  51H
;SECTOR_NUMBER          EQU  10H      ;ISP程序为1K时, 最大扇区数16, 10H=十进制 16

;MCU_TYPE_HIGH          EQU  00H      ;STC 89LE52RC, 89C52RC 的代号 
;MCU_TYPE_LOW           EQU  52H
;SECTOR_NUMBER          EQU  18H      ;ISP程序为1K时, 最大扇区数24, 18H=十进制 24

;MCU_TYPE_HIGH          EQU  00H      ;STC 89C/LE53RC 代号 
;MCU_TYPE_LOW           EQU  53H
;SECTOR_NUMBER          EQU  1CH      ;ISP程序为1K时, 最大扇区数28, 1CH=十进制 28

;MCU_TYPE_HIGH          EQU  00H      ;STC 89C/LE54RD+代号 
;MCU_TYPE_LOW           EQU  54H
;SECTOR_NUMBER          EQU  60H      ;ISP程序为1K时, 最大扇区数96, 60H=十进制 96

;MCU_TYPE_HIGH           EQU  00H      ;STC 89C/LE58RD+代号 
;MCU_TYPE_LOW            EQU  58H
;SECTOR_NUMBER           EQU  7AH      ;ISP程序为1K时, 最大扇区数122, 7AH=十进制 122

MCU_TYPE_HIGH          EQU  05H      ;STC 89C/LE516RD+代号 
MCU_TYPE_LOW           EQU  16H
SECTOR_NUMBER          EQU  7CH      ;ISP程序为1K时, 最大扇区数124, 7CH=十进制 124

;以上参数需要用户设置---------------------------------------------------------------

YES                     EQU  01H
NO                      EQU  00H

;命令字
COMM_ACK                EQU 0B0H      ;上位机与下位机双方的应答信号
COMM_RUN_USER_CODE      EQU 0B1H      ;结束 ISP 程序, 软复位切换到应用程序.
COMM_ERASE_USER_CODE    EQU 0B2H      ;擦除应用程序 flash
COMM_WRITE_FLASH        EQU 0B3H      ;写数据到用户应用程序区中(以下简称 AP 区)
COMM_ERASE_SECTOR_WRITE EQU 0B4H      ;写数据到 AP 区, 写前擦除扇区
COMM_READ_VERSION       EQU 0B5H      ;读版本号

START_CHAR_1            EQU  5AH      ;通讯时的第一个前导字节
START_CHAR_2            EQU 0A5H      ;通讯时的第二个前导字节

FILL_CHECK_RAM_CHAR     EQU  69H      ;填充 RAM 区抗干扰监控单元的字符

;-----------------------------------------------------------
;定义变量(全部小写)
check_RAM_20H           EQU  20H     ;抗干扰监控 RAM
;-------------------------------
time_flag               EQU  21H
time_over_bit           EQU  0       ;定时溢出标志位
frist_byte_over_bit     EQU  1       ;前导字节出错计数器溢出标志位

;-------------------------------
my_flag                 EQU  23H
old_val_bit             EQU  0
check_sumOK_bit         EQU  1       ;已正确收到PC机命令的标志, 1=已正确收到
ap_erase_bit		EQU  2
;-------------------------------
check_RAM_24H           EQU  24H     ;抗干扰监控 RAM

once_AP_erase           EQU  25H     ;复位后置为64H(0110,0100), 删除AP后置9BH(1001,1101)
start_char_error_count  EQU  26H     ;通讯时前导字节出错计数器

check_RAM_28H           EQU  28H     ;抗干扰监控 RAM

t0_over_count           EQU  29H     ;T0溢出计数器

check_RAM_2CH           EQU  2CH     ;抗干扰监控 RAM

receive_byte_count      EQU  2DH     ;串口当前已接收字节数

check_RAM_30H           EQU  30H     ;抗干扰监控 RAM

sending_sum_high        EQU  31H     ;存储发送时的校验和高字节
sending_sum_low         EQU  32H     ;存储发送时的校验和低字节

check_RAM_34H           EQU  34H     ;抗干扰监控 RAM

receive_sum_high        EQU  35H     ;接收中断校验和高字节
receive_sum_low         EQU  36H     ;接收中断校验和低字节

check_RAM_38H           EQU  38H     ;抗干扰监控 RAM
check_RAM_3CH           EQU  3CH     ;抗干扰监控 RAM

count_ACK               EQU  3EH     ;问讯帧计数器

;串口收发缓冲区的大小为 142
rs232buffer0            EQU  40H     ;串口收发缓冲区首地址
rs232buffer1            EQU  41H
rs232buffer2            EQU  42H
rs232buffer3            EQU  43H
rs232buffer4            EQU  44H
rs232buffer5            EQU  45H
rs232buffer6            EQU  46H
rs232buffer7            EQU  47H
rs232buffer8            EQU  48H
;rs232buffer9           EQU  49H
rs232buffer10           EQU  4AH
rs232buffer11           EQU  4BH
;------------------------------------------------
    ORG   0000H
    AJMP  MAIN
;------------------------------------------------
    ORG   0003H
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   000BH
    AJMP  Timer0
;------------------------------------------------
    ORG   0013H
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   001BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   0023H
    AJMP  UART
;------------------------------------------------
    ORG   002BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   0033H
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   003BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
    ORG   0050H
MAIN:
    MOV   SP, #0E0H

    ACALL Initial_MCU           ;系统初始化
    ACALL Initial_RS232         ;串口初始化 
    CLR	  P1.3

    ACALL Wait_P30_Change       ;检测串口电平是否有变化,无变化表示串口无命令
    JNC   Goto_AP               ;串口无命令, 软复位切换到应用程序

;    ACALL Send_Byte             ;发送一个字节

    ACALL Set_T0_Overtime_MAX   ;设置超时时间.

    MOV   count_ACK, #1         ;清问讯帧计数器
Main_1: 
    SETB  P1.3                        ;等待PC机命令
    JB    my_flag.check_sumOK_bit, Main_2 ;判收到PC机命令否,1 收到PC机命令
    ORL   WDT_CONTR, #17H       ;喂狗
    CLR	  P1.0
    SETB  P1.0
    JNB   time_flag.time_over_bit, Main_1
Goto_AP:
    AJMP  Run_User_Program      ;超时, 软复位切换到应用程序
Main_2:                         ;收到PC机命令
    CLR   ES                    ;禁止串口中断
    CLR	  P1.7
    MOV   A, #1                 ;连续收到3次问讯帧?
    CLR   C
    SUBB  A, count_ACK
    JC    Main_3                ;是的, 握手成功, 转 Main_3
    MOV   A, rs232buffer4       ;读 PC 命令
    CJNE  A, #COMM_ACK,  Goto_AP;不是问讯帧, 软复位切换到应用程序
    INC   count_ACK             ;问讯帧计数器 + 1
    SJMP  Main_4                ;回发应答信号
Main_3:
    MOV   A, rs232buffer4       ;读 PC 命令
    CJNE  A, #COMM_ACK, Main_5  ;收到问讯帧
    AJMP  Main_End
    ;CLR   P1.1
Main_4:
    ACALL Send_Command_ACK      ;回发应答信号
    SETB  P1.7
    AJMP  Main_End

Main_5:
    CJNE  A, #COMM_RUN_USER_CODE, Main_6  ;结束 ISP 程序, 软复位切换到应用程序
    ACALL Send_Command_ACK      ;回发应答信号
    AJMP  Run_User_Program      ;软复位切换到应用程序

Main_6:
    CJNE  A, #COMM_ERASE_USER_CODE, Main_7  ;擦除 AP 区?
    ACALL AP_Erase
    SJMP  Main_4                ;回发应答信号
    
Main_7:
    CJNE  A, #COMM_WRITE_FLASH, Main_8  ;写数据到flash book0 程序区
    CLR	  my_flag.ap_erase_bit
    ;CLR   C                     
    ACALL Write_Flash_Sector    ;C=0: 不擦除
    AJMP  Main_End
Main_8:
    CJNE  A, #COMM_ERASE_SECTOR_WRITE, Main_9 ;擦除扇区再写数据
    SETB  my_flag.ap_erase_bit
    ;SETB  C                     
    ACALL Write_Flash_Sector      ;C=1: 擦除扇区再写数据
    AJMP  Main_End

Main_9:
    CJNE  A, #COMM_READ_VERSION, Main_End

    MOV   Rs232buffer3, #0CH       ;发送 版本号, 和 MCU 代号
    MOV   Rs232buffer4, #COMM_READ_VERSION
    MOV   Rs232buffer5, #FIRMWARE_VERSION    ;版本号, 中间隐含一个小数点.
    MOV   rs232buffer6, #MCU_TYPE_HIGH ;MCU 代号高字节
    MOV   rs232buffer7, #MCU_TYPE_LOW ;MCU 代号低字节
    MOV   Rs232buffer8, #0FFH      ;该字节保留未用
    ACALL Send_RS232_Buffer        ;发送缓冲区数据

Main_End:
    ANL   ISP_CONTR, #7FH          ;关闭 IAP 功能
    ACALL Set_T0_Overtime_MAX
    CLR   TR0                      ;关闭 T0
    CLR   my_flag.check_sumOK_bit
    MOV   receive_byte_count, #0   ;清零串口接收数据指针

    MOV   start_char_error_count, #0FFH  ;通讯时前导字节出错计数器置初值
    SETB  ES
    AJMP  Main_1
;------------------------------------------------
IAP_Enable:                        ;打开 IAP 功能
    MOV   R0, #check_RAM_20H       ;检查 check_RAM 单元变化否，若有变化，
                                   ;预示着 RAM 区内容因干扰等原因被破坏。
    MOV   R2, #8
Check_RAM1:
    CJNE  @R0, #FILL_CHECK_RAM_CHAR, Goto_User_Program ;软复位切换到应用程序
    MOV   A, #4
    ADD   A, R0
    MOV   R0, A
    DJNZ  R2, Check_RAM1
;----------------
IAP_Enable1:
    MOV   ISP_CONTR, #ENABLE_ISP_IAP ;打开 IAP 功能
    RET
Goto_User_Program:
    AJMP  Run_User_Program
;------------------------------------------------
;入口:DPTR = 字节地址, A=字节
;字节编程

Byte_Program:
    MOV   ISP_CMD, #02H         ;Select Byte Program Mode
    ACALL Set_Address
    MOV   ISP_DATA, A           ;Fill the data to be programmed in ISP_DATA
Trigger_ISP:
    CLR  EA
    MOV  ISP_TRIG, #46H         ;Trigger ISP processing
    MOV  ISP_TRIG, #0B9H
    NOP
    SETB EA
    RET 
;------------------------------------------------
Set_Address:
    MOV  ISP_ADDRH, DPH         ;Fill page address in ISP_ADDRH & ISP_ADDRL
    MOV  ISP_ADDRL, DPL
    RET
;------------------------------------------------
;扇区擦除, 入口:DPTR = 扇区地址
Sector_Erase:
    MOV  ISP_CMD, #03           ;Select Page Erase Mode
    ACALL Set_Address
    ACALL Trigger_ISP           ;Trigger ISP processing
    RET
;------------------------------------------------
;擦除 AP 区
AP_Erase:    
    NOP
    NOP
    NOP

    ACALL IAP_Enable            ;打开 IAP 功能

    ;检查 once_AP_erase 是否为 #64H， 若不是 #64H 就软复位切换到应用程序。
    MOV   A, once_AP_erase      ;复位后置为64H, 删除 AP 后置 9BH
    CJNE  A, #64H, AP_erase_ERR ;软复位切换到应用程序
    MOV   R2, #SECTOR_NUMBER
    MOV   DPH, #0
AP_Erase_Loop:
    ACALL Sector_Erase
    INC   DPH
    INC   DPH
    DJNZ  R2, AP_Erase_Loop

    ANL   ISP_CONTR, #7FH       ;关闭 IAP 功能

    MOV   once_AP_erase, #9BH   ;复位后置为64H, 删除 AP 后置 9BH
    RET
AP_Erase_ERR:
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
;入口:DPTR = 字节地址
;出口:A = 读出字节
;读一字节 IAP command
Byte_Verify:
    MOV   ISP_CMD, #01           ;Select Read AP Mode
    ACALL Set_Address
    MOV   ISP_DATA, #00H         ;清数据寄存器
    ACALL Trigger_ISP
    ;Now in processing.(CPU will halt here before completing)
    MOV   A, ISP_DATA            ;Data will be in ISP_DATA
    RET
;------------------------------------------------
;C=0:写数据, 不擦除扇区
;C=1;擦除扇区再写数据
Write_Flash_Sector:
    ;写扇区, 检查 AP 区是否曾全部擦除。若未曾全部擦除就软复位切换到应用程序。
    MOV   A, once_AP_erase      ;复位后置为64H, 删除 AP 后置 9BH
    CJNE  A, #9BH, once_AP_erase_ERR ;软复位切换到应用程序
Write_Flash_Sector0:

    ACALL IAP_Enable            ;打开 IAP 功能

    MOV   DPH, rs232buffer7     ;设置地址
    MOV   DPL, rs232buffer8
    MOV	  C,   my_flag.ap_erase_bit
    JNC   Write_Flash_Sector_1
    ACALL Sector_Erase          ;擦除Flash扇区
Write_Flash_Sector_1:          ;写入Flash扇区, 越界,超长, 由上位机控制
    MOV   R2, rs232buffer10     ;数据长度
    MOV   R0, #rs232buffer11    ;数据首字节指针
    MOV	  R6, #0
    MOV	  R7, #0
Write_Flash_Sector_2:
    MOV   A, @R0
    ACALL Byte_Program
    INC   R0
    INC   DPTR
    DJNZ  R2, Write_Flash_Sector_2
;----------------------
Checksummat:                    ;计算数据块校验和
    MOV   DPH, rs232buffer7     ;设置读数据指针初值
    MOV   DPL, rs232buffer8
    MOV   R2, rs232buffer10     ;数据长度
    MOV   R6, #0                ;数据块校验和高字节
    MOV   R7, #0                ;数据块校验和低字节
Checksummat_1:
    ACALL Byte_Verify
    ADD   A, R7
    MOV   R7, A
    CLR   A
    ADDC  A, R6
    MOV   R6, A

    INC   DPTR
    DJNZ  R2, Checksummat_1

    ANL   ISP_CONTR, #7FH       ;关闭 IAP 功能

    MOV   Rs232buffer3, #0AH    ;发送校验和
    MOV   Rs232buffer5, R6
    MOV   Rs232buffer6, R7
    ACALL Send_RS232_Buffer     ;发送缓冲区数据
    RET
;----------------------
Once_AP_Erase_ERR:
    AJMP  Run_User_Program      ;软复位切换到应用程序
;------------------------------------------------
;软复位切换到应用程序
Run_User_Program:  
    MOV   A, #20H
    MOV   check_RAM_20H, A
    MOV   check_RAM_28H, A
    MOV   once_AP_erase, A
    MOV   ISP_CONTR, A          ;0010,0000 触发软件复位 boot from user_code
    SJMP  $                     ;正常情况下不会执行此指令
;------------------------------------------------
Initial_MCU:                    ;系统初始化
;---------------------   
    MOV   R3, #68H              ;12MHz 延时为 50mS
Clear_RAM_Delay0:
    MOV   R0, #08H              ;RAM 08H--7FH 清 0
    MOV   R2, #78H
    CLR   A
Clear_RAM_Delay1:
    MOV   @R0, A
    INC   R0
    DJNZ  R2, Clear_RAM_Delay1
    DJNZ  R3, Clear_RAM_Delay0
;---------------------   
    MOV   TMOD, #01H            ;0000,0001 设置T0为16位定时器
    ;MOV   IE, #80H
    MOV   IE, #82H              ;1000,0010 允许定时器0中断, 开中断
;---------------------   
    ;20H、24H、28H、2CH、30H、34H、38H、3CH 这 8 个单元复位后上初始化
    ;为 #FILL_CHECK_RAM_CHAR, 每次打开 IAP 功能时检查这 8 个单元变化否，
    ;若有变化，预示着 RAM 区内容因干扰等原因被破坏，就软复位切换到应用程序
    MOV   R0, #check_RAM_20H    ;初始化 check_RAM
    MOV   R2, #8
Initial_MCU1:
    MOV   @R0, #FILL_CHECK_RAM_CHAR
    MOV   A, #4
    ADD   A, R0
    MOV   R0, A
    DJNZ  R2, Initial_MCU1         
;---------------------   
    MOV   my_flag, #0           ;清标志
;---------------------   
    ;为加强保密性, 每次写 flashi 前检查 AP 区是否曾全部擦除。未曾
    ;全部擦除就软复位切换到应用程序。once_AP_erase 用作是否曾全部擦除标志。
    MOV   once_AP_erase, #64H   ;复位后置为64H, 删除 AP 区后置 9BH

    MOV   start_char_error_count, #0FFH  ;通讯时前导字节出错计数器置初值
;---------------------   
    RET
;------------------------------------------------
;检测串口电平是否有变化,无变化表示串口无命令
Wait_P30_Change:
    MOV   A, #TEST_P30_TIME     ; 设置超时时间.
    ACALL Set_T0_Overtime
     
    MOV   C, P3.0               ; store RXD/P3.0 value
    MOV   my_flag.old_val_bit, C
    Wait_P30_Change_1:
    JB    time_flag.time_over_bit, Wait_P30_Change_3  ;超时且无变化, 返回
    MOV   C, P3.0
    JNB   my_flag.old_val_bit, Wait_P30_Change_2
    CPL   C
Wait_P30_Change_2:
    JNC   Wait_P30_Change_1
    SETB  C                     ;有变化, 返回
    RET
Wait_P30_Change_3:
    CLR   C
    RET
;------------------------------------------------
Initial_RS232:               ;串口初始化  

    MOV   SCON, #SCON_CONFIGURE

    MOV   A, #RELOAD_COUNT_HIGH
    MOV   RCAP2H, A
    MOV   TH2, A
    MOV   A, #RELOAD_COUNT_LOW
    MOV   RCAP2L, A
    MOV   TL2, A

    MOV   T2CON, #34H
    SETB  ES				; enable UART IRQ
    CLR   check_sumOK_bit        ;清除"收到PC机命令"标志   
    MOV   receive_byte_count, #0 ;清零串口接收数据指针   
    SETB  EA
    RET
;------------------------------------------------
Set_T0_Overtime_MAX:            ;设置最大超时时间.
    MOV   A, #T0_WAIT_MAX
;------------------------------------------------
;入口: A = weit time count
Set_T0_Overtime:                ;设置超时时间.
    CLR   TR0                   ;停止定时器0工作
    MOV   t0_over_count, A
    CLR   A
    MOV   TL0, A
    MOV   TH0, A
    CLR   time_flag.time_over_bit ;清定时溢出标志
    SETB  TR0                   ;启动定时器0工作
    RET
;------------------------------------------------
Send_Command_ACK:               ;回发应答信号
    CLR   ES                    ;禁止串口中断
    MOV   Rs232buffer3, #08H
    MOV   Rs232buffer4, #COMM_ACK 
    ACALL Send_RS232_Buffer     ;发送缓冲区数据
    RET
;------------------------------------------------
Send_RS232_Buffer:              ;发送缓冲区数据
    CLR   ES                    ;禁止串口中断
    CLR   TR0                   ;停止定时器0工作

    MOV   Rs232buffer0, #START_CHAR_1
    MOV   Rs232buffer1, #START_CHAR_2
    MOV   Rs232buffer2, #00H

    MOV   R2, rs232buffer3      ;发送长度 -->R2
    DEC   R2
    DEC   R2
    DEC   R2
    CLR   A
    MOV   sending_sum_high, A   ;清0发送时的校验和
    MOV   sending_sum_low, A
    MOV   R0, #rs232buffer0
Send_RS232_Buffer_1:
    MOV   A, @R0
    ACALL Send_Byte             ;发送一个字节
    INC   R0
    DJNZ  R2, Send_RS232_Buffer_1

    MOV   R2, sending_sum_low   ;保存校验和
    MOV   A, sending_sum_high
    ACALL Send_Byte
    MOV   A, R2
    ACALL Send_Byte
    MOV   A, #16H               ;发送结束符
    ACALL Send_Byte

    CLR   my_flag.check_sumOK_bit ;清除"收到PC机命令"标志
    MOV   receive_byte_count, #0  ;清零串口接收数据指针
    SETB  REN                   ;允许串口接收数据
    SETB  ES                    ;允许串口中断
    RET
;------------------------------------------------
Send_Byte:                      ;发送一个字节
    MOV   R7, A

    MOV   A, #SCON_CONFIGURE
    CJNE  A, #0D0H, Send_Byte_a

    ;使用偶校验
    MOV   A, R7
   CLR   SCON.3                 ;SCON.TB8, 要发送的第9位, 偶校验
   JNB   PSW.0, Send_Byte_a
   SETB  SCON.3

Send_Byte_a:
    MOV   A, R7
    MOV   SBUF, A
    ADD   A, sending_sum_low    ;累加发送时的校验和
    MOV   sending_sum_low, A
    CLR   A
    ADDC  A, sending_sum_high   ;累加发送时的校验和
    MOV   sending_sum_high, A
    JNB   TI, $
    CLR   TI                    ;清零串口发送中断标志
    RET
;------------------------------------------------
Timer0:
    PUSH  PSW
    DJNZ  t0_over_count, Timer0_1
    SETB  time_flag.time_over_bit
    CLR   TR0
Timer0_1:
    POP   PSW
    RETI
;------------------------------------------------
UART:                           ;串口中断服务程序
    JBC   RI, UART_1
    RETI                        ;发送时使用的是查询方式，不使用中断
UART_1:                         ;接收中断标志 == 1,接收一个字节. 此时 RI 已被清0
    PUSH  PSW
    PUSH  ACC
    MOV   A, SBUF               ;保存接收到的字节 
    SETB  PSW.3                 ;选用第3组通用寄存器区
    MOV   R3, receive_sum_high  ;暂存校验和高位
    MOV   R4, receive_sum_low   ;暂存校验和低位
    MOV   A, receive_byte_count ;检查接收数是否超长
    SUBB  A, #143               ;通讯时数据区最大字节数为128，其它字节为14, 128+14=142
    JNC   Clear_Receive_Byte_Count ;接收数超长

    MOV   A, #rs232buffer0      ;计算串口接收数据指针, 保存接收到的字节
    ADD   A, receive_byte_count
    MOV   R0, A

    MOV   A, SBUF               ;保存接收到的字节               
    MOV   @R0, A
    MOV   R7, A


    MOV   A, #SCON_CONFIGURE             ;无偶校验时跳过以下一段
    CJNE  A, #0D0H, Receive_EVEN_2
    
;*******************************************
;    检测偶校验位
    MOV   A, R7
    JB   P, Receive_EVEN_1               ;P=1
    JNB  RB8, Receive_EVEN_2             ;P=0 AND RB8=0 偶校验正确
    SJMP Clear_Receive_Byte_Count        ;偶校验错, 清零串口接收数据指针   
Receive_EVEN_1:
    JNB  RB8, Clear_Receive_Byte_Count   ;P=1 AND RB8=0 偶校验错
Receive_EVEN_2:
;*******************************************
    
    CLR   A                              ;清下一个保存数据的单元
    INC   R0
    MOV   @R0, A
    DEC   R0
    MOV   A, R7
    ADD   A, receive_sum_low             ;计算累加和
    MOV   receive_sum_low, A
    CLR   A
    ADDC  A, receive_sum_high
    MOV   receive_sum_high, A
    
    INC   receive_byte_count             ;调整串口接收数据指针
    MOV   A, receive_byte_count
    MOV   R5, A
    CLR   C
    SUBB  A, #3
    JNC   Exceed_2_Byte                  ;已收到 2 个前导字符

    DJNZ  R5, Receive_2_Byte             ;收到的是第 1 个字符？
    CJNE  @R0, #START_CHAR_1, start_char_error  ;第 1 个字符不正确
    MOV   receive_sum_low, #START_CHAR_1 ;设置校验和初始值
    MOV   receive_sum_high, #00H
    SJMP  Exit_UART                      ;第 1 个字符正确, 返回
Receive_2_Byte:
    DJNZ  R5, Clear_Receive_Byte_Count   ;收到的是第 2 个字符？
    CJNE  @R0, #START_CHAR_2, start_char_error     ;第 2 个字符不正确
    SJMP  Exit_UART                      ;第 2 个字符正确, 返回

Exceed_2_Byte:                           ;收到的 2 个以上的字符
    MOV   A, receive_byte_count
    SUBB  A, #3
    JC    Exit_UART

    MOV   A, rs232buffer3                ;判是否接收到PC发来的校验和
    CLR   C
    SUBB  A, #3
    SUBB  A, receive_byte_count
    JNC   Exit_UART                      ;重置超时, 退出

    MOV   receive_sum_high, R3           ;此时接收的是累加和或结束符, 不再累加校验和
    MOV   receive_sum_low, R4            ;恢复原来的校验和

    MOV   A, rs232buffer3                ;接收数据已足够长?
    DEC   A
    CLR   C
    SUBB  A, receive_byte_count
    JNC   Exit_UART                      ;重置超时, 退出

    MOV   A, #(rs232buffer0-3)           ;接收完毕, 检测累加和是否正确, 结束符是否为16H
    ADD   A, receive_byte_count          ;读PC机发的校验和-->R1
    MOV   R1, A
    MOV   A, @R1
    CJNE  A, receive_sum_high, Clear_Receive_Byte_Count ;累加和不正确, 取消本次接收帧
    INC   R1
    MOV   A, @R1
    CJNE  A, receive_sum_low, Clear_Receive_Byte_Count  ;累加和不正确, 取消本次接收帧
    INC   R1
    MOV   A, @R1
    CJNE  A, #16H, Clear_Receive_Byte_Count             ;结束符不是16H, 取消本次接收帧
    SETB  my_flag.check_sumOK_bit        ;接收正确
    CLR   ES
Clear_Receive_Byte_Count:
    MOV   receive_byte_count, #0         ;清零串口接收数据指针
Exit_UART:
    ACALL Set_T0_Overtime_MAX
    POP   ACC
    POP   PSW
    RETI
start_char_error:
    MOV   receive_byte_count, #0
    DJNZ  start_char_error_count, Exit_UART ;前导字节出错计数器-1 = 0 ?
    SETB  time_flag.frist_byte_over_bit
    SJMP  Exit_UART

;**********************************************************
    END
;**********************************************************
