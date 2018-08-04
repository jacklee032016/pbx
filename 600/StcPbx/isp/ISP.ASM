
;********************************************************************************
;STC89LE51RC, STC89LE52RC, STC89LE53RC, STC89LE54RD+, STC89LE58RD+, STC89LE516RD+
;STC89C51RC, STC89C52RC, STC89C53RC, STC89C54RD+, STC89C58RD+, STC89C516RD+

; ����Ƶ��:11.0592MHz, ������:38400, �����޸ĳ����������ľ���Ƶ�ʺͲ����ʡ�

;--------------------------------------------------------------------------------
;�������⹦�ܼĴ���

;�� RS232�ڡ�TIMER2 �йص����⹦�ܼĴ���
T2CON                   EQU 0C8H
TR2                     EQU  02H      ;TR2 �� T2CON ���⹦�ܼĴ����ĵ� 2 λ

RCAP2L                  EQU 0CAH
RCAP2H                  EQU 0CBH
TH2                     EQU 0CDH
TL2                     EQU 0CCH

;�� IAP �����йص����⹦�ܼĴ���
ISP_DATA                EQU 0E2H      ;ISP/IAP Flash Data Register
ISP_ADDRH               EQU 0E3H      ;ISP/IAP Flash Address High
ISP_ADDRL               EQU 0E4H      ;ISP/IAP Flash Address Low
ISP_CMD                 EQU 0E5H      ;ISP/IAP Flash Command Register
ISP_TRIG                EQU 0E6H      ;ISP/IAP Flash Command Trigger
ISP_CONTR               EQU 0E7H      ;ISP/IAP Control Register

WDT_CONTR               EQU 0E1H      ;���Ź����ƼĴ���

;-----------------------------------------------------------
;���峣��(ȫ����д)

FIRMWARE_VERSION        EQU  13H      ;�汾��1.3, �м�����һ��С����.



;���²�����Ҫ�û�����---------------------------------------------------------------
                                      ;SCON SM0/FE SM1 SM2 REN TB8 RB8 Tl  Rl 
                                      ;Bit:   7     6   5   4   3   2   1  0 
;��żУ��Ҫ������ע���ͷ�, ʹ��żУ��ʱҪ������ע��
SCON_CONFIGURE          EQU  50H     ; 0101,0000 8λ�ɱ䲨����, ����żУ��
;ʹ��żУ��Ҫ������ע���ͷ�, ��ʹ��żУ��ʱҪ������ע��
;SCON_CONFIGURE          EQU 0D0H     ; 1101,0000 9λ�ɱ䲨����, ����żУ��


;���ò�����������(�������������ļ������˵��)
RELOAD_COUNT_HIGH       EQU  0FFH

;ʹ�����²������뽫 RELOAD_COUNT_HIGH ����Ϊ 0FFH
;RELOAD_COUNT_LOW        EQU  0FAH    ;Fosc = 22.1184MHz, Baud = 115200
RELOAD_COUNT_LOW        EQU  0B8H    ;Fosc = 22.1184MHz, Baud = 9600
;RELOAD_COUNT_LOW        EQU  0EEH    ;Fosc = 22.1184MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F0H    ;Fosc = 20.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F6H    ;Fosc = 12.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F7H    ;Fosc = 11.059MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0F8H    ;Fosc = 10.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0FBH    ;Fosc = 6.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  0FCH    ;Fosc = 5.000MHz, Baud = 38400
;RELOAD_COUNT_LOW        EQU  070H    ;Fosc = 11.059MHz, Baud = 2400


;���ü�⴮���Ƿ����źŵĳ�ʱʱ�䡣������Ϊ2400ʱ����ʱʱ��Ҫ�ӳ���
;Ӧ�ó������ʱ������ʱ���ʵ��ӳ���
;��ע��: ��ʱ���� ENABLE_ISP_IAP, T0_WAIT_MAX, ������Ҫ�޸ġ�
;TEST_P30_TIME           EQU   1       ;ʱ��Ƶ�� <= 4Hz
;TEST_P30_TIME           EQU   2       ;ʱ��Ƶ�� <= 6MHz
;TEST_P30_TIME           EQU   3       ;ʱ��Ƶ�� <= 12MHz
;TEST_P30_TIME           EQU   4       ;ʱ��Ƶ�� <= 18MHz
TEST_P30_TIME           EQU   5       ;ʱ��Ƶ�� <= 24MHz
;TEST_P30_TIME           EQU   7       ;ʱ��Ƶ�� <= 30MHz
;TEST_P30_TIME           EQU   8       ;ʱ��Ƶ�� <= 36MHz
;TEST_P30_TIME           EQU   9       ;ʱ��Ƶ�� <= 42MHz

