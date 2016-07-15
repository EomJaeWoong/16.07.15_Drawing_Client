#pragma once

#include "resource.h"

#define UM_SOCKET (WM_USER+1)

typedef struct st_Packet
{
	WORD wLength;
	int iStartX;
	int iStartY;
	int iEndX;
	int iEndY;
} Packet;