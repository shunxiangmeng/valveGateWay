#ifndef _com_h
#define _com_h

int com_init(void);


typedef struct 
{
	char cmd[16];
	unsigned int uid[8];
	unsigned int data[16];
	unsigned int status;
}COM_RX_DATA;


#endif