;�� IAP ���ܡ�MCU �ȴ� ISP/IAP ����ʱ��
ENABLE_ISP_IAP          EQU  80H      ;����Ƶ��<= 40MHz, 1000,0000
;ENABLE_ISP_IAP          EQU  81H      ;����Ƶ��<= 20MHz, 1000,0001
;ENABLE_ISP_IAP          EQU  82H      ;����Ƶ��<= 10MHz, 1000,0010
;ENABLE_ISP_IAP          EQU  83H      ;����Ƶ��<= 5MHz, 1000,0011

;����T0_WAIT_MAX: T0_WAIT_MAX ���������ʱʱ�䣬
;    ��ʱʱ�� =  65535 * 12 * T0_WAIT_MAX / ����Ƶ��
;    T0_WAIT_MAX = ��ʱʱ�� * ����Ƶ�� / 65535 / 12
;T0_WAIT_MAX             EQU  1CH ;��ʱʱ��=2S, ����Ƶ��=11.0592MHz
T0_WAIT_MAX             EQU  38H ;��ʱʱ��=2S, ����Ƶ��=22.1184MHz
;T0_WAIT_MAX            EQU 0F0H ;����ʱʹ��, ��ʱʱ��=8.5S, ����Ƶ��=22.1184MHz
;--------------------------------------------
;���� MCU �ͺŵĴ���, ����ֻ��ѡ��һ��, �����ע�͵�
;MCU_TYPE_HIGH          EQU  00H      ;STC 89LE51RC, 89C51RC �Ĵ��� 
;MCU_TYPE_LOW           EQU  51H
;SECTOR_NUMBER          EQU  10H      ;ISP����Ϊ1Kʱ, ���������16, 10H=ʮ���� 16

;MCU_TYPE_HIGH          EQU  00H      ;STC 89LE52RC, 89C52RC �Ĵ��� 
;MCU_TYPE_LOW           EQU  52H
;SECTOR_NUMBER          EQU  18H      ;ISP����Ϊ1Kʱ, ���������24, 18H=ʮ���� 24

;MCU_TYPE_HIGH          EQU  00H      ;STC 89C/LE53RC ���� 
;MCU_TYPE_LOW           EQU  53H
;SECTOR_NUMBER          EQU  1CH      ;ISP����Ϊ1Kʱ, ���������28, 1CH=ʮ���� 28

;MCU_TYPE_HIGH          EQU  00H      ;STC 89C/LE54RD+���� 
;MCU_TYPE_LOW           EQU  54H
;SECTOR_NUMBER          EQU  60H      ;ISP����Ϊ1Kʱ, ���������96, 60H=ʮ���� 96

;MCU_TYPE_HIGH           EQU  00H      ;STC 89C/LE58RD+���� 
;MCU_TYPE_LOW            EQU  58H
;SECTOR_NUMBER           EQU  7AH      ;ISP����Ϊ1Kʱ, ���������122, 7AH=ʮ���� 122

MCU_TYPE_HIGH          EQU  05H      ;STC 89C/LE516RD+���� 
MCU_TYPE_LOW           EQU  16H
SECTOR_NUMBER          EQU  7CH      ;ISP����Ϊ1Kʱ, ���������124, 7CH=ʮ���� 124

;���ϲ�����Ҫ�û�����---------------------------------------------------------------

YES                     EQU  01H
NO                      EQU  00H


AP_EXIST_ADDRH		EQU  0EAH      ;"AP���ڵı�־"�ĸߵ�ַ
AP_EXIST_ADDRL		EQU  00H       ;"AP���ڵı�־"�ĵ͵�ַ
AP_EXIST_FLAG1		EQU  55H      ;"AP���ڵı�־1"
AP_EXIST_FLAG2		EQU  55H      ;"AP���ڵı�־2"

;������
COMM_ACK                EQU 0B0H      ;��λ������λ��˫����Ӧ���ź�
COMM_RUN_USER_CODE      EQU 0B1H      ;���� ISP ����, ����λ�л���Ӧ�ó���.
COMM_ERASE_USER_CODE    EQU 0B2H      ;����Ӧ�ó��� flash
COMM_WRITE_FLASH        EQU 0B3H      ;д���ݵ��û�Ӧ�ó�������(���¼�� AP ��)
COMM_ERASE_SECTOR_WRITE EQU 0B4H      ;д���ݵ� AP ��, дǰ��������
COMM_READ_VERSION       EQU 0B5H      ;���汾��

