#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>     // for socket
#include <netinet/in.h>    // for sockaddr_in
#include <sys/socket.h>    // for socket
#include <fcntl.h>
#include <pthread.h>
#include "cloud.h"
#include "print.h"

#define SERVER_PORT			8888
#define SERVER_IP			"119.23.56.166"

static int g_socketFd = -1;

SYSTEM_INFO     g_systemInfo;  //当前系统信息

SEND_DATA g_sendData;   //发送数据

//初始化与服务器的连接
//返回socket_fd
static int socket_init(void)
{
	//设置一个socket地址结构server_addr,代表服务器internet地址, 端口
	struct sockaddr_in client_addr;

	bzero(&client_addr, sizeof(client_addr)); //把一段内存区的内容全部设置为0

	client_addr.sin_family = AF_INET;    //internet协议族
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
    client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口

    //创建用于internet的流协议(TCP)socket,用client_socket代表客户机socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);  //tcp
    if( client_socket < 0)
    {
        printf("Create Socket Failed!\r\n");
        return -1;
	}

	//把客户机的socket和客户机的socket地址结构联系起来
    if( bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n"); 
        return -2;
	}

	//设置一个socket地址结构server_addr,代表服务器的internet地址, 端口
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_aton(SERVER_IP, &server_addr.sin_addr) == 0) //服务器的IP地址
    {
        printf("Server IP Address Error!\n");
        return -3;
    }

    server_addr.sin_port = htons(SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);
    //向服务器发起连接,连接成功后client_socket代表了客户机和服务器的一个socket连接
    if(connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", SERVER_IP);
        return -4;
    }
    else
    {
    	INFO("Connected to: %s\n", SERVER_IP);
    }

    g_socketFd = client_socket;
    return client_socket;	
}


void* cloud_pthread(void *arg);
//
int cloud_init(void)
{
	int fd = socket_init();
	if (fd < 0)
	{
		printf("socket init error\r\n");
		return -1;
	}

	pthread_t pthread_task;
	int ret = pthread_create(&pthread_task, NULL, (void*)cloud_pthread, (void*)fd);
	if (ret)
	{
		close(fd);
		printf("create cloud thread error\r\n");
		return -2;
	}

	return fd;
}

//计算CRC
uint16_t get_crc16(unsigned char* pBuffer, int Length) 
{ 
	int j; 
	uint16_t wCRC = 0xFFFF; 
 
	for( ; Length > 0; Length-- ) 
	{ 
		wCRC ^= *pBuffer++; 
		for (j = 0;j < 8;j++) 
		{ 
			if (wCRC & 0x0001) 
			{ 
				wCRC = (wCRC >> 1) ^ 0xA001; 
			} 
			else 
			{ 
				wCRC = wCRC >> 1; 
			} 
		} 
	} 
 	return wCRC; 
} 

