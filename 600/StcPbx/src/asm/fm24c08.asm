
?PR?DELAY5US?ASM			SEGMENT	CODE

PUBLIC	DELAY5US

RSEG	?PR?_WR24C08_single?ASM
_WR24C08_single:	;д���ݵ��ֽ�
		PUSH	ACC
		SETB	COMBIT
	        LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A0H		;ȷ���ߵ�ַ��д����
		LCALL	ARL_SDA		;д��ߵ�ַ
		LCALL	INACK
		JB	F0,WR24_1
		LJMP	WR24_4
	WR24_1:	
		MOV	A,r5
		LCALL	ARL_SDA		;д��͵�ַ
		LCALL	INACK
		JB	F0,WR24_2
		LJMP	WR24_4
	WR24_2:	
		MOV	A,r7	;д������
		LCALL	ARL_SDA
		LCALL	INACK
		JB	F0,WR24_5			
	WR24_4:	
		CLR	COMBIT		;д������ʧ��
		LCALL	STOP
		LJMP	WR24_6
	WR24_5:	
		;SETB	T2CON.2		;����5MS��ʱ��
		LCALL	STOP
	WR24_6:	
		MOV	C,COMBIT
		POP	ACC
		RET			;д�����ݳ����������
;===========================================================
RSEG	?PR?_rd24C08_single?ASM
_RD24C08_single:	;��������ֽ�����
		PUSH	ACC
		MOV	A,R7;first arg, eg. high address of buffer
		MOV	R0,A
		SETB	COMBIT
		LCALL	START00
		MOV	A,r4; second args, eeprom low address
		RL	A
		ANL	A,#00000110B	; 
		ORL	A,#0A0H		;ȷ���ߵ�ַ��д����
		LCALL	ARL_SDA		;д��ߵ�ַ
		LCALL	INACK
		JB	F0,RD24_1
		LJMP	RD24_3
	RD24_1:	
		MOV	A,r5
		LCALL	ARL_SDA		;д��͵�ַ
		LCALL	INACK
		JB	F0,RD24_2
		LJMP	RD24_3
	RD24_2:	
		LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A1H		;ȷ���ߵ�ַ�Ͷ�����
		LCALL	ARL_SDA
		LCALL	INACK
		JNB	F0,RD24_3
		LCALL	SDA_ARL		;��ȡ����
		MOV	@R0,A
		LJMP	RD24_4
	RD24_3:	
		CLR	COMBIT		;��ȡ���ݳ���
	RD24_4:	
		LCALL	STOP
	RD24_5:	
		MOV	C,COMBIT
		POP	ACC
		RET			;��ȡ���ݳ����������
;===========================================================
SDA_ARL:	;�������ӳ���
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
		
ARL_SDA:	;���������ӳ���
		MOV	BitCnt,#8
	ARL_S1:	RLC	A
		MOV	COMSDA,C
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	IIC_CK
		LCALL	DELAY5US
		DJNZ	BitCnt,ARL_S1
		RET		
		
INACK:		;���ͻ����ս��ܻ�Ӧ���ź�
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
		
MRDINACK:	;���ջ�����Ӧ������ͻ�
		CLR	COMSDA
		LCALL	DELAY5US
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	IIC_CK
		LCALL	DELAY5US
		SETB	COMSDA
		RET		
		
START00:	;��ʼ�ź�
		SETB	COMSDA
		SETB	IIC_CK
		LCALL	DELAY5US
		CLR	COMSDA
		LCALL	DELAY5US
		CLR	IIC_CK
                LCALL	DELAY5US
		RET

STOP:		;�����ź�
		CLR	COMSDA
		LCALL	DELAY5US	
		SETB	IIC_CK
		LCALL	DELAY5US
		SETB	COMSDA
		LCALL	DELAY5US
		RET


RSEG	?PR?_wr24C08_multi?ASM	
_WR24C08_multi:	;д���ݶ��ֽ�  �����ʼ��ַ��Ҫд������ݳ��ȳ�����ҳ�ٽ�ʱ�����
		;DPH��DPL�е����ݾ���д�����ʼ��ַ
		;R0�е����ݾ�����MCU��RAM�еĶ�����������ַ
		;ByteCnt����һ��д��������ֽ���   ������಻�ܳ���16���ֽ��ұ�����ʼ��ַΪX0��ַ
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
		ORL	A,#0A0H		;ȷ���ߵ�ַ��д����
		LCALL	ARL_SDA		;д��ߵ�ַ
		LCALL	INACK
		JB	F0,WR24_1M
		LJMP	WR24_4M
	WR24_1M:	
		MOV	A,r5
		LCALL	ARL_SDA		;д��͵�ַ
		LCALL	INACK
		JB	F0,WR24_2M
		LJMP	WR24_4M
	WR24_2M:	
		MOV	A,@R0
		LCALL	ARL_SDA		;д������
		LCALL	INACK
		JNB	F0,WR24_5M
		INC	R0
		DJNZ	B,WR24_2M
		LJMP	WR24_5M
	WR24_4M:	
		CLR	COMBIT		;д������ʧ��
		LCALL	STOP
		LJMP	WR24_6M
	WR24_5M:	
		;SETB	T2CON.2		;����5MS��ʱ��
		LCALL	STOP
	WR24_6M:
		MOV	C,COMBIT
		POP	B
		POP	ACC
		MOV	R0,A
		POP	ACC
		RET			;д�����ݳ����������
;===========================================================
RSEG	?PR?_rd24C08_multi?ASM	
_RD24C08_multi:	;�����ݶ��ֽ�		
		;DPH��DPL�е����ݾ�����IIC��������ʼ��ַ
		;R0�е����ݾ�����EEPROM�еĶ��������ݷ���MCU��RAM�е���ַ
		;ByteCnt����һ�ζ����������ֽ���  ����������û�г�������  ���Ҿ��п�ҳ�Ķ�ȡ����
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
		ORL	A,#0A0H		;ȷ���ߵ�ַ��д����
		LCALL	ARL_SDA		;д��ߵ�ַ
		LCALL	INACK
		JB	F0,RD24_1M
		LJMP	RD24_4M
	RD24_1M:	
		MOV	A,r5
		LCALL	ARL_SDA		;д��͵�ַ
		LCALL	INACK
		JB	F0,RD24_2M
		LJMP	RD24_4M
	RD24_2M:	
		LCALL	START00
		MOV	A,r4
		RL	A
		ANL	A,#00000110B	
		ORL	A,#0A1H		;ȷ���ߵ�ַ�Ͷ�����
		LCALL	ARL_SDA
		LCALL	INACK
		JNB	F0,RD24_4M
	RD24_3M:	
		;DEC	ByteCnt
		DJNZ	B,RD24_3M1
		SJMP	RD24_3M2
	RD24_3M1:	
		LCALL	SDA_ARL		;��ȡ����
		LCALL	MRDINACK	;����Ӧ���źŸ�IIC
		MOV	@R0,A
		INC	R0
		DJNZ	B,RD24_3M1
	RD24_3M2:
		LCALL	SDA_ARL		;��ȡ����
		MOV	@R0,A
		LJMP	RD24_5M
	RD24_4M:	
		CLR	COMBIT		;��ȡ����ʧ��
	RD24_5M:	
		LCALL	STOP
	RD24_6M:	
		MOV	C,COMBIT
		POP	B
		POP	ACC
		MOV	R0,A
		POP	ACC
		RET			;��ȡ���ݳ����������

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


