#pragma once

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include "IAdo.h"

const BYTE SEND_SELECT_ONLINE[50] = {
	0x00, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x43, 0x4b, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x21, 0x00, 0x01
};

const BYTE SEND_SELECT_SCAN[50] = {
	0x01, 0x00, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x20, 0x43, 0x4b, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x21, 0x00, 0x01
};

inline BYTE* str2byte(const char* instr, int* outlen)
{
	int len = strlen(instr) / 2;
	*outlen = len;
	if (len == 0)
		return NULL;

	BYTE* out = new BYTE[len];
	ZeroMemory(out, sizeof(BYTE) * len);
	char* str = NULL;
	char b[2] = { 0 };
	for (int j = 0; j < len; j++)
	{
		memcpy(b, instr + j * 2, 2);
		char* str = NULL;
		out[j] = (BYTE)strtol(b, &str, 16);
	}
	return out;
}

inline BOOL IsByteString(CString str)
{
	BOOL ret = TRUE;
	LPTSTR s = str.GetBuffer();
	while (*s)
	{
		if (*s >= '0'&&*s <= '9' || *s >= 'a'&&*s <= 'f' || *s >= 'A'&&*s <= 'F')
		{
			s++;
		}
		else
		{
			ret = FALSE;
			break;
		}
	}
	str.ReleaseBuffer();
	return ret;
}

class INetClient
{
public:
	INetClient(_bstr_t strServerIP, UINT uServerPort, DWORD dwConnectTime, bool& isConnect)
	{
		sockClient = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addrSrv;
		addrSrv.sin_addr.S_un.S_addr = inet_addr(strServerIP);
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons((u_short)uServerPort);

		isConnect = false;
		timeval	tm;
		tm.tv_sec = 0;
		tm.tv_usec = dwConnectTime * 1000;
		fd_set set;
		FD_ZERO(&set);
		int error = -1;
		int len = sizeof(int);
		unsigned long ul = 1;
		ioctlsocket(sockClient, FIONBIO, &ul); // 设置为非阻塞模式
		if (connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
		{
			FD_SET(sockClient, &set);
			if (select((int)sockClient + 1, NULL, &set, NULL, &tm) > 0)
			{
				getsockopt(sockClient, SOL_SOCKET, SO_ERROR, (char*)(&error), (int*)&len);
				if (error == 0)
					isConnect = true;
			}
		}
		ul = 0;
		ioctlsocket(sockClient, FIONBIO, &ul); // 设置为非阻塞模式
	}
	virtual ~INetClient()
	{
		shutdown(sockClient, SD_BOTH);
		closesocket(sockClient);
	}

	virtual int Receive(char* lpBuf, int nBufLen)
	{
		return recv(sockClient, lpBuf, nBufLen, 0);
	}

	virtual int Send(const char* lpBuf, int nBufLen)
	{
		return send(sockClient, lpBuf, nBufLen, 0);
	}

protected:
	SOCKET sockClient;
};

class INet : public IAdo, public CSocket
{
public:
	INet()
	{
		loadConfig();

		ZeroMemory(ipServer, 16);
		char chost[MAX_PATH];
		int ret = gethostname(chost, MAX_PATH);
		if (ret == 0)
		{
			struct hostent* pHost = NULL;
			pHost = gethostbyname(chost);
			if (pHost)
				memcpy(ipServer, inet_ntoa(*(struct in_addr*)*pHost->h_addr_list), 16);
		}

		Create(config.udp_scan_port, SOCK_DGRAM);
	}
	~INet()
	{
		StopThread();
		Close();
	}