START_CHAR_1            EQU  5AH      ;ͨѶʱ�ĵ�һ��ǰ���ֽ�
START_CHAR_2            EQU 0A5H      ;ͨѶʱ�ĵڶ���ǰ���ֽ�

FILL_CHECK_RAM_CHAR     EQU  69H      ;��� RAM �������ż�ص�Ԫ���ַ�

;-----------------------------------------------------------
;�������(ȫ��Сд)
check_RAM_20H           EQU  20H     ;�����ż�� RAM
;-------------------------------
time_flag               EQU  21H
time_over_bit           EQU  0       ;��ʱ�����־λ
frist_byte_over_bit     EQU  1       ;ǰ���ֽڳ��������������־λ

;-------------------------------
my_flag                 EQU  23H
old_val_bit             EQU  0
check_sumOK_bit         EQU  1       ;����ȷ�յ�PC������ı�־, 1=����ȷ�յ�
ap_erase_bit		EQU  2
;-------------------------------
check_RAM_24H           EQU  24H     ;�����ż�� RAM

once_AP_erase           EQU  25H     ;��λ����Ϊ64H(0110,0100), ɾ��AP����9BH(1001,1101)
start_char_error_count  EQU  26H     ;ͨѶʱǰ���ֽڳ���������

check_RAM_28H           EQU  28H     ;�����ż�� RAM

t0_over_count           EQU  29H     ;T0���������

check_RAM_2CH           EQU  2CH     ;�����ż�� RAM

receive_byte_count      EQU  2DH     ;���ڵ�ǰ�ѽ����ֽ���

check_RAM_30H           EQU  30H     ;�����ż�� RAM

sending_sum_high        EQU  31H     ;�洢����ʱ��У��͸��ֽ�
sending_sum_low         EQU  32H     ;�洢����ʱ��У��͵��ֽ�

check_RAM_34H           EQU  34H     ;�����ż�� RAM

receive_sum_high        EQU  35H     ;�����ж�У��͸��ֽ�
receive_sum_low         EQU  36H     ;�����ж�У��͵��ֽ�

check_RAM_38H           EQU  38H     ;�����ż�� RAM
check_RAM_3CH           EQU  3CH     ;�����ż�� RAM

;count			EQU  3DH
count_ACK               EQU  3EH     ;��Ѷ֡������

;�����շ��������Ĵ�СΪ 142
rs232buffer0            EQU  40H     ;�����շ��������׵�ַ
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
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   000BH
    AJMP  Timer0
;------------------------------------------------
    ORG   0013H
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   001BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   0023H
    AJMP  UART
;------------------------------------------------
    ORG   002BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   0033H
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   003BH
    NOP
    NOP
    NOP
    NOP
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
    ORG   0050H
MAIN:
    MOV   SP, #0E0H

    ACALL Initial_MCU           ;ϵͳ��ʼ��
    ACALL Initial_RS232         ;���ڳ�ʼ��
    CLR	  P1.4 
    CLR	  P1.3

    ACALL Wait_P30_Change       ;��⴮�ڵ�ƽ�Ƿ��б仯,�ޱ仯��ʾ����������
   JNC   Goto_AP               ;����������, ����λ�л���Ӧ�ó���

;    ACALL Send_Byte             ;����һ���ֽ�

AP_CYCLE:    
    ACALL Set_T0_Overtime_MAX   ;���ó�ʱʱ��.

    MOV   count_ACK, #1         ;����Ѷ֡������
Main_1: 
    SETB  P1.3                        ;�ȴ�PC������
    JB    my_flag.check_sumOK_bit, Main_2 ;���յ�PC�������,1 �յ�PC������
    ORL   WDT_CONTR, #17H       ;ι��
    CLR	  P1.0
    SETB  P1.0
    JNB   time_flag.time_over_bit, Main_1
Goto_AP:
    AJMP  Run_User_Program      ;��ʱ, ����λ�л���Ӧ�ó���
Main_2:                         ;�յ�PC������
    CLR   ES                    ;��ֹ�����ж�
    CLR	  P1.7
    MOV   A, #1                 ;�����յ�3����Ѷ֡?
    CLR   C
    SUBB  A, count_ACK
    JC    Main_3                ;�ǵ�, ���ֳɹ�, ת Main_3
    MOV   A, rs232buffer4       ;�� PC ����
    CJNE  A, #COMM_ACK,  Goto_AP;������Ѷ֡, ����λ�л���Ӧ�ó���
    INC   count_ACK             ;��Ѷ֡������ + 1
    SJMP  Main_4                ;�ط�Ӧ���ź�
