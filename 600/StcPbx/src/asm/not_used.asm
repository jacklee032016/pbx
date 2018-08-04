belloff:	DS	1;���������־1��Ч
PUBLIC  belloff

BOHAOTONECON:	DS	1
PUBLIC	_RD_EXTSTATUS,_RD_TRKSTATUS
PUBLIC	_START_DIAL_TONE, _STOP_DIAL_TONE
PUBLIC	_SEND_BUSYCOME
PUBLIC  _TIME_BYTES_WRITE, _TIME_BYTES_READ

?PR?_TIME_BYTES_WRITE?ASM		SEGMENT	CODE
?PR?_TIME_BYTES_READ?ASM		SEGMENT	CODE

?PR?_RD_EXTSTATUS?ASM			SEGMENT	CODE
?PR?_RD_TRKSTATUS?ASM			SEGMENT	CODE
?PR?_START_DIAL_TONE?ASM		SEGMENT	CODE
?PR?_STOP_DIAL_TONE?ASM			SEGMENT	CODE
?PR?_SEND_BUSYCOME?ASM			SEGMENT	CODE

;?PR?_DTMF_OE_ENABLE?ASM			SEGMENT	CODE
;?PR?DTMF_OE_DISABLE?ASM			SEGMENT	CODE		
;?PR?_EXT_RELAY_CONTROL?ASM		SEGMENT	CODE
;?PR?_LINES_RELAY_CONTROL?ASM		SEGMENT	CODE
;?PR?PLAY_USER?ASM			SEGMENT	CODE
;?PR?STOP_USER?ASM			SEGMENT	CODE
;?PR?START_RECORD?ASM			SEGMENT	CODE
;?PR?STOP_RECORD?ASM			SEGMENT	CODE
;?PR?_WR24C08_single?ASM			SEGMENT	CODE
;?PR?_rd24C08_single?ASM			SEGMENT	CODE
;?PR?_rd24C08_multi?ASM			SEGMENT	CODE
;?PR?_wr24C08_multi?ASM			SEGMENT	CODE

;?PR?_DTMF_ENCODE_FIRST_ENABLE?ASM	SEGMENT	CODE
;?PR?DTMF_ENCODE_FIRST_DISABLE?ASM	SEGMENT	CODE
;?PR?_DTMF_ENCODE_SECOND_ENABLE?ASM	SEGMENT	CODE
;?PR?DTMF_ENCODE_SECOND_DISABLE?ASM	SEGMENT	CODE

?PR?BUZZER_ON?ASM  			SEGMENT	CODE
?PR?BUZZER_OFF?ASM			SEGMENT	CODE
?PR?RST_MT8816?ASM			SEGMENT	CODE
?PR?_connect?ASM			SEGMENT	CODE
?PR?_disconnect?ASM			SEGMENT	CODE

; define public symbol which can be used by other modules 
;PUBLIC	 DTMF_OE_DISABLE, _DTMF_OE_ENABLE
;PUBLIC	_DTMF_ENCODE_FIRST_ENABLE,DTMF_ENCODE_FIRST_DISABLE
;PUBLIC	_DTMF_ENCODE_SECOND_ENABLE,DTMF_ENCODE_SECOND_DISABLE
;PUBLIC	_EXT_RELAY_CONTROL, _LINES_RELAY_CONTROL
;PUBLIC	START_RECORD, STOP_RECORD, PLAY_USER, STOP_USER
;PUBLIC	_rd24C08_single, _WR24C08_single, _wr24C08_multi,_rd24C08_multi
PUBLIC	RST_MT8816, BUZZER_ON, BUZZER_OFF
PUBLIC	_connect, _disconnect

PUBLIC	BOHAOTONECON

; code for recorder
RSEG	?PR?PLAY_USER?ASM
PLAY_USER:;����¼��
		MOV	P2,#11111001B	
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET
RSEG	?PR?STOP_USER?ASM
STOP_USER:;ֹͣ����
		MOV	P2,#11111010B
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		RET
RSEG	?PR?START_RECORD?ASM
START_RECORD:	;����¼��	
		NOP
		MOV	P2,#11111011B
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET
RSEG	?PR?STOP_RECORD?ASM
STOP_RECORD:	;ֹͣ¼��	
		MOV	P2,#11111100B
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET	


