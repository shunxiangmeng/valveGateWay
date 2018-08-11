#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "com.h"
#include "cloud.h"

extern SYSTEM_INFO  g_systemInfo;  //当前系统信息

int main(void)
{

	memset((void*)&g_systemInfo, 0x00, sizeof(g_systemInfo));
	memcpy(g_systemInfo.stationId, "123ABC", 6);
	g_systemInfo.gunId = 0xaa;
	g_systemInfo.gunStatus = 0;
	g_systemInfo.isGunReady = 0;
	memcpy(g_systemInfo.staffId, "000000", 6);


	com_init();
	out_init();

	int socket = cloud_init();
	printf("socket:%d\r\n", socket);

	//usleep(1000*100);
	//cloud_send(socket);

	usleep(100*1000);
	cloud_sendMessage(0);

	while(1)
	{
		usleep(10);


	}

	return 0;

}


