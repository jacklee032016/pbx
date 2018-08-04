belloff:	DS	1;不在振铃标志1有效
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
PLAY_USER:;播放录音
		MOV	P2,#11111001B	
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET
RSEG	?PR?STOP_USER?ASM
STOP_USER:;停止放音
		MOV	P2,#11111010B
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		MOV	P0,#0FFH
		RET
RSEG	?PR?START_RECORD?ASM
START_RECORD:	;启动录音	
		NOP
		MOV	P2,#11111011B
		NOP	
		CLR	EPM7064_CS
		NOP
		SETB	EPM7064_CS
		RET
RSEG	?PR?STOP_RECORD?ASM
STOP_RECORD:	;停止录音	
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
ALL_ICS_RESET:	;复位IC
		LCALL	DTMF_OE_DISABLE
		CLR	DTMFGENCLK
		LCALL	RST_MT8816
		LCALL	RST_ISD1420
		MOV	BOHAOTONECON,#0;	/*;拨号音控制寄存器*/
		LCALL	SEND_BOHAOTONE
		LCALL	HT1380CS_Disable
		RET

;; DTMF encoder
RSEG	?PR?DTMF_ENCODE_FIRST_DISABLE?ASM		
DTMF_ENCODE_FIRST_DISABLE:;第0通道双音频发生器无效
		MOV	P2,#11111101B	
		NOP
		NOP
		CLR	EPM7064_CS	;CPLD
		NOP
		NOP
		SETB	EPM7064_CS
		RET	

RSEG	?PR?DTMF_ENCODE_SECOND_DISABLE?ASM		
DTMF_ENCODE_SECOND_DISABLE:;第0通道双音频发生器无效
		MOV	P2,#11111110B	
		NOP
		NOP
		CLR	EPM7064_CS
		NOP
		NOP
		SETB	EPM7064_CS
		RET		

RSEG	?PR?_DTMF_ENCODE_FIRST_ENABLE?ASM		
_DTMF_ENCODE_FIRST_ENABLE:	;第0通道双音频发生器有效
		LCALL	DTMF_ENCODE_FIRST_DISABLE	;刷新数据
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
_DTMF_ENCODE_SECOND_ENABLE:	;第1通道双音频发生器有效
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
DTMF_OE_DISABLE:	;关闭DTMF01数据输出释放总线
		CLR	HC138EN
		RET
		
RSEG	?PR?_DTMF_OE_ENABLE?ASM		
_DTMF_OE_ENABLE:	;使能输出
		PUSH	ACC
		PUSH	PSW
		MOV	A,R7
		CLR	C
		RRC	A
		ANL	P2,#0F0H	;  与操作
		ORL	P2,A		;  或操作
		SETB	HC138EN		;使能74HC138译码
		POP	PSW
		POP	ACC
		RET

; 分机振铃的继电器控制
RSEG	?PR?_EXT_RELAY_CONTROL?ASM
_EXT_RELAY_CONTROL:;发送话机继电器跳开/闭合参数
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

; 外线摘挂机的继电器控制
RSEG	?PR?_LINES_RELAY_CONTROL?ASM
_LINES_RELAY_CONTROL:;发送中继继电器跳开/闭合参数
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

;蜂鸣器
RSEG	?PR?BUZZER_ON?ASM   
BUZZER_ON:;蜂鸣器开	
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
BUZZER_OFF:;蜂鸣器关	
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
_SEND_BUSYCOME:;发送来电通道切换、忙音信号
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
SEND_BOHAOTONE:	;发送拨号音控制信息
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
_START_DIAL_TONE:;送拨号音
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		MOV	A,R7			;C调用时传递参数
		LCALL	Bytetobit1		;数字转换为位
		ORL	A,BOHAOTONECON
		MOV	BOHAOTONECON,A
		LCALL	SEND_BOHAOTONE
		POP	PSW
		POP	B
		POP	ACC
		RET
RSEG	?PR?_STOP_DIAL_TONE?ASM
_STOP_DIAL_TONE:	;停拨号音
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		MOV	A,R7
Stop_bohaoyin:
		LCALL	Bytetobit0		;数字转换为位
		ANL	A,BOHAOTONECON
		MOV	BOHAOTONECON,A
		LCALL	SEND_BOHAOTONE
		POP	PSW
		POP	B
		POP	ACC
		RET
;*************************************************************************************************				
Bytetobit0:				;关
		INC	A
		MOV	B,A
		MOV	A,#0FFH
		CLR	C
		RLC	A
		DJNZ	B,$-1
		RET
