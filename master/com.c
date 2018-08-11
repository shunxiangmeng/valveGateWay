#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "com.h"
#include "cJSON.h"
#include "cloud.h"
#include "print.h"

extern SYSTEM_INFO  g_systemInfo;  //当前系统信息
extern SEND_DATA  g_sendData;   //发送数据

void* com_pthread(void *arg);

int com_init(void)
{

	int fd = uart_init(2);
	if (fd < 0)
	{
		printf("open uart error\r\n");
	}
	else
	{
		printf("open uart ok, fd:%d\r\n", fd);
	}

	pthread_t pthread_task;
	int ret = pthread_create(&pthread_task, NULL, (void*)com_pthread, (void*)fd);
	if (ret)
	{
		close(fd);
		printf("create cloud thread error\r\n");
		return -2;
	}


}


void* com_pthread(void *arg)
{
	int fd = (int)arg;
	int i;
	int ret = 0;
	char buf_temp[16];
	char buf[128];
	int rxCount = 0;

	COM_RX_DATA rxData;



	while(1)
	{
		usleep(1000);
		if (rxCount == 0)
		{
			memset(buf, 0, sizeof(buf));
		}
		memset(buf_temp, 0, sizeof(buf_temp));

		ret = read(fd, buf_temp, sizeof(buf_temp));
		if (ret <= 0)
		{
			continue;
		}

		#if 0
		INFO("ret %d: ", ret);
		for (i = 0; i < ret; i++)
		{
			printf("%02X ", buf_temp[i]);
			if (i == 32)
			{
				printf("\r\n");
			}
		}
		printf("\r\n");
		#endif

		memcpy(&buf[rxCount], buf_temp, ret);
		rxCount += ret;

		if (ret < sizeof(buf_temp) || buf_temp[ret-1] == '\n')//接收完成
		{
			WARN("rxCount:%d \n", rxCount);
			printf("%s", buf);

			cJSON * item = NULL;//cjson对象

			cJSON *root = cJSON_Parse(buf); 
			if (!root)
			{
				printf("Error before: [%s]\n",cJSON_GetErrorPtr());
			}
			else
			{
				memset((char*)&rxData, 0, sizeof(rxData));

				item = cJSON_GetObjectItem(root, "cmd");
				memcpy(rxData.cmd, item->valuestring, strlen(item->valuestring));
				printf("cmd: %s\n", item->valuestring);

				item = cJSON_GetObjectItem(root, "uid");
				memcpy(rxData.uid , item->valuestring, sizeof(rxData.uid));
				printf("uid: %s\n", rxData.uid);

				item = cJSON_GetObjectItem(root, "data");
				memcpy(rxData.data , item->valuestring, sizeof(rxData.data) < strlen(item->valuestring)?sizeof(rxData.data):strlen(item->valuestring));
				printf("data: %s\n", rxData.data);

				item = cJSON_GetObjectItem(root, "status");
				rxData.status = item->valueint;
				printf("status: %u\n", rxData.status);


				if (memcmp("getCard", rxData.cmd, sizeof("getCard")) == 0)
				{
					//memcpy(g_systemInfo.uid, rxData.uid, sizeof(rxData.uid));

					memcpy(g_sendData.bottleId, rxData.data, 10);
					memcpy(g_sendData.TID, rxData.uid, 8);
					//memcpy(g_systemInfo.staffId, rxData.data, 6);

					cloud_sendMessage(1);
				}
				else if (memcmp("lostCard", rxData.cmd, sizeof("lostCard")) == 0)
				{
					//memcpy(g_systemInfo.uid, rxData.uid, sizeof(rxData.uid));

					memcpy(g_sendData.bottleId, rxData.data, 10);
					memcpy(g_sendData.TID, rxData.uid, 8);

					//memcpy(g_systemInfo.staffId, rxData.data, 6);

					cloud_sendMessage(0);
				}
				else if (memcmp("status", rxData.cmd, sizeof("status")) == 0)
				{
					g_systemInfo.isGunReady = rxData.status;
					cloud_sendMessage(0);
				}

			}

			rxCount = 0;
		}

	}

}

