#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "print.h"

int uart_init(int uart_num)
{
	int fd, ret;

	char path[32] = {0};

	sprintf(path, "/dev/ttyS%d", uart_num);
	printf("path %s\r\n", path);

	fd = open(path, O_RDWR|O_NOCTTY|O_NDELAY);
	if (fd < 0)
	{
		printf("open uart error, fd :%d\r\n", fd);
		return -1;
	}

	tcflush(fd, TCIOFLUSH); 

	fcntl(fd, F_SETFL, 0); //重设为堵塞状态, 去掉O_NONBLOCK

	struct termios opts;
    tcgetattr(fd, &opts); //把原设置获取出来，存放在opts

    //INFO("opts.c_cflag:0x%X\n", opts.c_cflag);
    //INFO("opts.c_lflag:0x%X\n", opts.c_lflag);

    //设置波特率
    cfsetispeed(&opts, B38400);
    cfsetospeed(&opts, B38400);

    opts.c_cflag |= CLOCAL;   //忽略调试解调器状态行
    opts.c_cflag |= CREAD;    //启动接收器

    // 8N1
    opts.c_cflag &= ~CSIZE;
    opts.c_cflag &= ~PARENB;   //不启用校验
    opts.c_cflag &= ~CSTOPB;   //1个停止位
    opts.c_cflag |= CS8;       //8bit
    opts.c_cflag &= ~CRTSCTS;  //关闭硬件流控

    opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN); //raw input

    opts.c_iflag &= ~(BRKINT | ICRNL| INPCK | ISTRIP | IXON);
    
    opts.c_oflag &= ~OPOST;    //禁止所有输出处理

    opts.c_cc[VTIME] = 0;   //5秒中没有数据也返回
	opts.c_cc[VMIN] = 0; 


	//INFO("opts.c_cflag:0x%X\n", opts.c_cflag);
	//INFO("opts.c_lflag:0x%X\n", opts.c_lflag);
    if (tcsetattr(fd, TCSANOW, &opts) < 0)   //设置属性,更改立即发生
    {
    	ERROR("tcsetattr error\n");
    }

	return fd;
}



