#pragma once

#include <iostream>
#include <vector>
using namespace std;

#pragma warning(disable:4146)
#include <icrsint.h>
#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","rsEOF")


#define SET_CATCH_ERR(obj)		AfxMessageBox(obj.Description())

struct IConfig
{
	UINT udp_scan_port;				// 扫描电脑在线状态的端口(本机)
	UINT udp_open_port;				// 打开电脑任务的端口(本机)
	UINT udp_open_port_c;			// 打开电脑任务的端口(对方)
	DWORD scan_setp_time;			// 扫描执行间隔时间
	DWORD task_setp_time;			// 任务执行间隔时间
	int task_max_count;				// 任务执行最大次数
};

struct IScanUpdate
{
	_bstr_t addr_s;
	_bstr_t addr_e;
};

struct IDeviceData
{
	_bstr_t	addr;
	_bstr_t mac;
	int port;
	_bstr_t	type;
	_bstr_t	area;
	_bstr_t	exhibit;
	_bstr_t	code_state;
	_bstr_t	code_state_return;
	_bstr_t	code_on;
	_bstr_t	code_off;
	int state;

	IDeviceData()
	{
		addr = _bstr_t("");
		mac = _bstr_t("");
		port = 0;
		type = _bstr_t("");
		area = _bstr_t("");
		exhibit = _bstr_t("");
		code_state = _bstr_t("");
		code_state_return = _bstr_t("");
		code_on = _bstr_t("");
		code_off = _bstr_t("");
		state = 0;
	}
};

struct ITaskData :public IDeviceData
{
	int task = 0;
	int runcount = 0;
};

struct ICodeBookData
{
	long code_id;
	_bstr_t	code_send;
	_bstr_t	code_recv;
	_bstr_t	explain;

	ICodeBookData()
	{
		code_id = 0;
		code_send = _bstr_t("");
		code_recv = _bstr_t("");
		explain = _bstr_t("");
	}
};

struct INetData
{
	char addr[16];
	byte mac[6];
};

