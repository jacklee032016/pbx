	Readme for UARTD


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