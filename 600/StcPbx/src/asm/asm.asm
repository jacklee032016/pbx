; $Id: asm.asm,v 1.5 2007/07/25 17:49:59 lizhijie Exp $
; define BIT address in BIT memory space 

DTMFGENCLK	BIT		P1.0
MT8816_CS	BIT		P1.1
MT8816_ST0	BIT		P1.2
MT8816_ST1	BIT		P1.3
CTS		BIT		P1.4
SCLK 		BIT		P1.5
IIC_CK		BIT		P1.6
COMSDA		BIT		P1.7
IO_DATA		BIT		P1.7

HC138A		BIT		P2.0
HC138B		BIT		P2.1
HC138C		BIT		P2.2
HC138D		BIT		P2.3
EPM7064_CS	BIT		P2.4
HOOKRD_CS	BIT		P2.5
BACKRD_CS	BIT		P2.6
DTMFRD_CS	BIT		P2.7

HC138EN		BIT		P3.4


;FSK_LDCK	BIT		P3.4
;FSK_LDDATA	BIT		P3.5
;TOE23		BIT		P3.6
;FSKDOUT		BIT		P3.7

; name of current module
NAME	ASM					;定义模块名

; code segment
?PR?ALL_ICS_RESET?ASM			SEGMENT	CODE
?PR?delay5ms?ASM			SEGMENT	CODE

?PR?DELAY5US?ASM			SEGMENT	CODE


; data segment(first 128 bytes)
?DT?ASM					SEGMENT DATA
RSEG	?DT?ASM	;relocatable segment

BitCnt:		DS	1

; BIT segment 
?BI?ASM					SEGMENT BIT
RSEG	?BI?ASM
COMBIT:		DBIT	1

PUBLIC	ALL_ICS_RESET, delay5ms

PUBLIC	DELAY5US


;公开函数名以便C模块可调用他们
; select this relacatable(generic) segment, so counter used for this segment

		
RSEG	?PR?ALL_ICS_RESET?ASM	
ALL_ICS_RESET:	;复位IC
		CLR	DTMFGENCLK
;		LCALL	HT1380CS_Disable
		RET
;============================================

RSEG	?PR?delay5ms?ASM		
delay5ms:
		PUSH	DPL	
		PUSH	DPH
	DEL5002:	
		MOV	DPL,#23
	DEL5001:	
		MOV	DPH,#198
		DJNZ	DPH,$
		DJNZ	DPL,DEL5001
		POP	DPH
		POP	DPL
		RET	
		

; used only when FM24C08 is accessed
RSEG	?PR?DELAY5US?ASM		
DELAY5US:	
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		RET

	END