class IAdo
{
public:
	IAdo()
	{
		pConn.CreateInstance(__uuidof(Connection));
		pRst.CreateInstance(__uuidof(Recordset));
		pRstTask.CreateInstance(__uuidof(Recordset));
		try
		{
			pConn->ConnectionTimeout = 10;
			pConn->Open(_bstr_t("File Name=conn.udl"),
				_bstr_t(""), _bstr_t(""), adConnectUnspecified);
			pRst->CursorLocation = adUseClient;
			pRst->Open(_bstr_t("select * from main_device order by addr"),
				pConn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
			pRstTask->CursorLocation = adUseClient;
			pRstTask->Open(_bstr_t("select main_device.*, task_list.task, task_list.runcount from main_device inner join task_list on main_device.addr = task_list.addr"),
				pConn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		}
		catch (_com_error e)
		{
			if (pConn->State != adStateClosed)
				pConn->Close();
			SET_CATCH_ERR(e);
		}
	}

	~IAdo()
	{
		try
		{
			if (pRst->State != adStateClosed)
				pRst->Close();
			pRst.Release();
			if (pRstTask->State != adStateClosed)
				pRstTask->Close();
			pRstTask.Release();
			if (pConn->State != adStateClosed)
				pConn->Close();
			pConn.Release();
		}
		catch (_com_error e)
		{
			pRst = NULL;
			pRstTask = NULL;
			pConn = NULL;
			SET_CATCH_ERR(e);
		}
	}

protected:
	_ConnectionPtr pConn;
	_RecordsetPtr pRst;
	_RecordsetPtr pRstTask;
	IConfig	config;						// 配置端口
	vector<IScanUpdate> scan_update;	// 扫描地址
	vector<IDeviceData> devices;		// 设备数据列表
	vector<INetData> scans;				// 配置扫描IP列表
	vector<ITaskData> tasks;			// 设备任务列表
	vector<ICodeBookData> codebooks;	// 编码列表

protected:
	bool loadConfig()
	{
		_RecordsetPtr rst;
		rst.CreateInstance(__uuidof(Recordset));
		try
		{
			rst->CursorLocation = adUseClient;
			rst->Open(_bstr_t("select * from config_system"),
				pConn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
			_variant_t vt;
			ZeroMemory(&config, sizeof(IConfig));
			if (!rst->rsEOF)
			{
				vt = rst->GetCollect(_variant_t("udp_scan_port"));
				if (vt.vt == VT_I2)
					config.udp_scan_port = (UINT)vt.intVal;
				vt = rst->GetCollect(_variant_t("udp_open_port"));
				if (vt.vt == VT_I2)
					config.udp_open_port = (UINT)vt.intVal;
				vt = rst->GetCollect(_variant_t("udp_open_port_c"));
				if (vt.vt == VT_I2)
					config.udp_open_port_c = (UINT)vt.intVal;
				vt = rst->GetCollect(_variant_t("scan_setp_time"));
				if (vt.vt == VT_I2)
					config.scan_setp_time = (DWORD)vt.intVal;
				vt = rst->GetCollect(_variant_t("task_setp_time"));
				if (vt.vt == VT_I2)
					config.task_setp_time = (DWORD)vt.intVal;
				vt = rst->GetCollect(_variant_t("task_max_count"));
				if (vt.vt == VT_I2)
					config.task_max_count = vt.intVal;
			}
			rst->Close();

			rst->Open(_bstr_t("select * from config_addr"),
				pConn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
			scan_update.clear();
			scans.clear();
			while (!rst->rsEOF)
			{
				char addr3[16] = { 0 };
				BYTE start = 0;
				BYTE end = 0;
				_variant_t vts = rst->GetCollect(_variant_t("addr_s"));
				_variant_t vte = rst->GetCollect(_variant_t("addr_e"));
				if (vts.vt == VT_BSTR && vte.vt == VT_BSTR)
				{
					struct in_addr ss;
					inet_pton(AF_INET, _bstr_t(vts), (void*)&ss);
					struct in_addr se;
					inet_pton(AF_INET, _bstr_t(vte), (void*)&se);
					if (ss.S_un.S_un_b.s_b1 == se.S_un.S_un_b.s_b1 &&
						ss.S_un.S_un_b.s_b2 == se.S_un.S_un_b.s_b2 &&
						ss.S_un.S_un_b.s_b3 == se.S_un.S_un_b.s_b3)
					{
						memcpy(addr3, (LPSTR)_bstr_t(vts), 16);
						strrchr(addr3, '.')[1] = '\0';
						if (ss.S_un.S_un_b.s_b4 <= se.S_un.S_un_b.s_b4)
						{
							start = ss.S_un.S_un_b.s_b4;
							end = se.S_un.S_un_b.s_b4;
						}
						else
						{
							start = se.S_un.S_un_b.s_b4;
							end = ss.S_un.S_un_b.s_b4;
						}
						IScanUpdate su = {};
						su.addr_s = _bstr_t(vts);
						su.addr_e = _bstr_t(vte);
						scan_update.push_back(su);
						for (int j = start; j < (int)end; j++)
						{
							INetData data = {};
							sprintf_s(data.addr, 16, "%s%d", (LPSTR)(addr3), (int)j);
							scans.push_back(data);
						}
					}
				}
				rst->MoveNext();
			}
			rst->Close();
			rst.Release();
		}
		catch (_com_error e)
		{
			rst = NULL;
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool loadCodeBook()
	{
		codebooks.clear();
		_RecordsetPtr rst;
		rst.CreateInstance(__uuidof(Recordset));
		try
		{
			rst->CursorLocation = adUseClient;
			rst->Open(_bstr_t("select * from code_book where code_id > 1 order by code_id"),
				pConn.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
			_variant_t vt;
			while (!rst->rsEOF)
			{
				ICodeBookData data;
				vt = rst->GetCollect(_variant_t("code_id"));
				if (vt.vt == VT_I4)
					data.code_id = vt.lVal;
				vt = rst->GetCollect(_variant_t("code_send"));
				if (vt.vt == VT_BSTR)
					data.code_send = (_bstr_t)(vt);
				vt = rst->GetCollect(_variant_t("code_recv"));
				if (vt.vt == VT_BSTR)
					data.code_recv = (_bstr_t)(vt);
				vt = rst->GetCollect(_variant_t("explain"));
				if (vt.vt == VT_BSTR)
					data.explain = (_bstr_t)(vt);
				rst->MoveNext();

				codebooks.push_back(data);
			}

			rst->Close();
			rst.Release();
		}
		catch (_com_error e)
		{
			rst = NULL;
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool loadTask()
	{
		try
		{
			tasks.clear();
			pRstTask->Requery(adOptionUnspecified);
			_variant_t vt;
			while (!pRstTask->rsEOF)
			{
				ITaskData data;
				vt = pRstTask->GetCollect(_variant_t("addr"));
				if (vt.vt != VT_BSTR)
				{
					pRstTask->MoveNext();
					continue;
				}
				data.addr = (_bstr_t)(vt);

				vt = pRstTask->GetCollect(_variant_t("mac"));
				if (vt.vt == VT_BSTR)
					data.mac = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("port"));
				if (vt.vt == VT_I2)
					data.port = vt.intVal;

				vt = pRstTask->GetCollect(_variant_t("type"));
				if (vt.vt == VT_BSTR)
					data.type = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("area"));
				if (vt.vt == VT_BSTR)
					data.area = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("exhibit"));
				if (vt.vt == VT_BSTR)
					data.exhibit = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("code_state"));
				if (vt.vt == VT_BSTR)
					data.code_state = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("code_state_return"));
				if (vt.vt == VT_BSTR)
					data.code_state_return = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("code_on"));
				if (vt.vt == VT_BSTR)
					data.code_on = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("code_off"));
				if (vt.vt == VT_BSTR)
					data.code_off = (_bstr_t)(vt);
				vt = pRstTask->GetCollect(_variant_t("state"));
				if (vt.vt == VT_I2)
					data.state = vt.intVal;
				vt = pRstTask->GetCollect(_variant_t("task"));
				if (vt.vt == VT_I2)
					data.task = vt.intVal;
				vt = pRstTask->GetCollect(_variant_t("runcount"));
				if (vt.vt == VT_I2)
					data.runcount = vt.intVal;
				pRstTask->MoveNext();

				tasks.push_back(data);
			}
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool loadDevice(bool isRequery = true)
	{
		try
		{
			devices.clear();
			if (isRequery)
				pRst->Requery(adOptionUnspecified);
			_variant_t vt;
			while (!pRst->rsEOF)
			{
				IDeviceData data;
				vt = pRst->GetCollect(_variant_t("addr"));
				if (vt.vt != VT_BSTR)
				{
					pRst->MoveNext();
					continue;
				}
				data.addr = (_bstr_t)(vt);

				vt = pRst->GetCollect(_variant_t("mac"));
				if (vt.vt == VT_BSTR)
					data.mac = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("port"));
				if (vt.vt == VT_I2)
					data.port = vt.intVal;

				vt = pRst->GetCollect(_variant_t("type"));
				if (vt.vt == VT_BSTR)
					data.type = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("area"));
				if (vt.vt == VT_BSTR)
					data.area = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("exhibit"));
				if (vt.vt == VT_BSTR)
					data.exhibit = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("code_state"));
				if (vt.vt == VT_BSTR)
					data.code_state = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("code_state_return"));
				if (vt.vt == VT_BSTR)
					data.code_state_return = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("code_on"));
				if (vt.vt == VT_BSTR)
					data.code_on = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("code_off"));
				if (vt.vt == VT_BSTR)
					data.code_off = (_bstr_t)(vt);
				vt = pRst->GetCollect(_variant_t("state"));
				if (vt.vt == VT_I2)
					data.state = vt.intVal;
				pRst->MoveNext();

