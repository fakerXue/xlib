#ifndef __APPRECVER_H__
#define __APPRECVER_H__

#include "../depends/define.h"
#include "xCores.h"

// hicker@2017-5-23 08:39:02 coding 待设计
// 广播接收器与普通监听器的区别在于:
// 前者处理全app范围内的公共消息，故全app只有一个广播接收器，不需要被观察者显式调用注册/反注册接口
// 后者处理被观察者的消息，监听器与被观察者通常一对一或一对多，需要被观察者显式调用注册/反注册接口

namespace xlib
{

class AppRecver
{
public:
	enum
	{
		ACT_BASE = 0x2000,
		ACT_END = 0x3000, // 作为RegistAction可变参的结尾标记
	};

	virtual void OnAppAction(int nAct, char *str4k, void *wp, void *lp) = 0;

	static BOOL Initialize()
	{
		int cid = AppEngine::GetInstance()->GetCID();
		return AppEngine::GetInstance()->Ignite(cid, &AppEngine::Cylinder, 0);
	}

	static void UnInitialize()
	{
		int cid = AppEngine::GetInstance()->GetCID();
		AppEngine::GetInstance()->Stall(cid,1000);
	}

	/*************************************************************************
	** Desc		: 注册Action
	** Param	: [in] nLvl 欲插入的位置，0表示在最前面，(-1)表示插入到末尾
	** Param	: [in] ... 要注册的Action，必须以AppRecver::ACT_NULL作为变参最后一项；
	** Return	: 
	** Author	: hicker@2018-2-9 17:29:22
	*************************************************************************/
	void RegistAction(int nLvl, ...)
	{
		va_list body;
		va_start(body, nLvl);
		AppEngine::GetInstance()->RegistAction(this, nLvl, body);
		va_end(body);
	}

	/*************************************************************************
	** Desc		: 注册Action
	** Param	: [in] nAct0 为ACT_NULL时，表示反注册所有Action
	** Param	: [in] ... 欲反注册的Action，必须以AppRecver::ACT_NULL作为变参最后一项，当第1项为AppRecver::ACT_NULL时表示反注册所有Action
	** Return	:
	** Author	: hicker@2018-2-9 17:29:22
	*************************************************************************/
	void UnRegistAction(int nAct0=ACT_END, ...)
	{
		va_list body;
		va_start(body, nAct0);
		AppEngine::GetInstance()->UnRegistAction(this, nAct0, body);
		va_end(body);
	}

	static bool SendAction(int nAct, char *str4K, void *wp = NULL, void *lp = NULL)
	{
		return AppEngine::GetInstance()->SendAction(nAct, str4K, wp, lp);
	}

	static void PostAction(int nAct, char *str4K, void *wp = NULL, void *lp = NULL)
	{
		AppEngine::GetInstance()->PostAction(nAct, str4K, wp, lp);
	}

private:

	class AppEngine :public xCores::Engine
	{
	private:
		AppEngine() :Engine(this), m_msgQue(1024){ m_pSignal = new xCores::Signal(); ID_CYLONLY = GetFreeId(); };
		virtual ~AppEngine(){ delete m_pSignal; };
	public:
		static AppEngine* GetInstance()
		{
			static AppEngine *pAppEngine = NULL;
			if (!pAppEngine) pAppEngine = new AppEngine();
			return pAppEngine;
		}

		int GetCID(){return ID_CYLONLY;}

		int Cylinder(int iNul)
		{
			xCores::MsgQue<AppRecver*>::QMSG qMsg;
			while (m_msgQue.PopFront(&qMsg))
			{
				for (xCores::Map<AppRecver*, xCores::Vector<int> >::Iterator it = m_mapReceiver.Begin(); it != m_mapReceiver.End(); it++)
				{
					xCores::Vector<int>::Iterator itFind = xCores::Find(it->second.Begin(), it->second.End(), qMsg.i4);//it->second.Begin();
					if (itFind != it->second.End())
					{
						it->first->OnAppAction(qMsg.i4, qMsg.str4k, qMsg.wp, qMsg.lp);
					}
				}
				m_pSignal->Notify();
				if (0 == TryState(ID_CYLONLY))
					break;
			}
			return 0;
		}

	protected:
		// 0参表示注册所有
		void RegistAction(AppRecver *pReceiver, int nLvl, va_list body)
		{
			xCores::Vector<int> vecAct;
			int nAct = va_arg(body, int);
			for (int i = nAct; i < ACT_END; i = va_arg(body, int))
			{
				vecAct.Insert(nLvl, i);
			}
			m_mapReceiver[pReceiver] = vecAct;
		}
		// 0参表示反注册所有
		void UnRegistAction(AppRecver *pReceiver, int nAct0, va_list body)
		{
			//DBGOUT("nAct=%08X,pReceiver=%d,m_mapReceiver[pReceiver].Size()=%d\n", nAct, pReceiver, m_mapReceiver[pReceiver].Size());
			if (nAct0 == ACT_END)
			{
				m_mapReceiver[pReceiver].Clear();
			}
			else
			{
				for (int i = va_arg(body, int); i < ACT_END; i = va_arg(body, int))
				{
					xCores::Vector<int>::Iterator it = xCores::Find(m_mapReceiver[pReceiver].Begin(), m_mapReceiver[pReceiver].End(), i);
					if (it != m_mapReceiver[pReceiver].End())
						m_mapReceiver[pReceiver].Erase(it);
				}
			}
		}

		bool SendAction(int nAct, char *str4k, void *wp = NULL, void *lp = NULL)
		{
			PostAction(nAct, str4k, wp, lp);
			m_pSignal->Wait();
			return true;
		}

		void PostAction(int nAct, char *str4k, void *wp = NULL, void *lp = NULL)
		{
			m_msgQue.Post(nAct, str4k, wp, lp);
		}

	private:
		int ID_CYLONLY;
		xCores::Signal *m_pSignal;
		xCores::Map<AppRecver*, xCores::Vector<int> > m_mapReceiver;
		xCores::MsgQue<AppRecver*> m_msgQue;
		friend class AppRecver;
	};
};

}


// 用例
// AppRecver::SendAction(AppRecver::AP_ONSEND, this, DatX);
// AppRecver::PostAction(AppRecver::AP_ONSEND, this, DatX);
//public interface AppListener {
//	public void onAppReceive(String action, Intent intent);
//}

#endif