	void RunThread()
	{
		hOnlineStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		hOnlineThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadOnlineProc, (LPVOID)this, 0, NULL);
		hTaskStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		hTaskThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadTaskProc, (LPVOID)this, 0, NULL);
	}

	void StopThread()
	{
		if (hTaskStopEvent != NULL)
			SetEvent(hTaskStopEvent);
		if (hTaskThread != NULL)
		{
			WaitForSingleObject(hTaskThread, INFINITE);
			CloseHandle(hTaskThread);
			hTaskThread = NULL;
		}
		if (hTaskStopEvent != NULL)
		{
			CloseHandle(hTaskStopEvent);
			hTaskStopEvent = NULL;
		}

		if (hOnlineStopEvent != NULL)
			SetEvent(hOnlineStopEvent);
		if (hOnlineThread != NULL)
		{
			WaitForSingleObject(hOnlineThread, INFINITE);
			CloseHandle(hOnlineThread);
			hOnlineThread = NULL;
		}
		if (hOnlineStopEvent != NULL)
		{
			CloseHandle(hOnlineStopEvent);
			hOnlineStopEvent = NULL;
		}
	}

	virtual void OnReceive(int nErrorCode)
	{
		CString ip;
		UINT port = 0;
		BYTE rd[1024] = {};
		int ret = ReceiveFrom(rd, 1024, ip, port, 0);
		if (ret > 0 && port == 137 && ip != CString(L""))
		{
			if (ip != lastIPOnline && rd[0] == 0x00)
			{
				for (int i = 0; i < (int)devices.size(); i++)
				{
					if (ip == (CString)(LPTSTR)devices[i].addr)
					{
						onlines[i] = true;
						break;
					}
				}
				lastIPOnline = ip;
			}
			if (ip != lastIPScan && rd[0] == 0x01)
			{
				for (int i = 0; i < (int)scans.size(); i++)
				{
					if (ip == (CString)(scans[i].addr))
					{
						int p = rd[56] * 18 + 56;
						for (int j = 1; j < 7; j++)
							scans[i].mac[j - 1] = rd[p + j];
						OnUpdateScanInfo(i);
						break;
					}
				}
				lastIPScan = ip;
			}
		}

		CSocket::OnReceive(nErrorCode);
	} 

	virtual void OnInitDevice() = 0;
	virtual void OnUpdateDeviceState(int index) = 0;
	virtual void OnUninitDevice() = 0;
	virtual void OnInitScan() = 0;
	virtual void OnUpdateScanInfo(int index) = 0;
	virtual void OnUninitScan() = 0;
	virtual void OnInitTask() = 0;
	virtual void OnUpdateTaskInfo(ITaskData task, BYTE run) = 0;
	virtual void OnUninitTask() = 0;

public:
	HANDLE hOnlineStopEvent = NULL;
	HANDLE hOnlineThread = NULL;
	HANDLE hTaskStopEvent = NULL;
	HANDLE hTaskThread = NULL;
	bool scanOnOff = false;
	char ipServer[16];					// 本机IP
	CString lastIPOnline = CString("");	// 上一次接收数据的来源
	CString lastIPScan = CString("");	// 上一次接收数据的来源
	vector<bool> onlines;				// 在线实时列表