				devices.push_back(data);
			}
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

public:
	bool updateConfig()
	{
		try
		{
			_bstr_t sql = _bstr_t("update config_system set udp_scan_port=");
			sql += _bstr_t(config.udp_scan_port);
			sql += _bstr_t(", udp_open_port=");
			sql += _bstr_t(config.udp_open_port);
			sql += _bstr_t(", udp_open_port_c=");
			sql += _bstr_t(config.udp_open_port_c);
			sql += _bstr_t(", scan_setp_time=");
			sql += _bstr_t(config.scan_setp_time);
			sql += _bstr_t(", task_setp_time=");
			sql += _bstr_t(config.task_setp_time);
			sql += _bstr_t(", task_max_count=");
			sql += _bstr_t(config.task_max_count);
			pConn->Execute(sql, NULL, adCmdText);

			sql = _bstr_t("delete from config_addr");
			pConn->Execute(sql, NULL, adCmdText);

			for (int i = 0; i < (int)scan_update.size(); i++)
			{
				sql = _bstr_t("insert into config_addr(addr_s, addr_e) values('");
				sql += scan_update[i].addr_s;
				sql += _bstr_t("', '");
				sql += scan_update[i].addr_e;
				sql += _bstr_t("')");
				pConn->Execute(sql, NULL, adCmdText);
			}
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

public:
	bool addDevice(IDeviceData data)
	{
		try
		{
			_bstr_t sql = _bstr_t("insert into main_device(addr, mac, port, area, exhibit, type, code_state, code_state_return, code_on, code_off, state) values('");
			sql += data.addr;
			sql += _bstr_t("', '");
			sql += data.mac;
			sql += _bstr_t("', ");
			sql += _bstr_t(data.port);
			sql += _bstr_t(", '");
			sql += data.area;
			sql += _bstr_t("', '");
			sql += data.exhibit;
			sql += _bstr_t("', '");
			sql += data.type;
			sql += _bstr_t("', '");
			sql += data.code_state;
			sql += _bstr_t("', '");
			sql += data.code_state_return;
			sql += _bstr_t("', '");
			sql += data.code_on;
			sql += _bstr_t("', '");
			sql += data.code_off;
			sql += _bstr_t("', ");
			sql += _bstr_t(data.state);
			sql += _bstr_t(")");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			return updateDevice(data);
		}
		return true;
	}

	bool updateDevice(IDeviceData data)
	{
		try
		{
			_bstr_t sql = _bstr_t("update main_device set mac='");
			sql += data.mac;
			sql += _bstr_t("', port=");
			sql += _bstr_t(data.port);
			sql += _bstr_t(", area='");
			sql += data.area;
			sql += _bstr_t("', exhibit='");
			sql += data.exhibit;
			sql += _bstr_t("', type='");
			sql += data.type;
			sql += _bstr_t("', code_state='");
			sql += data.code_state;
			sql += _bstr_t("', code_state_return='");
			sql += data.code_state_return;
			sql += _bstr_t("', code_on='");
			sql += data.code_on;
			sql += _bstr_t("', code_off='");
			sql += data.code_off;
			sql += _bstr_t("' where addr='");
			sql += data.addr;
			sql += _bstr_t("'");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool updateDeviceState(_bstr_t addr, int state)
	{
		try
		{
			_bstr_t sql = _bstr_t("update main_device set state=");
			sql += _bstr_t(state);
			sql += _bstr_t(" where addr='");
			sql += addr;
			sql += _bstr_t("'");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool clearDevice(_bstr_t addr)
	{
		try
		{
			_bstr_t sql = _bstr_t("delete from main_device");
			if (addr != _bstr_t(""))
			{
				sql += _bstr_t(" where addr='");
				sql += addr;
				sql += _bstr_t("'");
			}
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

public:
	bool addTask(_bstr_t addr, int task)
	{
		try
		{
			_bstr_t sql = _bstr_t("insert into task_list(addr, task, runcount) values('");
			sql += addr;
			sql += _bstr_t("', ");
			sql += _bstr_t(task);
			sql += _bstr_t(", 0)");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			try
			{
				_bstr_t sql = _bstr_t("update task_list set task=");
				sql += _bstr_t(task);
				sql += _bstr_t(", runcount=0 where addr='");
				sql += addr;
				sql += _bstr_t("' and task<>");
				sql += _bstr_t(task);
				pConn->Execute(sql, NULL, adCmdText);
			}
			catch (_com_error e)
			{
				SET_CATCH_ERR(e);
				return false;
			}
		}
		return true;
	}

	bool updateTaskCount(_bstr_t addr)
	{
		try
		{
			_bstr_t sql = _bstr_t("update task_list set runcount=runcount+1 where addr='");
			sql += addr;
			sql += _bstr_t("'");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool clearTask(_bstr_t addr = _bstr_t(""))
	{
		try
		{
			_bstr_t sql = _bstr_t("delete from task_list");
			if (addr != _bstr_t(""))
			{
				sql += _bstr_t(" where addr='");
				sql += addr;
				sql += _bstr_t("'");
			}
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

public:
	bool addCodeBook(ICodeBookData data)
	{
		try
		{
			_bstr_t sql = _bstr_t("insert into code_book(code_send, code_recv, explain) values('");
			sql += data.code_send;
			sql += _bstr_t("', '");
			sql += data.code_recv;
			sql += _bstr_t("', '");
			sql += data.explain;
			sql += _bstr_t("')");
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}

	bool clearCodeBook(long code_id = -1)
	{
		try
		{
			_bstr_t sql = _bstr_t("delete from code_book");
			if (code_id != -1)
			{
				sql += _bstr_t(" where code_id=");
				sql += _bstr_t(code_id);
			}
			pConn->Execute(sql, NULL, adCmdText);
		}
		catch (_com_error e)
		{
			SET_CATCH_ERR(e);
			return false;
		}
		return true;
	}
};