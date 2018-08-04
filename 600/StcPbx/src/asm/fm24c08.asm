
?PR?DELAY5US?ASM			SEGMENT	CODE

PUBLIC	DELAY5US

RSEG	?PR?_WR24C08_single?ASM
_WR24C08_single:	;写数据单字节
		PUSH	ACC
		SETB	COMBIT
	        LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A0H		;确定高地址和写命令
		LCALL	ARL_SDA		;写入高地址
		LCALL	INACK
		JB	F0,WR24_1
		LJMP	WR24_4
	WR24_1:	
		MOV	A,r5
		LCALL	ARL_SDA		;写入低地址
		LCALL	INACK
		JB	F0,WR24_2
		LJMP	WR24_4
	WR24_2:	
		MOV	A,r7	;写入数据
		LCALL	ARL_SDA
		LCALL	INACK
		JB	F0,WR24_5			
	WR24_4:	
		CLR	COMBIT		;写入数据失败
		LCALL	STOP
		LJMP	WR24_6
	WR24_5:	
		;SETB	T2CON.2		;启动5MS定时器
		LCALL	STOP
	WR24_6:	
		MOV	C,COMBIT
		POP	ACC
		RET			;写入数据程序结束返回
;===========================================================
RSEG	?PR?_rd24C08_single?ASM
_RD24C08_single:	;随机读单字节数据
		PUSH	ACC
		MOV	A,R7;first arg, eg. high address of buffer
		MOV	R0,A
		SETB	COMBIT
		LCALL	START00
		MOV	A,r4; second args, eeprom low address
		RL	A
		ANL	A,#00000110B	; 
		ORL	A,#0A0H		;确定高地址和写命令
		LCALL	ARL_SDA		;写入高地址
		LCALL	INACK
		JB	F0,RD24_1
		LJMP	RD24_3
	RD24_1:	
		MOV	A,r5
		LCALL	ARL_SDA		;写入低地址
		LCALL	INACK
		JB	F0,RD24_2
		LJMP	RD24_3
	RD24_2:	
		LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A1H		;确定高地址和读命令
		LCALL	ARL_SDA
		LCALL	INACK
		JNB	F0,RD24_3
		LCALL	SDA_ARL		;读取数据
		MOV	@R0,A
		LJMP	RD24_4
	RD24_3:	
		CLR	COMBIT		;读取数据出错
	RD24_4:	
		LCALL	STOP
	RD24_5:	
		MOV	C,COMBIT
		POP	ACC
		RET			;读取数据程序结束返回
;===========================================================
SDA_ARL:	;读数据子程序
		MOV	BitCnt,#8
	SDA_A1:	
		SETB	IIC_CK
		LCALL	DELAY5US
		MOV	C,COMSDA
 		RLC	A
		CLR	IIC_CK
		LCALL	DELAY5US
		DJNZ	BitCnt,SDA_A1
		RET
		
ARL_SDA:	;发送数据子程序
		MOV	BitCnt,#8
	ARL_S1:	RLC	A
		MOV	COMSDA,C
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	IIC_CK
		LCALL	DELAY5US
		DJNZ	BitCnt,ARL_S1
		RET		
		
INACK:		;发送机接收接受机应答信号
		SETB	COMSDA		
		SETB	IIC_CK
		LCALL	DELAY5US
		SETB	F0
		JNB	COMSDA,CEND	
		CLR	F0
	CEND:		
		CLR	IIC_CK
		LCALL	DELAY5US
		RET	
		
MRDINACK:	;接收机发送应答给发送机
		CLR	COMSDA
		LCALL	DELAY5US
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	IIC_CK
		LCALL	DELAY5US
		SETB	COMSDA
		RET		
		
START00:	;开始信号
		SETB	COMSDA
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	COMSDA
		LCALL	DELAY5US
		CLR	IIC_CK
                LCALL	DELAY5US
		RET