Main_3:
    MOV   A, rs232buffer4       ;�� PC ����
    CJNE  A, #COMM_ACK, Main_5  ;�յ���Ѷ֡
    AJMP  Goto_AP
    ;CLR   P1.1
Main_4:
    ACALL Send_Command_ACK      ;�ط�Ӧ���ź�
    SETB  P1.7
    AJMP  Main_End

Main_5:
    CJNE  A, #COMM_RUN_USER_CODE, Main_6  ;���� ISP ����, ����λ�л���Ӧ�ó���
    ACALL Send_Command_ACK      ;�ط�Ӧ���ź�
    ;ACALL IAP_Enable            ;�� IAP ����
    ;MOV   DPH, #AP_EXIST_ADDRH     ;����"AP�Ƿ���ڱ�־"�ĵ�ַ
   ; MOV   DPL, #AP_EXIST_ADDRL
   ; MOV   A,#55H
   ; ACALL Byte_Program
   ; INC   DPTR
  ;  MOV   A,#55H
  ;  ANL   ISP_CONTR, #7FH 	;�ر� IAP ����
    AJMP  Run_User_Program       ;����λ�л���Ӧ�ó���

Main_6:
    CJNE  A, #COMM_ERASE_USER_CODE, Main_7  ;���� AP ��?
    ACALL AP_Erase
    SJMP  Main_4                ;�ط�Ӧ���ź�
    
Main_7:
    CJNE  A, #COMM_WRITE_FLASH, Main_8  ;д���ݵ�flash book0 ������
    CLR	  my_flag.ap_erase_bit
    ;CLR   C                     
    ACALL Write_Flash_Sector    ;C=0: ������
    AJMP  Main_End
Main_8:
    CJNE  A, #COMM_ERASE_SECTOR_WRITE, Main_9 ;����������д����
    SETB  my_flag.ap_erase_bit
    ;SETB  C                     
    ACALL Write_Flash_Sector      ;C=1: ����������д����
    AJMP  Main_End

Main_9:
    CJNE  A, #COMM_READ_VERSION, Main_End

    MOV   Rs232buffer3, #0CH       ;���� �汾��, �� MCU ����
    MOV   Rs232buffer4, #COMM_READ_VERSION
    MOV   Rs232buffer5, #FIRMWARE_VERSION    ;�汾��, �м�����һ��С����.
    MOV   rs232buffer6, #MCU_TYPE_HIGH ;MCU ���Ÿ��ֽ�
    MOV   rs232buffer7, #MCU_TYPE_LOW ;MCU ���ŵ��ֽ�
    MOV   Rs232buffer8, #0FFH      ;���ֽڱ���δ��
    ACALL Send_RS232_Buffer        ;���ͻ���������

Main_End:
    ANL   ISP_CONTR, #7FH          ;�ر� IAP ����
    ACALL Set_T0_Overtime_MAX
    CLR   TR0                      ;�ر� T0
    CLR   my_flag.check_sumOK_bit
    MOV   receive_byte_count, #0   ;���㴮�ڽ�������ָ��

    MOV   start_char_error_count, #0FFH  ;ͨѶʱǰ���ֽڳ����������ó�ֵ
    SETB  ES
    AJMP  Main_1
;------------------------------------------------
IAP_Enable:                        ;�� IAP ����
    MOV   R0, #check_RAM_20H       ;��� check_RAM ��Ԫ�仯�����б仯��
                                   ;Ԥʾ�� RAM ����������ŵ�ԭ���ƻ���
    MOV   R2, #8
Check_RAM1:
    CJNE  @R0, #FILL_CHECK_RAM_CHAR, Goto_User_Program ;����λ�л���Ӧ�ó���
    MOV   A, #4
    ADD   A, R0
    MOV   R0, A
    DJNZ  R2, Check_RAM1
;----------------
IAP_Enable1:
    MOV   ISP_CONTR, #ENABLE_ISP_IAP ;�� IAP ����
    RET
Goto_User_Program:
    AJMP  Run_User_Program
;------------------------------------------------
;���:DPTR = �ֽڵ�ַ, A=�ֽ�
;�ֽڱ��

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
;��������, ���:DPTR = ������ַ
Sector_Erase:
    MOV  ISP_CMD, #03           ;Select Page Erase Mode
    ACALL Set_Address
    ACALL Trigger_ISP           ;Trigger ISP processing
    RET
