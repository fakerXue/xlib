#ifndef __APPRECVER_H__
#define __APPRECVER_H__

#include "../depends/define.h"
#include "xCores.h"

// hicker@2017-5-23 08:39:02 coding �����
// �㲥����������ͨ����������������:
// ǰ�ߴ���ȫapp��Χ�ڵĹ�����Ϣ����ȫappֻ��һ���㲥������������Ҫ���۲�����ʽ����ע��/��ע��ӿ�
// ���ߴ����۲��ߵ���Ϣ���������뱻�۲���ͨ��һ��һ��һ�Զ࣬��Ҫ���۲�����ʽ����ע��/��ע��ӿ�

namespace xlib
{

class AppRecver
{
public:
	enum
	{
		ACT_BASE = 0x2000,
		ACT_END = 0x3000, // ��ΪRegistAction�ɱ�εĽ�β���
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
	** Desc		: ע��Action
	** Param	: [in] nLvl �������λ�ã�0��ʾ����ǰ�棬(-1)��ʾ���뵽ĩβ
	** Param	: [in] ... Ҫע���Action��������AppRecver::ACT_NULL��Ϊ������һ�
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
	** Desc		: ע��Action
	** Param	: [in] nAct0 ΪACT_NULLʱ����ʾ��ע������Action
	** Param	: [in] ... ����ע���Action��������AppRecver::ACT_NULL��Ϊ������һ�����1��ΪAppRecver::ACT_NULLʱ��ʾ��ע������Action
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
		// 0�α�ʾע������
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
		// 0�α�ʾ��ע������
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


// ����
// AppRecver::SendAction(AppRecver::AP_ONSEND, this, DatX);
// AppRecver::PostAction(AppRecver::AP_ONSEND, this, DatX);
//public interface AppListener {
//	public void onAppReceive(String action, Intent intent);
//}

#endif

