; $Id: asm.asm,v 1.1 2007/07/30 07:11:38 lizhijie Exp $
; define BIT address in BIT memory space 


; name of current module
NAME	ASM					;定义模块名

; code segment
?PR?delay5ms?ASM			SEGMENT	CODE

?PR?DELAY5US?ASM			SEGMENT	CODE


; data segment(first 128 bytes)
;?DT?ASM					SEGMENT DATA
;RSEG	?DT?ASM	;relocatable segment


; BIT segment 

PUBLIC	delay5ms
PUBLIC	DELAY5US


;公开函数名以便C模块可调用他们
; select this relacatable(generic) segment, so counter used for this segment
		
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

