#pragma once

typedef struct _stackblur_mt_info
{
	unsigned char* src;
	unsigned int w;
	unsigned int h;
	unsigned int radius;
	unsigned int stride;
	int cores;
	unsigned int div;
	unsigned char* stack;
}_stackblur_mt_info;


extern "C" void stackblur(unsigned char* src, unsigned int w, unsigned int h, unsigned int stride, unsigned int radius);

extern "C" _stackblur_mt_info* stackblur_mt_begin(unsigned char* src, unsigned int w, unsigned int h, unsigned int stride, unsigned int radius, int cores = 1);
extern "C" void stackblur_mt(_stackblur_mt_info* info, int core, int step);
extern "C" void stackblur_mt_end(_stackblur_mt_info* info);