;------------------------------------------------
;���� AP ��
AP_Erase:    
    NOP
    NOP
    NOP

    ACALL IAP_Enable            ;�� IAP ����

    ;��� once_AP_erase �Ƿ�Ϊ #64H�� ������ #64H ������λ�л���Ӧ�ó���
    ;MOV   A, once_AP_erase      ;��λ����Ϊ64H, ɾ�� AP ���� 9BH
    ;CJNE  A, #64H, AP_erase_ERR ;����λ�л���Ӧ�ó���
    MOV   R2, #SECTOR_NUMBER
    MOV   DPH, #0
AP_Erase_Loop:
    ACALL Sector_Erase
    INC   DPH
    INC   DPH
    DJNZ  R2, AP_Erase_Loop

    ANL   ISP_CONTR, #7FH       ;�ر� IAP ����

    MOV   once_AP_erase, #9BH   ;��λ����Ϊ64H, ɾ�� AP ���� 9BH
    RET
AP_Erase_ERR:
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
;���:DPTR = �ֽڵ�ַ
;����:A = �����ֽ�
;��һ�ֽ� IAP command
Byte_Verify:
    MOV   ISP_CMD, #01           ;Select Read AP Mode
    ACALL Set_Address
    MOV   ISP_DATA, #00H         ;�����ݼĴ���
    ACALL Trigger_ISP
    ;Now in processing.(CPU will halt here before completing)
    MOV   A, ISP_DATA            ;Data will be in ISP_DATA
    RET
;------------------------------------------------
;C=0:д����, ����������
;C=1;����������д����
Write_Flash_Sector:
    ;д����, ��� AP ���Ƿ���ȫ����������δ��ȫ������������λ�л���Ӧ�ó���
    ;MOV   A, once_AP_erase      ;��λ����Ϊ64H, ɾ�� AP ���� 9BH
    ;CJNE  A, #9BH, once_AP_erase_ERR ;����λ�л���Ӧ�ó���
Write_Flash_Sector0:

    ACALL IAP_Enable            ;�� IAP ����

    MOV   DPH, rs232buffer7     ;���õ�ַ
    MOV   DPL, rs232buffer8
    MOV	  C,   my_flag.ap_erase_bit
    JNC   Write_Flash_Sector_1
    ACALL Sector_Erase          ;����Flash����
Write_Flash_Sector_1:          ;д��Flash����, Խ��,����, ����λ������
    MOV   R2, rs232buffer10     ;���ݳ���
    MOV   R0, #rs232buffer11    ;�������ֽ�ָ��
    MOV	  R6, #0
    MOV	  R7, #0
Write_Flash_Sector_2:
    MOV   A, @R0
    ADD   A, R7
    MOV   R7, A
    CLR   A
    ADDC  A, R6
    MOV   R6, A
    
    MOV   A, @R0
    ACALL Byte_Program
    INC   R0
    INC   DPTR
    DJNZ  R2, Write_Flash_Sector_2
;----------------------
;Checksummat:                    ;�������ݿ�У���
    ;MOV   DPH, rs232buffer7     ;���ö�����ָ���ֵ
    ;MOV   DPL, rs232buffer8
   ;MOV   R2, rs232buffer10     ;���ݳ���
   ;MOV   R6, #0                ;���ݿ�У��͸��ֽ�
    ;MOV   R7, #0                ;���ݿ�У��͵��ֽ�
;Checksummat_1:
    ;ACALL Byte_Verify
    ;ADD   A, R7
    ;MOV   R7, A
    ;CLR   A
    ;ADDC  A, R6
   ;MOV   R6, A

    ;INC   DPTR
    ;DJNZ  R2, Checksummat_1

    ANL   ISP_CONTR, #7FH       ;�ر� IAP ����

    MOV   Rs232buffer3, #0AH    ;����У���
    MOV   Rs232buffer5, R6
    MOV   Rs232buffer6, R7
    ACALL Send_RS232_Buffer     ;���ͻ���������
    RET
;----------------------
Once_AP_Erase_ERR:
    AJMP  Run_User_Program      ;����λ�л���Ӧ�ó���
;------------------------------------------------
Start_Main:
    ANL   ISP_CONTR, #7FH 	;�ر� IAP ����
    MOV   my_flag, #0           ;���־

    MOV   once_AP_erase, #64H   ;��λ����Ϊ64H, ɾ�� AP ������ 9BH

    MOV   start_char_error_count, #0FFH  ;ͨѶʱǰ���ֽڳ����������ó�ֵ
    ACALL Initial_RS232         ;���ڳ�ʼ��
    LJMP  AP_CYCLE
