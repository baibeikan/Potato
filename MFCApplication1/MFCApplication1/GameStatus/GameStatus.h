#ifndef __GameStatus_H__
#define __GameStatus_H__
#include "StatusBase.h"
#include "Direct9Tool.h"

typedef StatusBase GameStatus;

enum E_GAME_STATUS
{
	GAME_LOAD_STATUS,
	GAME_IDLE_STATUS,
	GAME_RUN_STATUS,
	GAME_MAX_STATUS,
};

#define NUM_TEXTURE_MAX (11)
#define LOAD_TEXTURE_MAX (3)
#define TEXTURE_MAX (40)

#define RES_DIR _T(".\\resource\\dds")
#define IMAGE _T("dds")


#endif