RST_ISD1420:
		MOV	P2,#11111010B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P2,#11111100B
		NOP
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET


RSEG	?PR?ALL_ICS_RESET?ASM	
ALL_ICS_RESET:	;��λIC
		LCALL	DTMF_OE_DISABLE
		CLR	DTMFGENCLK
		LCALL	RST_MT8816
		LCALL	RST_ISD1420
		MOV	BOHAOTONECON,#0;	/*;���������ƼĴ���*/
		LCALL	SEND_BOHAOTONE
		LCALL	HT1380CS_Disable
		RET

;; DTMF encoder
RSEG	?PR?DTMF_ENCODE_FIRST_DISABLE?ASM		
DTMF_ENCODE_FIRST_DISABLE:;��0ͨ��˫��Ƶ��������Ч
		MOV	P2,#11111101B	
		NOP
		NOP
		CLR	EPM7064_CS	;CPLD
		NOP
		NOP
		SETB	EPM7064_CS
		RET	

RSEG	?PR?DTMF_ENCODE_SECOND_DISABLE?ASM		
DTMF_ENCODE_SECOND_DISABLE:;��0ͨ��˫��Ƶ��������Ч
		MOV	P2,#11111110B	
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		NOP
		SETB	EPM7064_CS
		RET		

RSEG	?PR?_DTMF_ENCODE_FIRST_ENABLE?ASM		
_DTMF_ENCODE_FIRST_ENABLE:	;��0ͨ��˫��Ƶ��������Ч
		LCALL	DTMF_ENCODE_FIRST_DISABLE	;ˢ������
		MOV	P0,R7
		NOP
		NOP
		SETB	DTMFGENCLK
		NOP
		NOP
		CLR	DTMFGENCLK
		NOP
		NOP
		MOV	P2,#11110101B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		NOP
		SETB	EPM7064_CS
		RET
RSEG	?PR?_DTMF_ENCODE_SECOND_ENABLE?ASM	
_DTMF_ENCODE_SECOND_ENABLE:	;��1ͨ��˫��Ƶ��������Ч
		LCALL	DTMF_ENCODE_SECOND_DISABLE
		PUSH	ACC
		MOV	A,R7
		SWAP	A
		MOV	P0,A
		POP	ACC
		NOP
		NOP
		SETB	DTMFGENCLK
		NOP
		NOP
		CLR	DTMFGENCLK
		NOP
		NOP
		MOV	P2,#11110110B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		NOP
		SETB	EPM7064_CS
		RET


;; DTMF decoder
RSEG	?PR?DTMF_OE_DISABLE?ASM
DTMF_OE_DISABLE:	;�ر�DTMF01��������ͷ�����
		CLR	HC138EN
		RET
		
RSEG	?PR?_DTMF_OE_ENABLE?ASM		
_DTMF_OE_ENABLE:	;ʹ�����
		PUSH	ACC
		PUSH	PSW
		MOV	A,R7
		CLR	C
		RRC	A
		ANL	P2,#0F0H	;  �����
		ORL	P2,A		;  �����
		SETB	HC138EN		;ʹ��74HC138����
		POP	PSW
		POP	ACC
		RET

; �ֻ�����ļ̵�������
RSEG	?PR?_EXT_RELAY_CONTROL?ASM
_EXT_RELAY_CONTROL:;���ͻ����̵�������/�պϲ���
		PUSH	ACC
		PUSH	PSW
		mov	a,r7
		MOV	P0,A
		NOP
		MOV	P2,#11110111B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		POP	PSW
		POP	ACC
		RET

; ����ժ�һ��ļ̵�������
RSEG	?PR?_LINES_RELAY_CONTROL?ASM
_LINES_RELAY_CONTROL:;�����м̵̼�������/�պϲ���
		PUSH	ACC
		PUSH	PSW
		mov	a,r7
		MOV	P0,A
		NOP
		MOV	P2,#11111000B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		POP	PSW
		POP	ACC
		RET

