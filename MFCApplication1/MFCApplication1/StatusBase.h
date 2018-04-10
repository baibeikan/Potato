#ifndef __StatusBase_H__
#define __StatusBase_H__

class StatusDrive;
class StatusBase
{
public:
	StatusBase(StatusDrive* pStatusDrive)
		:m_pStatusDrive(pStatusDrive) {}
	virtual ~StatusBase(){}

public:
	// 状态进入
	virtual void OnEnter() = 0;
	// 状态更新
	virtual void OnUpdate(float deltaTime) = 0;
	// 状态离开
	virtual void OnLeave() = 0;

	virtual int GetStatusID() = 0;
	// 输入处理
	virtual bool InputProcess(int message, int wParam, int lParam) { return false; }
protected:
	StatusDrive* m_pStatusDrive;
};

#endif
