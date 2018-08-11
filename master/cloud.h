#ifndef _cloud_H
#define _cloud_H


#define OUT_PIN		14


int cloud_init(void);
int cloud_send(int fd);
int cloud_sendMessage(unsigned char isCard);

int out_init(void);
void out_setGpioValve(int n, int valve);


typedef struct 
{
	unsigned char head;  //0x97
	unsigned char cmd;
	unsigned char len_l;
	unsigned char len_h;
	unsigned char stationId[6];  //气站ID编码
	unsigned char gunId;         //气站第几号枪
	unsigned char hasPeople;     //是否有人物id
	unsigned char isReadCard;    //是否读到芯片 
	unsigned char isGunReady;    //光电开关状态(0x01为把阀门压紧)
	unsigned char staffId[6];    //人员编号
	unsigned char cardId1[6];    //钢瓶归属，暂时为0
	unsigned char bottleId[10];  //钢瓶ID
	unsigned char TID[12];       //TID
	unsigned char crc_l;
	unsigned char crc_h;
}SEND_DATA;

typedef struct 
{
	unsigned char head;  //0x79
	unsigned char cmd;
	unsigned char len_l;
	unsigned char len_h;
	unsigned char stationId[6]; //气站ID编码
	unsigned char gunId;        //气站第几号枪
	unsigned char hasPeople;    //是否有人物id
	unsigned char isReadCard;   //是否读到芯片 
	unsigned char isGunReady;   //光电开关状态(0x01为把阀门压紧)
	unsigned char staffId[6];
	unsigned char cardId1[6];
	unsigned char bottleId[10]; //钢瓶ID
	unsigned char TID[12];      //TID
	unsigned char isOpenValve;  //阀状态  0不允许开阀,1允许开阀(模式1),2允许开阀(模式2)等等
	unsigned char ledStatus;    //灯状态    0待机状态,1正常充气状态等等
	unsigned char crc_l;
	unsigned char crc_h;
}RECV_DATA;




typedef struct 
{
	unsigned char stationId[8]; //气站ID
	unsigned int  gunStatus;    //充气开关状态
	unsigned int  isGunReady;   //气枪就位状态
	unsigned int  gunId;        //枪的ID
	unsigned char staffId[8];   //上岗员工ID，字符串
	unsigned char bottleId[8];  //钢瓶ID，正在充气的钢瓶
	unsigned char uid[8];
}SYSTEM_INFO;  //当前系统信息

#endif