;������
RSEG	?PR?BUZZER_ON?ASM   
BUZZER_ON:;��������	
		PUSH	ACC
		MOV	A,R7
		PUSH	ACC
		MOV	A,track_rly
		SETB	ACC.5
		MOV	r7,A
		LCALL	_LINES_RELAY_CONTROL
		POP	ACC
		MOV	R7,A
		POP	ACC
		RET
RSEG	?PR?BUZZER_OFF?ASM  
BUZZER_OFF:;��������	
		PUSH	ACC
		MOV	A,R7
		PUSH	ACC
		MOV	A,track_rly
		CLR	ACC.5
		MOV	r7,A
		LCALL	_LINES_RELAY_CONTROL
		POP	ACC
		MOV	R7,A
		POP	ACC
		RET

RSEG	?PR?RST_MT8816?ASM	
RST_MT8816:	
		PUSH	ACC
		MOV	A,R7
		PUSH	ACC
		CLR	MT8816_CS
		CLR	MT8816_ST0
		CLR	MT8816_ST1
		MOV	A,track_rly
		CLR	ACC.4
		MOV	R7,A
		LCALL	_LINES_RELAY_CONTROL
		MOV	A,track_rly
		SETB	ACC.4
		MOV	R7,A
		LCALL	_LINES_RELAY_CONTROL
		POP	ACC
		MOV	R7,A
		POP	ACC
		RET

RSEG	?PR?_SEND_BUSYCOME?ASM	
_SEND_BUSYCOME:;��������ͨ���л���æ���ź�
		PUSH	ACC
		MOV	A,r7
		MOV	P0,A
		NOP
		MOV	P2,#11110010B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		POP	ACC
		RET

;============================================	
SEND_BOHAOTONE:	;���Ͳ�����������Ϣ
		MOV	A,BOHAOTONECON
		MOV	P0,A
		NOP
		MOV	P2,#11110001B
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		RET
		
RSEG	?PR?_START_DIAL_TONE?ASM
_START_DIAL_TONE:;�Ͳ�����
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		MOV	A,R7			;C����ʱ���ݲ���
		LCALL	Bytetobit1		;����ת��Ϊλ
		ORL	A,BOHAOTONECON
		MOV	BOHAOTONECON,A
		LCALL	SEND_BOHAOTONE
		POP	PSW
		POP	B
		POP	ACC
		RET
RSEG	?PR?_STOP_DIAL_TONE?ASM
_STOP_DIAL_TONE:	;ͣ������
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		MOV	A,R7
Stop_bohaoyin:
		LCALL	Bytetobit0		;����ת��Ϊλ
		ANL	A,BOHAOTONECON
		MOV	BOHAOTONECON,A
		LCALL	SEND_BOHAOTONE
		POP	PSW
		POP	B
		POP	ACC
		RET
;*************************************************************************************************				
Bytetobit0:				;��
		INC	A
		MOV	B,A
		MOV	A,#0FFH
		CLR	C
		RLC	A
		DJNZ	B,$-1
		RET
;*************************************************************************************************
Bytetobit1:				;��
		INC	A
		MOV	B,A
		CLR	A
		SETB	C
		RLC	A
		DJNZ	B,$-1
		RET

RSEG	?PR?_RD_EXTSTATUS?ASM                                                                                                                                                                                                                                                                                                                                                                          
_RD_EXTSTATUS:;������ժ�һ�״̬
		PUSH	PSW
		PUSH	ACC
		PUSH	0
		mov	a,r7		; address of C variable in R7
		CLR	RS1
		CLR	RS0
		mov	r0,a		; put this address into R0
		MOV	P0,#0FFH	
		CLR	HOOKRD_CS	; chip select
		MOV	A,P0		; read P0
		CPL	A		; nagate A
		ANL	A,belloff	; mask this pin which is ringing now
		SETB	HOOKRD_CS	
		MOV	@r0,a		; store value into C variable
	RD_EXTSTATUSOUT:
		POP	0
		POP	ACC
		POP	PSW
		RET

