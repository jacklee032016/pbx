	Readme for UARTD


һ.����
	1.ִ������make ARCH=arm����IXP422�汾,make ARCH=arm install ����*.tar������
	2.ִ������make ����PC�汾,make install����*.tar������
	3.ִ������make LINK=staic ARCH=arm ���ɾ�̬�����ĳ���
	4.ִ������make DEBUG=y ARCH=arm ���ɿ�������ʾ������Ϣ�ĳ���                    
��.���������ļ�
	����serialsĿ¼��mkdev_uart.sh�ű������������������ļ�
	
��.���г���
	1.�����ڵ���ģʽ
		#uartd  -d
	2.�����ڹ���ģʽ
		#uartd  (�ػ�����)
	3.����
		#uartd -h
��.���Գ�������У�
     ��testĿ¼�����������ڴ����շ����ݵĲ��Թ��ߣ��ֱ��ǻ���PC��ARM�汾��PC
     �������uartd_test_pc,ARM�����uartd_test_pc,ֻ�����testĿ¼������
     ./uartd_test_pc�Ϳ�����PC���ϲ��Դ����շ������Ƿ�����