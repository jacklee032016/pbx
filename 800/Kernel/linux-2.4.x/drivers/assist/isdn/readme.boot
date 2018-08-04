# $Id: readme.boot,v 1.1.1.1 2006/11/30 16:27:58 lizhijie Exp $		
		ISDN驱动协议栈的配置
		            李志杰  2005.01.18


ISDN驱动的协议栈可以用动态和静态两种方式进行配置

可以配置的内容
    每个ISDN HFC芯片上可以使用的端口数目
        端口数可以小于4；小于4时，端口的使用从第一个端口（0）开始
    每个端口上使用的协议栈类型
        只配置NT和TE模式，其余的协议栈内容不容许配置


ISDN驱动使用协议栈可以用两种方式来配置
1、编译时
    在make menuconfig的菜单中选择需要配置的端口数目和每个端口上的协议类型

2/、加载时
    在加载最后的模块（assist_isdn_hfc.o）时，可以指定参数来配置驱动运行时
    使用的协议栈。
    
    参数有port_used和port_protocol两个，具体的使用为
    insmod assist_isdn_hfc.o port_used=0x4/3/2/1 port_protocol=0xX
    
    port_used参数：指定使用的端口数目
    port_protocol参数：指定每个端口使用的协议栈类型，方法为Bit0/1/2/3分别
    对应端口1/2/3/4；当该位为1时，该端口的协议为NT类型，否则为TE类型。
		            