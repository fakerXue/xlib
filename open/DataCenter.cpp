#include "DataCenter.h"

using namespace xlib;

DataCenter::DataPack::DataPack(char *pData, int nData, long tmOffset)
{
	this->trTick = (long)SysUtil::GetCurrentTick(false) + tmOffset;
	this->pData = (char*)malloc(nData);
	this->nData = nData;
	memcpy(this->pData, pData, this->nData);
}

DataCenter::DataCenter() :Engine(this)
{
	ID_CYLRUNNER = GetFreeId();
	m_bStop = true;
	m_bRunning = false;
}

bool DataCenter::Start()
{
	m_bStop = false;
	m_bRunning = false;
	Ignite(ID_CYLRUNNER, /*union_cast<int>(*/&DataCenter::Poller/*)*/, 0);
	return true;
}

bool DataCenter::Stop()
{
	m_bStop = true;
	Stall(ID_CYLRUNNER);
	return true;
}

void DataCenter::SetSender(IDataSender *pSender){ m_pSender = pSender; }
void DataCenter::SetRecver(IDataRecver *pRecver){ m_pRecver = pRecver; }


// 数据入口
void DataCenter::DCCollect(void *pData, int nData)
{
	DataPack *dp = new DataPack((char*)pData, nData);
	//lock(m_lsRecv)
	{
		m_vecRecv.PushBack(dp);
	}
}

// 数据出口
void DataCenter::DCPrepare(void *pData, int nData, long tmOffset)
{
	DataPack *dp = new DataPack((char*)pData, nData, tmOffset);
	//lock(m_lsSend)
	{
		m_vecSend.PushBack(dp);
	}
}

bool DataCenter::Toggle(bool bGet)
{
	if (bGet)return m_bRunning;
	m_bRunning = !m_bRunning;
	return m_bRunning;
}

int DataCenter::Poller(int arg0)
{
	do
	{
		if (!m_bStop && !m_bRunning)
		{
#ifdef __WIN32__
			Sleep(50);
#elif defined __LINUX__
			usleep(50*1000);
#endif
			continue;
		}

		unsigned long tickNow = (long)SysUtil::GetCurrentTick(false);
		DataProcer(tickNow); // 发送
		DataParser(tickNow); // 接收

	} while (!m_bStop); // 经测试，continue可以执行到此条件

	return 0;
};

void DataCenter::DataProcer(long nowTick)
{
	for (int i = 0; i < m_vecSend.Size(); i++)
	{
		if (m_vecSend[i]->trTick < nowTick)
		{
			m_pSender->DCSend(m_vecSend[i]->pData, m_vecSend[i]->nData);
			break;
		}
	}
};

void DataCenter::DataParser(long nowTick)
{
	for (int i = 0; i < m_vecRecv.Size(); i++)
	{
		if (m_vecRecv[i]->trTick < nowTick)
		{
			m_pRecver->DCRecv(m_vecRecv[i]->pData, m_vecRecv[i]->nData);
			break;
		}
	}
};