private:
	// 测试在线电脑
	DWORD testComputer(int index)
	{
		DWORD dwWait = WAIT_FAILED;
		int ret = SendTo((void*)SEND_SELECT_ONLINE, 50, 137, (LPCTSTR)(LPTSTR)(devices[index].addr));
		if (ret == 50)
			dwWait = WaitForSingleObject(hOnlineStopEvent, 40);
		return dwWait;
	}
	// 测试在线投影
	DWORD testProjection(int index)
	{
		DWORD dwWait = WAIT_FAILED;
		int lens = 0;
		int lenr = 0;
		BYTE* sd = str2byte((char*)devices[index].code_state, &lens);
		BYTE* rd = str2byte((char*)devices[index].code_state_return, &lenr);
		if (sd && rd)
		{
			bool is = false;
			INetClient proj(devices[index].addr, (UINT)devices[index].port, 20, is);
			if (is)
			{
				int ret = proj.Send((char*)sd, lens);
				if (ret == lens)
				{
					dwWait = WaitForSingleObject(hOnlineStopEvent, 20);
					char br[1024] = {};
					ret = proj.Receive(br, lenr);
					if (ret == lenr)
					{
						if (memcmp(br, rd, lenr) == 0)
							onlines[index] = true;
					}
				}
			}
		}
		SAFE_DELETE_ARRAY(sd);
		SAFE_DELETE_ARRAY(rd); 
		return dwWait;
	}
	// 测试在线其它
	DWORD testOther(int index)
	{
		DWORD dwWait = WAIT_FAILED;
		bool is = false;
		INetClient s(devices[index].addr, (UINT)devices[index].port, 20, is);
		dwWait = WaitForSingleObject(hOnlineStopEvent, 20);
		if (is)
			onlines[index] = true;
		return dwWait;
	}
	// 0x00:成功
	// 0x10:成功但不保证执行结果
	// 0x20:无效的命令不再尝试
	// 0x40:超时不再尝试
	// 0x80:错误不再尝试
	// 0x01:继续尝试
	// 打开
	BYTE openDevice(ITaskData task)
	{
		if (task.state == 1 && task.type != _bstr_t("投影"))
		{
			if (task.runcount == 0)
				return 0x20;
			else if (task.runcount < config.task_max_count)
				return 0x00;
		}
		if (task.runcount >= config.task_max_count)
			return 0x40;

		BYTE flag = 0x01;
		if (task.type == _bstr_t("电脑"))
		{
			if (task.mac.length() != 12)
			{
				flag = 0x80;
			}
			else
			{
				CSocket s;
				if (s.Create(config.udp_open_port, SOCK_DGRAM))
				{
					BOOL bOptVal = TRUE;
					s.SetSockOpt(SO_BROADCAST, (char*)&bOptVal, sizeof(BOOL));
					BYTE mac[6];
					char b[2] = { 0 };
					for (int j = 0; j < 6; j++)
					{
						memcpy(b, ((char*)task.mac) + j * 2, 2);
						char* str = NULL;
						mac[j] = (BYTE)strtol(b, &str, 16);
					}
					BYTE c[120];
					for (int i = 0; i < 6; i++)
						c[i] = 0xFF;
					for (int i = 0; i < 16; i++)
						memcpy(c + 6 + i * 6, mac, 6);

					sockaddr_in to;
					to.sin_family = AF_INET;
					//to.sin_port = htons(7);
					to.sin_port = config.udp_open_port_c;
					to.sin_addr.s_addr = htonl(INADDR_BROADCAST);

					task.addr = htonl(INADDR_BROADCAST);
					//s.SendTo(c, 102, config.udp_open_port_c, task.addr);
					s.SendTo(c, 102, (const struct sockaddr *)&to, sizeof(to));
					s.Close();
				}
			}
		}
		else if (task.type == _bstr_t("投影"))
		{
			if (task.port < 1024)
			{
				flag = 0x80;
			}
			else
			{
				int lens = 0;
				BYTE* sd = str2byte((char*)task.code_on, &lens);
				if (sd)
				{
					bool is = false;
					INetClient s(task.addr, (UINT)task.port, 20, is);
					if (is)
						s.Send((char*)sd, lens);
				}
				else
				{
					flag = 0x80;
				}
				SAFE_DELETE_ARRAY(sd);
			}
		}
		else if (task.type == _bstr_t("其它"))
		{
			flag = 0x20;
		}
		return flag;
	}
	// 关闭
	BYTE closeDevice(ITaskData task)
	{
		if (task.state == 0 && task.type == _bstr_t("其他"))
		{
			if (task.runcount == 0)
				return 0x20;
			else if (task.runcount < config.task_max_count)
				return 0x00;
		}
		if (task.runcount >= config.task_max_count)
			return 0x40;

		BYTE flag = 0x01;
		if (task.type == _bstr_t("电脑"))
		{
			HANDLE hToken = NULL;
			TOKEN_PRIVILEGES tkp = { 0 };
			BOOL fResult = FALSE;
			if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			{
				LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
				tkp.PrivilegeCount = 1;
				tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
				if (GetLastError() == ERROR_SUCCESS)
				{
					fResult = InitiateSystemShutdown(task.addr, L"AndControl",
						1, // time-out period 
						FALSE, FALSE); // reboot after shutdown 
					if (!fResult)
					{
					}
					tkp.Privileges[0].Attributes = 0;
					AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
					if (GetLastError() == ERROR_SUCCESS)
					{

					}
				}
			}
		}
		else if (task.type == _bstr_t("投影"))
		{
			if (task.port < 1024)
			{
				flag = 0x80;
			}
			else
			{
				int lens = 0;
				BYTE* sd = str2byte((char*)task.code_off, &lens);
				if (sd)
				{
					bool is = false;
					INetClient s(task.addr, (UINT)task.port, 20, is);
					if (is)
						s.Send((char*)sd, lens);
				}
				else
				{
					flag = 0x80;
				}
				SAFE_DELETE_ARRAY(sd);
			}
		}
		else if (task.type == _bstr_t("其它"))
		{
			flag = 0x20;
		}
		return flag;
	}
	// 消息
	BYTE messageDevice(ITaskData task)
	{
		if (task.type == _bstr_t("投影"))// 投影关机状态仍有网络
			return 0x20;
		if (task.port < 1024)
			return 0x20;
		int id = -1;
		for (int i = 0; i < (int)codebooks.size(); i++)
		{
			if (codebooks[i].code_id == (long)task.task)
			{
				id = i;
				break;
			}
		}
		if (id == -1)
			return 0x20;
		if (task.runcount >= config.task_max_count)
			return 0x40;

		// 不检测电脑状态了
		//if (task.state == 0)
		//	return 0x80;

		BYTE flag = 0x01;
		int lens = 0;
		int lenr = 0;
		BYTE* sd = str2byte((char*)codebooks[id].code_send, &lens);
		BYTE* rd = str2byte((char*)codebooks[id].code_recv, &lenr);
		if (sd)
		{
			bool is = false;
			INetClient s(task.addr, (UINT)task.port, 20, is);
			if (is)
			{
				int ret = s.Send((char*)sd, lens);
				if (ret == lens)
				{
					if (rd)
					{
						WaitForSingleObject(hTaskStopEvent, 20);
						char br[1024] = {};
						ret = s.Receive(br, lenr);
						if (ret == lenr)
						{
							if (memcmp(br, rd, lenr) == 0)
								flag = 0x00;
						}
					}
					else
					{
						flag = 0x10;
					}
				}
			}

		}
		else
		{
			flag = 0x80;
		}
		SAFE_DELETE_ARRAY(sd);
		SAFE_DELETE_ARRAY(rd);

		return flag;
	}