; read track voltage level into low nibble of ldstatus and high nibble of backsoundbuf
; when a bit is low, it is means activating or ringing 
RSEG	?PR?_RD_TRKSTATUS?ASM                                                                                                                                                                                                                                                                                                                                                                          
_RD_TRKSTATUS:;
		PUSH	PSW
		PUSH	ACC
		PUSH	B
		PUSH	0
		mov	a,r5		; second parameter, backsoundbuf
		MOV	B,R7		; first parameter, ldstatus
		CLR	RS1
		CLR	RS0
		mov	r0,a		; move address of backsoundbuf into R0
		MOV	P0,#0FFH
		CLR	BACKRD_CS	; set CS signal
		MOV	A,P0		; read P0 into A
		MOV	@R0,A		; move A into address stored in R0
		SETB	BACKRD_CS
		SWAP	A
		MOV	R0,B
		MOV	@r0,a		; save ldstatus
	RD_TRKSTATUSOUT:
		POP	0
		POP	B
		POP	ACC
		POP	PSW
		RET

		
RSEG	?PR?_connect?ASM
_connect:;���ӵ㣨x,y)
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		mov	a,r7
		mov	b,r5
connect:;
		CLR	C			;A��Ϊ8816��X����   B��Ϊ8816��Y����
		XCH	A,B
		SUBB	A,#08H
		JC	Connect00		;����y���жϲ�������
		XCH	A,B			;�����ڶ�Ƭ
		SWAP	A
		RR	A
		ORL	A,B
		SETB	ACC.7			;ON
		MOV	P0,A
		SETB	MT8816_CS		;8816Enable
		NOP
		SETB	MT8816_ST1		;RD the data
		NOP
		CLR	MT8816_ST1
		NOP
		CLR	MT8816_CS		;8816Disable
		MOV	P0,#0FFH
		LJMP	Connectout
	Connect00:
		mov	a,r7
		mov	b,r5		;������һƬ
		SWAP	A
		RR	A
		ORL	A,B
		SETB	ACC.7			;ON
		MOV	P0,A
		SETB	MT8816_CS		;8816Enable
		NOP
		SETB	MT8816_ST0		;RD the data
		NOP
		CLR	MT8816_ST0
		NOP
		CLR	MT8816_CS		;8816Disable
		MOV	P0,#0FFH
	Connectout:
		POP	PSW
		POP	B
		POP	ACC
		RET
;*************************************************************************************************		

RSEG	?PR?_disconnect?ASM
_disconnect:;�Ͽ���	
		PUSH	ACC
		PUSH	B
		PUSH	PSW	
		mov	a,r7
		mov	b,r5
Disconnect:
		CLR	C			;A��Ϊ8816��X����   B��Ϊ8816��Y����
		XCH	A,B
		SUBB	A,#08H
		JC	Disconnect00		;����y���жϲ�������
		XCH	A,B			;�����ڶ�Ƭ
		SWAP	A
		RR	A
		ORL	A,B
		CLR	ACC.7			;OFF
		MOV	P0,A
		SETB	MT8816_CS		;8816Enable
		NOP
		SETB	MT8816_ST1		;RD the data
		NOP
		CLR	MT8816_ST1
		NOP
		CLR	MT8816_CS		;8816Disable
		MOV	P0,#0FFH
		LJMP	Disconnectout
	Disconnect00:
		mov	a,r7
		mov	b,r5			;������һƬ
		SWAP	A
		RR	A
		ORL	A,B
		CLR	ACC.7			;OFF
		MOV	P0,A
		SETB	MT8816_CS		;8816Enable
		NOP
		SETB	MT8816_ST0		;RD the data
		NOP
		CLR	MT8816_ST0
		NOP
		CLR	MT8816_CS		;8816Disable
		MOV	P0,#0FFH
	Disconnectout:
		POP	PSW
		POP	B
		POP	ACC
		RET

RSEG	?PR?_TIME_BYTES_WRITE?ASM		
_TIME_BYTES_WRITE:
	PUSH	ACC
	PUSH	PSW
	MOV	A,R0
	PUSH	ACC
	PUSH	B
	MOV	B,R3
	MOV	A,R5
	MOV	R0,A
	LCALL	HT1380CS_Disable
;	CLR 	RST		;��λ����Ϊ�͵�ƽ�������ݴ�����ֹ
	NOP
	NOP
	CLR 	SCLK		;��ʱ������
	NOP
	NOP
	LCALL	HT1380CS_Enable