;*************************************************************************************************
Bytetobit1:				;开
		INC	A
		MOV	B,A
		CLR	A
		SETB	C
		RLC	A
		DJNZ	B,$-1
		RET

RSEG	?PR?_RD_EXTSTATUS?ASM                                                                                                                                                                                                                                                                                                                                                                          
_RD_EXTSTATUS:;读话机摘挂机状态
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
_connect:;连接点（x,y)
		PUSH	ACC
		PUSH	B
		PUSH	PSW
		mov	a,r7
		mov	b,r5
connect:;
		CLR	C			;A中为8816的X坐标   B中为8816的Y坐标
		XCH	A,B
		SUBB	A,#08H
		JC	Connect00		;根据y轴判断操作对象
		XCH	A,B			;操作第二片
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
		mov	b,r5		;操作第一片
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
_disconnect:;断开点	
		PUSH	ACC
		PUSH	B
		PUSH	PSW	
		mov	a,r7
		mov	b,r5
Disconnect:
		CLR	C			;A中为8816的X坐标   B中为8816的Y坐标
		XCH	A,B
		SUBB	A,#08H
		JC	Disconnect00		;根据y轴判断操作对象
		XCH	A,B			;操作第二片
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
		mov	b,r5			;操作第一片
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
;	CLR 	RST		;复位引脚为低电平所有数据传送终止
	NOP
	NOP
	CLR 	SCLK		;清时钟总线
	NOP
	NOP
	LCALL	HT1380CS_Enable
;	SETB 	RST		;复位引脚为高电平逻辑控制有效
	NOP
	NOP
	MOV 	A,R7		;准备发送命令字节
	MOV 	BitCnt,#08h	;传送位数为8
S_Byte0:
	RRC 	A		;将最低位传送给进位位C
	MOV 	IO_DATA,C	;位传送至数据总线
	NOP
	NOP
	SETB 	SCLK		;时钟上升沿发送数据有效
	NOP
	NOP
	CLR 	SCLK		;清时钟总线
	DJNZ 	BitCnt,S_Byte0	;位传送未完毕则继续
	NOP
	NOP
S_Byte1:			;准备发送数据
	MOV 	A,@R0		;传送数据过程与传送命令相同
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
	INC 	R0 		;发送数据的内存地址加1
	DJNZ 	B,S_Byte1	;字节传送未完毕则继续
	NOP
	NOP
	LCALL	HT1380CS_Disable
;	CLR 	RST 		;逻辑操作完毕清RST
	POP	B
	POP	ACC
	MOV	R0,A
	POP	PSW
	POP	ACC	
	RET
;********************************************************************
;接收数据程序;
;名称:TIME_BYTES_READ
;描述:从被控器HT1380 接收ByteCnt 个字节数据
;命令字节地址在Command 中
;所接收数据的字节数在ByteCnt 中接收的数据在RcvDat 缓冲区中
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
;	CLR 	RST 		;复位引脚为低电平所有数据传送终止
	NOP
	NOP
	CLR 	SCLK 		;清时钟总线
	NOP
	NOP
	LCALL	HT1380CS_Enable
;	SETB 	RST 		;复位引脚为高电平逻辑控制有效
	MOV 	A,R7	 	;准备发送命令字节
	MOV 	BitCnt,#08h 	;传送位数为8
R_Byte0:
	RRC 	A		; 将最低位传送给进位位C
	MOV 	IO_DATA,C 	;位传送至数据总线
	NOP
	NOP
	SETB 	SCLK 		;时钟上升沿发送数据有效
	NOP
	NOP
	CLR 	SCLK 		;清时钟总线
	DJNZ 	BitCnt,R_Byte0 	;位传送未完毕则继续
	NOP
	NOP
R_Byte1: 			;准备接收数据
	CLR 	A 		;清类加器
	CLR 	C 		;清进位位C
	MOV 	BitCnt,#08h 	;接收位数为8
R_Byte2:
	NOP
	NOP
	MOV 	C,IO_DATA 	;数据总线上的数据传送给C
	RRC 	A		;从最低位接收数据
	SETB 	SCLK 		;时钟总线置高
	NOP
	NOP
	CLR 	SCLK 		;时钟下降沿接收数据有效
	DJNZ 	BitCnt,R_Byte2 	;位接收未完毕则继续
	MOV 	@R0,A 		;接收到的完整数据字节放入接收内存缓冲区
	INC 	R0 		;接收数据的内存地址加1
	DJNZ 	B,R_Byte1	;字节接收未完毕则继续
	NOP
	NOP
	LCALL	HT1380CS_Disable
;	CLR 	RST 		;逻辑操作完毕清RST
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