;����λ�л���Ӧ�ó���
Run_User_Program: 
    ACALL IAP_Enable
    MOV   DPH, #AP_EXIST_ADDRH     ;����"AP�Ƿ���ڱ�־"�ĵ�ַ
    MOV   DPL, #AP_EXIST_ADDRL
    ACALL Byte_Verify		   ;����־1
    CJNE  A,#AP_EXIST_FLAG1,Start_Main
    INC   DPTR
    ACALL Byte_Verify
    CJNE  A,#AP_EXIST_FLAG2,Start_Main
    ANL   ISP_CONTR, #7FH 	;�ر� IAP ����
RUN:
    MOV   A, #20H
    MOV   check_RAM_20H, A
    MOV   check_RAM_28H, A
    MOV   once_AP_erase, A
    MOV   ISP_CONTR, A          ;0010,0000 ����������λ boot from user_code
    SJMP  $                     ;��������²���ִ�д�ָ��
;------------------------------------------------
Initial_MCU:                    ;ϵͳ��ʼ��
;---------------------   
    ;MOV   R3, #68H              ;12MHz ��ʱΪ 50mS
;Clear_RAM_Delay0:
    ;MOV   R0, #08H              ;RAM 08H--7FH �� 0
    ;MOV   R2, #0F7H
    ;CLR   A
;Clear_RAM_Delay1:
    ;MOV   @R0, A
    ;INC   R0
    ;DJNZ  R2, Clear_RAM_Delay1
    ;DJNZ  R3, Clear_RAM_Delay0
;---------------------   
    MOV   TMOD, #01H            ;0000,0001 ����T0Ϊ16λ��ʱ��
    ;MOV   IE, #80H		;��ֹ��ʱ0
    MOV   IE, #82H              ;1000,0010 ������ʱ��0�ж�, ���ж�
;---------------------   
    ;20H��24H��28H��2CH��30H��34H��38H��3CH �� 8 ����Ԫ��λ���ϳ�ʼ��
    ;Ϊ #FILL_CHECK_RAM_CHAR, ÿ�δ� IAP ����ʱ����� 8 ����Ԫ�仯��
    ;���б仯��Ԥʾ�� RAM ����������ŵ�ԭ���ƻ���������λ�л���Ӧ�ó���
    MOV   R0, #check_RAM_20H    ;��ʼ�� check_RAM
    MOV   R2, #8
Initial_MCU1:
    MOV   @R0, #FILL_CHECK_RAM_CHAR
    MOV   A, #4
    ADD   A, R0
    MOV   R0, A
    DJNZ  R2, Initial_MCU1         
;---------------------   
    MOV   my_flag, #0           ;���־
;---------------------   
    ;Ϊ��ǿ������, ÿ��д flashi ǰ��� AP ���Ƿ���ȫ��������δ��
    ;ȫ������������λ�л���Ӧ�ó���once_AP_erase �����Ƿ���ȫ��������־��
    MOV   once_AP_erase, #64H   ;��λ����Ϊ64H, ɾ�� AP ������ 9BH

    MOV   start_char_error_count, #0FFH  ;ͨѶʱǰ���ֽڳ����������ó�ֵ
;---------------------   
    RET
;------------------------------------------------
;��⴮�ڵ�ƽ�Ƿ��б仯,�ޱ仯��ʾ����������
Wait_P30_Change:
    MOV   A, #TEST_P30_TIME     ; ���ó�ʱʱ��.
    ACALL Set_T0_Overtime
     
    MOV   C, P3.0               ; store RXD/P3.0 value
    MOV   my_flag.old_val_bit, C
    Wait_P30_Change_1:
    JB    time_flag.time_over_bit, Wait_P30_Change_3  ;��ʱ���ޱ仯, ����
    MOV   C, P3.0
    JNB   my_flag.old_val_bit, Wait_P30_Change_2
    CPL   C
Wait_P30_Change_2:
    JNC   Wait_P30_Change_1
    SETB  C                     ;�б仯, ����
    RET
Wait_P30_Change_3:
    CLR   C
    RET
;------------------------------------------------
Initial_RS232:               ;���ڳ�ʼ��  

    MOV   SCON, #SCON_CONFIGURE

    MOV   A, #RELOAD_COUNT_HIGH
    MOV   RCAP2H, A
    MOV   TH2, A
    MOV   A, #RELOAD_COUNT_LOW
    MOV   RCAP2L, A
    MOV   TL2, A

    MOV   T2CON, #34H
    SETB  ES
    CLR   check_sumOK_bit        ;���"�յ�PC������"��־   
    MOV   receive_byte_count, #0 ;���㴮�ڽ�������ָ��   
    SETB  EA
    RET