STOP:		;结束信号
		CLR	COMSDA
		LCALL	DELAY5US	
		SETB	IIC_CK
		LCALL	DELAY5US
		SETB	COMSDA
		LCALL	DELAY5US
		RET


RSEG	?PR?_wr24C08_multi?ASM	
_WR24C08_multi:	;写数据多字节  如果起始地址和要写入的数据长度出现在页临界时会出错
		;DPH和DPL中的数据决定写入的起始地址
		;R0中的数据决定从MCU的RAM中的读出的数据首址
		;ByteCnt决定一次写入的数据字节数   但是最多不能超过16个字节且必须起始地址为X0地址
		PUSH	ACC
		MOV	A,R0
		PUSH	ACC
		PUSH	B
		MOV	B,R3
		mov	a,r7
		MOV	R0,a
		SETB	COMBIT
	        LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A0H		;确定高地址和写命令
		LCALL	ARL_SDA		;写入高地址
		LCALL	INACK
		JB	F0,WR24_1M
		LJMP	WR24_4M
	WR24_1M:	
		MOV	A,r5
		LCALL	ARL_SDA		;写入低地址
		LCALL	INACK
		JB	F0,WR24_2M
		LJMP	WR24_4M
	WR24_2M:	
		MOV	A,@R0
		LCALL	ARL_SDA		;写入数据
		LCALL	INACK
		JNB	F0,WR24_5M
		INC	R0
		DJNZ	B,WR24_2M
		LJMP	WR24_5M
	WR24_4M:	
		CLR	COMBIT		;写入数据失败
		LCALL	STOP
		LJMP	WR24_6M
	WR24_5M:	
		;SETB	T2CON.2		;启动5MS定时器
		LCALL	STOP
	WR24_6M:
		MOV	C,COMBIT
		POP	B
		POP	ACC
		MOV	R0,A
		POP	ACC
		RET			;写入数据程序结束返回
;===========================================================
RSEG	?PR?_rd24C08_multi?ASM	
_RD24C08_multi:	;读数据多字节		
		;DPH和DPL中的数据决定从IIC读出的起始地址
		;R0中的数据决定从EEPROM中的读出的数据放在MCU的RAM中的首址
		;ByteCnt决定一次读出的数据字节数  读出的数据没有长度限制  而且具有跨页的读取功能
		PUSH	ACC
		MOV	A,R0
		PUSH	ACC
		PUSH	B
		MOV	B,R3
		mov	a,r7
		MOV	R0,a
		;JB	IICBUSY,$+8
		;CLR	COMBIT
		;LJMP	RD24_6M
		SETB	COMBIT
	        LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A0H		;确定高地址和写命令
		LCALL	ARL_SDA		;写入高地址
		LCALL	INACK
		JB	F0,RD24_1M
		LJMP	RD24_4M
	RD24_1M:	
		MOV	A,r5
		LCALL	ARL_SDA		;写入低地址
		LCALL	INACK
		JB	F0,RD24_2M
		LJMP	RD24_4M
	RD24_2M:	
		LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A1H		;确定高地址和读命令
		LCALL	ARL_SDA
		LCALL	INACK
		JNB	F0,RD24_4M
	RD24_3M:	
		;DEC	ByteCnt
		DJNZ	B,RD24_3M1
		SJMP	RD24_3M2
	RD24_3M1:	
		LCALL	SDA_ARL		;读取数据
		LCALL	MRDINACK	;发送应答信号给IIC
		MOV	@R0,A
		INC	R0
		DJNZ	B,RD24_3M1
	RD24_3M2:
		LCALL	SDA_ARL		;读取数据
		MOV	@R0,A
		LJMP	RD24_5M
	RD24_4M:	
		CLR	COMBIT		;读取数据失败
	RD24_5M:	
		LCALL	STOP
	RD24_6M:	
		MOV	C,COMBIT
		POP	B
		POP	ACC
		MOV	R0,A
		POP	ACC
		RET			;读取数据程序结束返回

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


