
#include <stdio.h>
#include <string.h>

#include "binimagearray.h"

static uint8_t bia_coordmap_s2a(int s_i,int s_j,int coord_a[2],bia_t* bia)
{
	uint8_t mode = bia->arrange_mode;
	uint16_t width = bia->width;
	uint16_t height = bia->height;
	int a_i,a_j;

	if(s_i >= width || s_j >= height)
		return 0;

	switch(mode)
	{
		case BIA_ARRANGE_BHbH:
			a_i = (width/8) * s_j + s_i / 8;
			a_j = s_i % 8;
			break;
		case BIA_ARRANGE_BVbV:
			a_i = (height/8)*s_i + s_j / 8;
			a_j = s_j % 8;
			break;
		case BIA_ARRANGE_BVbH:
			a_i = (s_i/8)*height + s_j;
			a_j = s_i % 8;
			break;
		case BIA_ARRANGE_BHbV:
			a_i = (s_j/ 8)*width + s_i;
			a_j = s_j % 8;
			break;
		default:
			return 0;
		
	}

	coord_a[0] = a_i;
	coord_a[1] = a_j;

	return 1;
}


void bia_setpixel(int s_i,int s_j,uint8_t* buffer,bia_t* bia)
{
	uint16_t width = bia->width;
	uint16_t height = bia->height;
	int a[2];

	if(s_i >= width || s_j >= height)
		return;
	if(!bia_coordmap_s2a(s_i,s_j,a,bia))
		return;
	
	//fprintf(stderr,"set a[%dx%d]\n",a[0],a[1]);
	if(bia->msb)
	{
		buffer[a[0]] |= (0x80)>>a[1];
	}
	else
	{
		buffer[a[0]] |= (0x01)<<a[1];
	}
}

uint8_t bia_getpixel(int s_i,int s_j,uint8_t* buffer,bia_t* bia)
{
	uint16_t width = bia->width;
	uint16_t height = bia->height;
	int a[2];

	if(s_i >= width || s_j >= height)
		return 0;
	
	if(!bia_coordmap_s2a(s_i,s_j,a,bia))
		return 0;
	

	if(bia->msb)
	{
		return (buffer[a[0]] & ((0x80)>>a[1]));
	}
	else
	{
		return (buffer[a[0]] & ((0x01)<<a[1]));
	}
}