//发送数据
int cloud_send(int fd)
{
	unsigned char buf[128] = {0x00};
	char temp[20] = {0};

	buf[0] = 0x97; //帧头
	buf[1] = 0xD0; //控制板告诉服务器读到标签数据
	buf[2] = 0x2C; //len_L  38 = 7+1+1+1+6+6+10+12
	buf[3] = 0;    //len_H

	memcpy(&buf[4], "123458", 6);  //stationId
	buf[10] = 0x03; //2号枪
	buf[11] = 0x01; //有人物ID
	buf[12] = 0x01; //读到芯片
	buf[13] = 0x00; //光电开关状态(0x01为把阀门压紧)

	memcpy(&buf[14], "000987", 6);         //6位芯片客户编码, 员工ID
	memset(&buf[20], 0x00, 6);             //钢瓶归属ID
	memcpy(&buf[26], "1234567890", 10);    //10钢瓶ID
	memcpy(&buf[36], "123456789012", 12);  //12位TID

	unsigned int crc = get_crc16(buf, 48);

	//printf("crc: 0x%x\r\n", crc);

	buf[48] = (char)crc;
	buf[49] = (char)(crc >> 8);

	int i = 0;
	for (i = 0; i < 50; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");

	int ret = send(g_socketFd, buf, 50, 0);
	printf("send ret:%d\r\n", ret);

	return 0;
}



//接收云数据处理线程函数
void* cloud_pthread(void *arg)
{
	int socket = (int)arg;
	int len;
	int i = 0;
	char data[128];
	char temp[32];

	RECV_DATA recvData;

	while(1)
	{
		usleep(5000);
		len = recv(socket, data, sizeof(data), 0);
		if (len > 0)
		{
			INFO("recv data from server len: %d\n", len);
			for (i = 0; i < len; i++)
			{
				printf("%02X ", (unsigned char)data[i]);
			}
			printf("\n");


			if (len >= sizeof(RECV_DATA))
			{
				memcpy((char*)&recvData, data, sizeof(recvData));

				memset(temp, 0, sizeof(temp));
				memcpy(temp, recvData.stationId, 6);

				printf("stationId: %s\n", temp);
				printf("gunId: 0x%02x\n", recvData.gunId);
				printf("hasPeople: %d\n", recvData.hasPeople);
				printf("isReadCard: %d\n", recvData.isReadCard);
				printf("isGunReady: %d\n", recvData.isGunReady);
				printf("TID: %s\n", recvData.TID);
				printf("data: %s\n", recvData.bottleId);  //data
				printf("valveStatus: %d\n", recvData.isOpenValve);
				printf("ledStatus: %d\n", recvData.ledStatus);
				printf("\n");
			}
		

		}
	}
	close(socket);

}


int cloud_sendMessage(unsigned char isCard)
{
	
	unsigned char buf[128] = {0x00};

	//memset((void*)&g_sendData, 0x00, sizeof(sendData));

	g_sendData.head = 0x97;
	g_sendData.cmd = 0xD0;
	g_sendData.len_l = 44;
	g_sendData.len_h = 0x00;

	memcpy(g_sendData.stationId, g_systemInfo.stationId, sizeof(g_sendData.stationId));
	g_sendData.gunId = g_systemInfo.gunId;

	if (memcmp(g_systemInfo.staffId, "000000", 6) == 0)
	{
		g_sendData.hasPeople = 0;
	}
	else
	{
		g_sendData.hasPeople = 1;
	}

	g_sendData.isReadCard = isCard;
	g_sendData.isGunReady = g_systemInfo.isGunReady;

	memcpy(g_sendData.staffId, g_systemInfo.staffId, 6);

	//memcpy(g_sendData.TID, g_systemInfo.uid, 8);

	unsigned int crc = get_crc16((unsigned char*)&g_sendData, sizeof(g_sendData)-2);

	g_sendData.crc_l = (char)crc;
	g_sendData.crc_h = (char)(crc >> 8);

	memcpy(buf, (char*)&g_sendData, sizeof(g_sendData));

	INFO("send data to server\n");
	int ret = send(g_socketFd, buf, sizeof(g_sendData), 0);

	int i = 0;
	for (i = 0; i < 50; i++)
	{
		printf("%02X ", buf[i]);
	}
	printf("\r\n");

	return ret;
}

//------------------------------------------------------------------------
int out_initGpio(int n)
{
	char str[16] = {0};
	int fp = open("/sys/class/gpio/export", O_WRONLY);  
    if (fp < 0)  
    {
        printf("export open filed\n"); 
        return -1; 
    }
    else  
    {
    	sprintf(str, "%d", n);
        write(fp, str, strlen(str));  
    }
    close(fp); 
}

void out_setGpioDirection(int n, char *direction)
{
	char path[100] = {0};  
	char str[16] = {0};
    sprintf(path, "/sys/class/gpio/gpio%d/direction", n);  
    int fp = open(path, O_WRONLY);  
    if (fp < 0)  
    {
    	printf("direction open filed\n");  
    	return ;
    }
    else  
    {
    	sprintf(str, "%s", direction);
        write(fp, str, strlen(str)); 
    }
    close(fp); 
}


void out_setGpioValve(int n, int valve)
{
	char path[100] = {0};  
	char str[16] = {0};
    sprintf(path, "/sys/class/gpio/gpio%d/valve", n);  
    int fp = open(path, O_WRONLY);  
    if (fp < 0)  
    {
    	printf("valve open filed\n");  
    	return ;
    }
    else  
    {
    	sprintf(str, "%d", valve);
        write(fp, str, strlen(str)); 
    }
    close(fp); 
}


int out_init(void)
{
	out_initGpio(OUT_PIN);
	out_setGpioDirection(OUT_PIN, "out");
	out_setGpioValve(OUT_PIN, 0);
}

