#ifndef __GameIdleStatus_H__
#define __GameIdleStatus_H__
#include "GameStatus.h"

class GameIdleStatus : public GameStatus
{
public:
	GameIdleStatus(StatusDrive* pStatusDrive)
		:GameStatus(pStatusDrive)
	{
	}
	
	virtual ~GameIdleStatus(){}

public:
	// 状态进入
	virtual void OnEnter();
	// 状态更新
	virtual void OnUpdate(float deltaTime);
	// 状态离开
	virtual void OnLeave();

	virtual int GetStatusID() { return GAME_IDLE_STATUS;  }


protected:

};

#endif