;------------------------------------------------
Set_T0_Overtime_MAX:            ;�������ʱʱ��.
    MOV   A, #T0_WAIT_MAX
;------------------------------------------------
;���: A = weit time count
Set_T0_Overtime:                ;���ó�ʱʱ��.
    CLR   TR0                   ;ֹͣ��ʱ��0����
    MOV   t0_over_count, A
    CLR   A
    MOV   TL0, A
    MOV   TH0, A
    CLR   time_flag.time_over_bit ;�嶨ʱ�����־
    SETB  TR0                   ;������ʱ��0����
    RET
;------------------------------------------------
Send_Command_ACK:               ;�ط�Ӧ���ź�
    CLR   ES                    ;��ֹ�����ж�
    MOV   Rs232buffer3, #08H
    MOV   Rs232buffer4, #COMM_ACK 
    ACALL Send_RS232_Buffer     ;���ͻ���������
    RET
;------------------------------------------------
Send_RS232_Buffer:              ;���ͻ���������
    CLR   ES                    ;��ֹ�����ж�
    CLR   TR0                   ;ֹͣ��ʱ��0����

    MOV   Rs232buffer0, #START_CHAR_1
    MOV   Rs232buffer1, #START_CHAR_2
    MOV   Rs232buffer2, #00H

    MOV   R2, rs232buffer3      ;���ͳ��� -->R2
    DEC   R2
    DEC   R2
    DEC   R2
    CLR   A
    MOV   sending_sum_high, A   ;��0����ʱ��У���
    MOV   sending_sum_low, A
    MOV   R0, #rs232buffer0
Send_RS232_Buffer_1:
    MOV   A, @R0
    ACALL Send_Byte             ;����һ���ֽ�
    INC   R0
    DJNZ  R2, Send_RS232_Buffer_1

    MOV   R2, sending_sum_low   ;����У���
    MOV   A, sending_sum_high
    ACALL Send_Byte
    MOV   A, R2
    ACALL Send_Byte
    MOV   A, #16H               ;���ͽ�����
    ACALL Send_Byte

    CLR   my_flag.check_sumOK_bit ;���"�յ�PC������"��־
    MOV   receive_byte_count, #0  ;���㴮�ڽ�������ָ��
    SETB  REN                   ;�������ڽ�������
    SETB  ES                    ;���������ж�
    RET
;------------------------------------------------
Send_Byte:                      ;����һ���ֽ�
    MOV   R7, A

    MOV   A, #SCON_CONFIGURE
    CJNE  A, #0D0H, Send_Byte_a

    ;ʹ��żУ��
    MOV   A, R7
   CLR   SCON.3                 ;SCON.TB8, Ҫ���͵ĵ�9λ, żУ��
   JNB   PSW.0, Send_Byte_a
   SETB  SCON.3

Send_Byte_a:
    MOV   A, R7
    MOV   SBUF, A
    ADD   A, sending_sum_low    ;�ۼӷ���ʱ��У���
    MOV   sending_sum_low, A
    CLR   A
    ADDC  A, sending_sum_high   ;�ۼӷ���ʱ��У���
    MOV   sending_sum_high, A
    JNB   TI, $
    CLR   TI                    ;���㴮�ڷ����жϱ�־
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
UART:                           ;�����жϷ������
    JBC   RI, UART_1
    RETI                        ;����ʱʹ�õ��ǲ�ѯ��ʽ����ʹ���ж�
UART_1:                         ;�����жϱ�־ == 1,����һ���ֽ�. ��ʱ RI �ѱ���0
    PUSH  PSW
    PUSH  ACC
    MOV   A, SBUF               ;������յ����ֽ� 
    SETB  PSW.3                 ;ѡ�õ�3��ͨ�üĴ�����
    MOV   R3, receive_sum_high  ;�ݴ�У��͸�λ
    MOV   R4, receive_sum_low   ;�ݴ�У��͵�λ
    MOV   A, receive_byte_count ;���������Ƿ񳬳�
    SUBB  A, #143               ;ͨѶʱ����������ֽ���Ϊ128�������ֽ�Ϊ14, 128+14=142
    JNC   Clear_Receive_Byte_Count ;����������

    MOV   A, #rs232buffer0      ;���㴮�ڽ�������ָ��, ������յ����ֽ�
    ADD   A, receive_byte_count
    MOV   R0, A

    MOV   A, SBUF               ;������յ����ֽ�               
    MOV   @R0, A
    MOV   R7, A


    MOV   A, #SCON_CONFIGURE             ;��żУ��ʱ��������һ��
    CJNE  A, #0D0H, Receive_EVEN_2
    
