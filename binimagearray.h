#ifndef _BIN_IMAGE_ARRAY_H_
#define _BIN_IMAGE_ARRAY_H_


#include <stdint.h>

#define BIA_HEAD_ID 0x8a9ce17b

//水平扫描
#define BIA_ARRANGE_BHbH	0
//垂直扫描
#define BIA_ARRANGE_BVbV	1
//字节垂直扫描，bit水平扫描
#define BIA_ARRANGE_BVbH	2
//字节水平扫描，bit垂直扫描
#define BIA_ARRANGE_BHbV	3

#define BIA_ARRANGE_MAX		4

#pragma pack(push, 1)
typedef struct
{
	//文件类型校验ID
	uint32_t id;
	//bit排列方式
	uint8_t arrange_mode;
	//高位优先
	uint8_t msb;
	
	uint16_t width;
	uint16_t height;
	//帧数
	uint16_t number;
	//帧间延时
	uint16_t delay;
	//数据总大小
	uint32_t size;
}bia_t;
#pragma pack(pop)


void bia_setpixel(int x,int y,uint8_t* frame,bia_t* bia);
uint8_t bia_getpixel(int x,int y,uint8_t* frame,bia_t* bia);



#endif