public:
	static DWORD ThreadOnlineProc(LPVOID lParam)
	{
		AfxSocketInit();

		INet* p = (INet*)lParam;
		// 加载设备信息
		p->loadCodeBook();
		p->loadDevice();
		p->onlines.clear();
		for (int i = 0; i < (int)p->devices.size(); i++)
			p->onlines.push_back(false);
		p->OnInitDevice();

		while (WaitForSingleObject(p->hOnlineStopEvent, p->config.scan_setp_time) == WAIT_TIMEOUT)
		{
			DWORD dwWait = WAIT_FAILED;
			// 检测
			p->lastIPOnline = CString("");
			for (int it = 0; it < (int)p->devices.size(); it++)
			{
				p->onlines[it] = false;
				if (p->devices[it].type == _bstr_t("电脑"))
					dwWait = p->testComputer(it);
				else if (p->devices[it].type == _bstr_t("投影"))
					dwWait = p->testProjection(it);
				else if (p->devices[it].type == _bstr_t("其它"))
					dwWait = p->testOther(it);
				if (dwWait == WAIT_OBJECT_0)
					break;
			}
			if (dwWait == WAIT_OBJECT_0)
				break;
			for (int it = 0; it < (int)p->devices.size(); it++)
			{
				if ((p->devices[it].state == 1) != p->onlines[it])
				{
					p->updateDeviceState(p->devices[it].addr, (p->onlines[it] ? 1 : 0));
					p->devices[it].state = (p->onlines[it] ? 1 : 0);
					p->OnUpdateDeviceState(it);
				}
			}

			// 扫描电脑
			if (p->scanOnOff)
			{
				p->OnInitScan();
				p->lastIPScan = CString("");
				for (int i = 0; i < (int)p->scans.size(); i++)
				{
					ZeroMemory(p->scans[i].mac, sizeof(BYTE) * 6);
					if (_bstr_t(p->ipServer) == _bstr_t(p->scans[i].addr))
						continue;

					int ret = p->SendTo((void*)SEND_SELECT_SCAN, 50, 137, (LPCTSTR)(LPTSTR)(_bstr_t(p->scans[i].addr)));
					if (ret == 50)
					{
						dwWait = WaitForSingleObject(p->hOnlineStopEvent, 20);
						if (dwWait == WAIT_OBJECT_0)
							break;
					}
				}
				if (dwWait == WAIT_OBJECT_0)
					break;

				for (int i = 0; i < (int)p->scans.size(); i++)
				{
					if (_bstr_t(p->ipServer) == _bstr_t(p->scans[i].addr))
						continue;

					BYTE t[6] = { 0 };
					if (memcmp(p->scans[i].mac, t, 6) == 0)
						continue;

					bool is = false;
					for (int j = 0; j < (int)p->devices.size(); j++)
					{
						if (p->devices[j].addr == _bstr_t(p->scans[i].addr))
						{
							is = true;
							break;
						}
					}
					if (is)
						continue;

					char cmac[13];
					sprintf_s(cmac, 13, "%02x%02x%02x%02x%02x%02x",
						p->scans[i].mac[0], p->scans[i].mac[1], p->scans[i].mac[2],
						p->scans[i].mac[3], p->scans[i].mac[4], p->scans[i].mac[5]);
					IDeviceData d;
					d.addr = _bstr_t(p->scans[i].addr);
					d.port = 0;
					d.mac = _bstr_t(cmac);
					d.type = _bstr_t("电脑");
					d.state = 1;
					p->addDevice(d);
				}
				p->OnUninitScan();
				p->scanOnOff = false;
				// 重新加载设备信息
				p->loadDevice(true);
				p->onlines.clear();
				for (int i = 0; i < (int)p->devices.size(); i++)
					p->onlines.push_back(false);
				p->OnInitDevice();
			}
		}
		
		return 0;
	}

	static DWORD ThreadTaskProc(LPVOID lParam)
	{
		AfxSocketInit();

		int lastTaskCount = 0;
		INet* p = (INet*)lParam;
		p->clearTask();
		while (WaitForSingleObject(p->hTaskStopEvent, p->config.task_setp_time) == WAIT_TIMEOUT)
		{
			p->loadTask();

			int currentTaskCount = (int)p->tasks.size();
			if (currentTaskCount > 0 && lastTaskCount == 0)
				p->OnInitTask();
			if (currentTaskCount == 0 && lastTaskCount > 0)
				p->OnUninitTask();
			lastTaskCount = currentTaskCount;

			for (int i = 0; i < currentTaskCount; i++)
			{
				BYTE isRun = 0x00;
				// 关闭任务
				if (p->tasks[i].task == 0)
					isRun = p->closeDevice(p->tasks[i]);
				// 开启任务
				if (p->tasks[i].task == 1)
					isRun = p->openDevice(p->tasks[i]);
				// 代码消息任务
				if (p->tasks[i].task > 1)
					isRun = p->messageDevice(p->tasks[i]);

				p->OnUpdateTaskInfo(p->tasks[i], isRun);
				p->updateTaskCount(p->tasks[i].addr);

				// 需要删除的任务
				if ((isRun & 0xf0) == isRun)
					p->clearTask(p->tasks[i].addr);
			}
		}

		return 0;
	}
};