;*******************************************
;    ���żУ��λ
    MOV   A, R7
    JB   P, Receive_EVEN_1               ;P=1
    JNB  RB8, Receive_EVEN_2             ;P=0 AND RB8=0 żУ����ȷ
    SJMP Clear_Receive_Byte_Count        ;żУ���, ���㴮�ڽ�������ָ��   
Receive_EVEN_1:
    JNB  RB8, Clear_Receive_Byte_Count   ;P=1 AND RB8=0 żУ���
Receive_EVEN_2:
;*******************************************
    
    CLR   A                              ;����һ���������ݵĵ�Ԫ
    INC   R0
    MOV   @R0, A
    DEC   R0
    MOV   A, R7
    ADD   A, receive_sum_low             ;�����ۼӺ�
    MOV   receive_sum_low, A
    CLR   A
    ADDC  A, receive_sum_high
    MOV   receive_sum_high, A
    
    INC   receive_byte_count             ;�������ڽ�������ָ��
    MOV   A, receive_byte_count
    MOV   R5, A
    CLR   C
    SUBB  A, #3
    JNC   Exceed_2_Byte                  ;���յ� 2 ��ǰ���ַ�

    DJNZ  R5, Receive_2_Byte             ;�յ����ǵ� 1 ���ַ���
    CJNE  @R0, #START_CHAR_1, start_char_error  ;�� 1 ���ַ�����ȷ
    MOV   receive_sum_low, #START_CHAR_1 ;����У��ͳ�ʼֵ
    MOV   receive_sum_high, #00H
    SJMP  Exit_UART                      ;�� 1 ���ַ���ȷ, ����
Receive_2_Byte:
    DJNZ  R5, Clear_Receive_Byte_Count   ;�յ����ǵ� 2 ���ַ���
    CJNE  @R0, #START_CHAR_2, start_char_error     ;�� 2 ���ַ�����ȷ
    SJMP  Exit_UART                      ;�� 2 ���ַ���ȷ, ����

Exceed_2_Byte:                           ;�յ��� 2 �����ϵ��ַ�
    MOV   A, receive_byte_count
    SUBB  A, #3
    JC    Exit_UART

    MOV   A, rs232buffer3                ;���Ƿ���յ�PC������У���
    CLR   C
    SUBB  A, #3
    SUBB  A, receive_byte_count
    JNC   Exit_UART                      ;���ó�ʱ, �˳�

    MOV   receive_sum_high, R3           ;��ʱ���յ����ۼӺͻ������, �����ۼ�У���
    MOV   receive_sum_low, R4            ;�ָ�ԭ����У���

    MOV   A, rs232buffer3                ;�����������㹻��?
    DEC   A
    CLR   C
    SUBB  A, receive_byte_count
    JNC   Exit_UART                      ;���ó�ʱ, �˳�

    MOV   A, #(rs232buffer0-3)           ;�������, ����ۼӺ��Ƿ���ȷ, �������Ƿ�Ϊ16H
    ADD   A, receive_byte_count          ;��PC������У���-->R1
    MOV   R1, A
    MOV   A, @R1
    CJNE  A, receive_sum_high, Clear_Receive_Byte_Count ;�ۼӺͲ���ȷ, ȡ�����ν���֡
    INC   R1
    MOV   A, @R1
    CJNE  A, receive_sum_low, Clear_Receive_Byte_Count  ;�ۼӺͲ���ȷ, ȡ�����ν���֡
    INC   R1
    MOV   A, @R1
    CJNE  A, #16H, Clear_Receive_Byte_Count             ;����������16H, ȡ�����ν���֡
    SETB  my_flag.check_sumOK_bit        ;������ȷ
    CLR   ES
Clear_Receive_Byte_Count:
    MOV   receive_byte_count, #0         ;���㴮�ڽ�������ָ��
Exit_UART:
    ACALL Set_T0_Overtime_MAX
    POP   ACC
    POP   PSW
    RETI
start_char_error:
    MOV   receive_byte_count, #0
    DJNZ  start_char_error_count, Exit_UART ;ǰ���ֽڳ���������-1 = 0 ?
    SETB  time_flag.frist_byte_over_bit
    SJMP  Exit_UART

;**********************************************************
    END
;**********************************************************