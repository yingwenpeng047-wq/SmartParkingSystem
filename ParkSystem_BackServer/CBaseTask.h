#pragma once
#include "WriteMemShare.h"
#include <arpa/inet.h>


//任务基类
class CBaseTask
{
public:
	CBaseTask();
	virtual ~CBaseTask();

	virtual void work() = 0;
	static void init_crc_table(void);
	static unsigned int crc32(unsigned int crc, unsigned char* buffer, unsigned int size);

protected:
	static unsigned int crc_table[256];
};

