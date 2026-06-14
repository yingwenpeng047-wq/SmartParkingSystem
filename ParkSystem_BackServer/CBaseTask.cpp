#include "CBaseTask.h"

unsigned int CBaseTask::crc_table[256] = { 0 };

CBaseTask::CBaseTask()
{

}


CBaseTask::~CBaseTask()
{

}


void CBaseTask::init_crc_table(void)
{
	for (unsigned int i = 0; i < 256; ++i) {
		unsigned int c = i;
		for (int j = 0; j < 8; ++j)
			c = (c & 1) ? (0xedb88320L ^ (c >> 1)) : (c >> 1);
		crc_table[i] = c;
	}
}
unsigned int CBaseTask::crc32(unsigned int crc, unsigned char* buffer, unsigned int size)
{
	for (unsigned int i = 0; i < size; ++i)
		crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
	return crc ^ 0xFFFFFFFF;
}