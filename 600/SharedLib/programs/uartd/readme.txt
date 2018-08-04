# $Id: readme.txt,v 1.3 2007/03/24 21:55:30 lizhijie Exp $
	Readme for UARTD

2007.03.24
    重新规划uartd程序框架
        消息队列采用三个：分别为uartd、cgipbx、sip使用
        消息队列的发送和接收均采用阻塞模式
            接收消息一定采用阻塞模式
            发送消息，也采用阻塞模式，因为消息队列中可以使用的消息数目远
            大于发送需要的上限，所以实际根本不可能发生阻塞
        串口收发
            串口接收，采用阻塞模式
            串口发生 也采用阻塞形式，因为报文非常小，所以原理上不可能阻塞
        均采用阻塞的方式是为了保证消息的容错性
        
        实际程序最终阻塞可能发生的地方是串口的读操作和消息队列的读操作，
        这是合理的程序设计
        
        
        串口接收数据
            采用的buffer大小为16字节，可能最多会收回两个消息，但是也可能
            出现一个消息被拆分成多次读结果的问题，必须使用程序处理。
        
        全部程序使用uartd与模拟PBX系统通信
            这些程序均使用消息队列和uartd通信
            程序包括：
                sip ua
                cgi pbx
                录音程序
                自检程序

一.编译
	1.执行命令make ARCH=arm生成IXP422版本,make ARCH=arm install 生成*.tar发布包
	2.执行命令make 生成PC版本,make install生成*.tar发布包
	3.执行命令make LINK=staic ARCH=arm 生成静态链连的程序
	4.执行命令make DEBUG=y ARCH=arm 生成可用于显示调试信息的程序                    
二.生成配置文件
	运行serials目录下mkdev_uart.sh脚本程序即生成所需配置文件
	
三.运行程序
	1.运行在调试模式
		#uartd  -d
	2.运行在工作模式
		#uartd  (守护进程)
	3.帮助
		#uartd -h
四.测试程序的运行：
     在test目录下有两个用于串口收发数据的测试工具，分别是基于PC和ARM版本，PC
     机版的是uartd_test_pc,ARM版的是uartd_test_pc,只需进入test目录后，运行
     ./uartd_test_pc就可以在PC机上测试串口收发数据是否正常