;	SETB 	RST		;��λ����Ϊ�ߵ�ƽ�߼�������Ч
	NOP
	NOP
	MOV 	A,R7		;׼�����������ֽ�
	MOV 	BitCnt,#08h	;����λ��Ϊ8
S_Byte0:
	RRC 	A		;�����λ���͸���λλC
	MOV 	IO_DATA,C	;λ��������������
	NOP
	NOP
	SETB 	SCLK		;ʱ�������ط���������Ч
	NOP
	NOP
	CLR 	SCLK		;��ʱ������
	DJNZ 	BitCnt,S_Byte0	;λ����δ��������
	NOP
	NOP
S_Byte1:			;׼����������
	MOV 	A,@R0		;�������ݹ����봫��������ͬ
	MOV 	BitCnt,#08h
S_Byte2:
	RRC 	A
	MOV 	IO_DATA,C
	NOP
	NOP
	SETB 	SCLK
	NOP
	NOP
	CLR 	SCLK
	DJNZ 	BitCnt,S_Byte2
	INC 	R0 		;�������ݵ��ڴ��ַ��1
	DJNZ 	B,S_Byte1	;�ֽڴ���δ��������
	NOP
	NOP
	LCALL	HT1380CS_Disable
;	CLR 	RST 		;�߼����������RST
	POP	B
	POP	ACC
	MOV	R0,A
	POP	PSW
	POP	ACC	
	RET
;********************************************************************
;�������ݳ���;
;����:TIME_BYTES_READ
;����:�ӱ�����HT1380 ����ByteCnt ���ֽ�����
;�����ֽڵ�ַ��Command ��
;���������ݵ��ֽ�����ByteCnt �н��յ�������RcvDat ��������
;********************************************************************
RSEG	?PR?_TIME_BYTES_READ?ASM	
_TIME_BYTES_READ:
	PUSH	ACC
	PUSH	PSW
	MOV	A,R0
	PUSH	ACC
	PUSH	B
	MOV	B,R3
	MOV	A,R5
	MOV	R0,A
	LCALL	HT1380CS_Disable	
;	CLR 	RST 		;��λ����Ϊ�͵�ƽ�������ݴ�����ֹ
	NOP
	NOP
	CLR 	SCLK 		;��ʱ������
	NOP
	NOP
	LCALL	HT1380CS_Enable
;	SETB 	RST 		;��λ����Ϊ�ߵ�ƽ�߼�������Ч
	MOV 	A,R7	 	;׼�����������ֽ�
	MOV 	BitCnt,#08h 	;����λ��Ϊ8
R_Byte0:
	RRC 	A		; �����λ���͸���λλC
	MOV 	IO_DATA,C 	;λ��������������
	NOP
	NOP
	SETB 	SCLK 		;ʱ�������ط���������Ч
	NOP
	NOP
	CLR 	SCLK 		;��ʱ������
	DJNZ 	BitCnt,R_Byte0 	;λ����δ��������
	NOP
	NOP
R_Byte1: 			;׼����������
	CLR 	A 		;�������
	CLR 	C 		;���λλC
	MOV 	BitCnt,#08h 	;����λ��Ϊ8
R_Byte2:
	NOP
	NOP
	MOV 	C,IO_DATA 	;���������ϵ����ݴ��͸�C
	RRC 	A		;�����λ��������
	SETB 	SCLK 		;ʱ�������ø�
	NOP
	NOP
	CLR 	SCLK 		;ʱ���½��ؽ���������Ч
	DJNZ 	BitCnt,R_Byte2 	;λ����δ��������
	MOV 	@R0,A 		;���յ������������ֽڷ�������ڴ滺����
	INC 	R0 		;�������ݵ��ڴ��ַ��1
	DJNZ 	B,R_Byte1	;�ֽڽ���δ��������
	NOP
	NOP
	LCALL	HT1380CS_Disable
;	CLR 	RST 		;�߼����������RST
	POP	B
	POP	ACC
	MOV	R0,A
	POP	PSW
	POP	ACC
	RET
HT1380CS_Enable:
		MOV	P2,#11110000B
		NOP
		NOP
		CLR	EPM7064_CS
		RET
HT1380CS_Disable:
		MOV	P2,#0F0H
		NOP
		NOP
		SETB	EPM7064_CS
		RET

