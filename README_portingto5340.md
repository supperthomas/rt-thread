1.安装 Env ，

[下载 Env 工具](https://www.rt-thread.org/page/download.html)

[Env 用户手册](https://github.com/RT-Thread/rtthread-manual-doc/blob/master/env/env.md)
2.下载代码 
3.安装 新编译器
xpack-windows-build-tools-4.2.1-2-win32-x64.zip (Make命令）
gcc-arm-none-eabi-10.3-2021.07-win32.zip   （GCC工具链）
4.打开Env 命令行 cd进入编译目录
example:
andrewli@DESKTOP-0J284J3 C:\Work\gitee_projects\rt_thread_porting_to_nrf5340\bsp\nrf5x\nrf5340
>
5.设置编译器路径
example:
>set RTT_CC=gcc
> set RTT_EXEC_PATH=C:\Work\RTthread\gcc-arm-none-eabi-10.3-2021.07\bin （需改成第三步新编译器的安装路径）
6.配置编译代码
配置可以用
>menuconfig
也可以直接编译
>scons

可以用jflash 直接烧 rtthread.bin
**或者直接双击flash.bat进行烧写**

7.gcc调试需下载安装ozone 加载 rtthread.elf，**当然也可以使用下面的Keil进行调试**

8.
LED test 在 rt_thread_porting_to_nrf5340\bsp\nrf5x\nrf5340\applications\application.c
串口有 输出和 cmd shell 


**Keil工程
**********

输入如下命令生成keil5工程
   scons --target=mdk5

然后打开
	project.uvprojx

即可编译和下载


**DFU： Keil工程配合MCUBoot
*************************

由于MCUBoot占用0x0000 ~ 0xC000地址，所以application的起始地址需要改成0xc200 (0x200是header)

Keil配置修改:
在Linker标签页中，选择“Use memory layout from target dialog”
再选择Targe标签页，把IROM1起始地址(Start)改为0xC200

然后编译，并得到rtthread.hex。 

把rtthread.hex拷贝到flash_script目录，该目录同时包含mcuboot.hex和网络核merged_CPUNET.hex。
注意：mcuboot.hex需要修改原始的mcuboot工程，修改点包括：
 1. 修改mcuboot config： CONFIG_MCUBOOT_CLEANUP_ARM_CORE=y CONFIG_BOOT_INTR_VEC_RELOC=y CONFIG_BOOT_SIGNATURE_TYPE_ECDSA_P256=y
 #. 修改mcuboot main.c文件，跳转之前打开中断：
    #if CONFIG_MCUBOOT_CLEANUP_ARM_CORE
    __set_CONTROL(0x00); /* application will configures core on its own */
    __ISB();
    __enable_irq();
    #endif

而merged_CPUNET.hex包含网络核应用zephyr.hex和网络核bootloader:b0n

点击里面的flash.bat，即可完成签名，image合并和下载。

程序运行成功后，4个LED会闪烁，通过串口助手输入help，可以看到帮助菜单，然后输入“ipc_sample"，这个命令将打开网络核，此时网络核广播为”nrf_dfu_ipc"，
并且打开RTT viewer查看网络核日志，网络核会显示日志:
00> [00:14:00.039,581] <inf> main: netcore main
00> [00:14:01.349,121] <inf> ipc_api: IPC test:
00>                                   49 20 61 6d 20 66 72 6f  6d 20 41 50 50 20 a8    |I am fro m APP . 
00> [00:14:05.039,733] <inf> main: netcore main

