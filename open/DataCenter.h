#ifndef __DATACENTER_H__
#define __DATACENTER_H__

#include "../depends/define.h"
#include "xCores.h"
#include "SysUtil.h"

namespace xlib
{
	class DataCenter :public xCores::Engine
	{
	public:
		struct DataPack
		{
			long trTick;
			char *pData;
			int nData;
			DataPack(char *pData, int nData, long tmOffset = 0);
		};

		class IDataSender
		{
		protected:
			virtual void DCSend(char *pData, int nData) = 0;
			friend DataCenter;
		};

		class IDataRecver
		{
		protected:
			virtual void DCRecv(char *pData, int nData) = 0;
			friend DataCenter;
		};

	public:
		DataCenter();
		bool Start();
		bool Stop();
		void SetSender(IDataSender *pSender);
		void SetRecver(IDataRecver *pRecver);

		// 数据入口
		void DCCollect(void *pData, int nData);
		// 数据出口
		void DCPrepare(void *pData, int nData, long tmOffset);
		bool Toggle(bool bGet = false);

	protected:
		int ID_CYLRUNNER;
		bool m_bStop;
		bool m_bRunning;
		xCores::Vector<DataPack*> m_vecSend;
		xCores::Vector<DataPack*> m_vecRecv;
		IDataSender *m_pSender;
		IDataRecver *m_pRecver;

		int Poller(int arg0);
		void DataProcer(long nowTick);
		void DataParser(long nowTick);
	};

}

#endif

