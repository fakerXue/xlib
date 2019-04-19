#ifndef __TASKRUNNER_H__
#define __TASKRUNNER_H__

#include "../depends/define.h"
#include "xCores.h"

//class ITaskRunnerHandler
//{
//public:
//	int (ITaskRunnerHandler::*pFunc)(int, int, int, char*);
//
//	void func()
//	{
//		(this->*pFunc)(0,0,0,"14");
//	}
//};
//
//typedef int (ITaskRunnerHandler::*NONPTRFUNC)(int, int, int, char*);

namespace xlib
{

template<class T>
class TaskRunner:public xCores::Engine
{
public:
	struct TaskInfo
	{
		TaskInfo(int iType, int (T::*pfnProc)(int, int, int, char*, long), int arg0, int arg1, char *arg2, long tmOffset = 0)
		{
			static int _id_ = 1;
			this->id = _id_++;
			this->iType = iType;
			this->pfnProc = pfnProc;
			this->arg0 = arg0;
			this->arg1 = arg1;
			if (arg2) strncpy(arg2, arg2, sizeof(this->arg2));
			this->trigTick = xUtils::SysUtil::GetCurrentTick() + tmOffset;
		}
		int (T::*pfnProc)(int, int, int, char*);
		int id;
		int iType;
		int arg0;
		int arg1;
		char arg2[4096];
		unsigned long trigTick;
	};

public:
	TaskRunner(T *pHandler) :Engine(this)
	{
		ID_CYLRUNNER = GetFreeId();
		m_pHandler = pHandler; 
		m_bStop = true;
		m_bRunning = false;
	};

	bool Start()
	{
		m_bStop = false;
		m_bRunning = false;
		Ignite(ID_CYLRUNNER, /*union_cast<int>(*/&TaskRunner::Runner/*)*/, 0);
		return true;
	}

	bool Stop()
	{
		m_bStop = true;
		Stall(ID_CYLRUNNER);
		return true;
	}

	int Load(int (T::*pFunc)(int, int, int, char*), int arg0, int arg1, char *str4k, long tmOffset = 0)
	{
		TaskInfo *pTaskInfo = new TaskInfo(pFunc, arg0, arg1, str4k, tmOffset);
		m_vecTaskInfo.PushBack(pTaskInfo);

		return pTaskInfo->id;
	}

	// 若id<1则根据iType卸载，若id>0则根据id卸载并忽略iType，返回卸载的任务个数
	int Unload(int id, int iType)
	{
		int iCount = 0;
		for (typename xCores::Vector<TaskInfo*>::Itertor it = m_vecTaskInfo.Begin(); it != m_vecTaskInfo.End();)
		{
			if (id<1 && iType>0)
			{
				if (it->iType == iType)
				{
					m_vecTaskInfo.Erase(it);
					it = m_vecTaskInfo.Begin();
					iCount++;
					continue;
				}
			}
			else if (it->id == id)
			{
				m_vecTaskInfo.Erase(it);
				iCount++;
				break;
			}
			it++;
		}
		return iCount;
	}

	bool Toggle(bool bGet=false)
	{
		if (bGet)return m_bRunning;
		m_bRunning = !m_bRunning;
		return m_bRunning;
	}

protected:
	int ID_CYLRUNNER;
	T *m_pHandler;
	bool m_bStop;
	bool m_bRunning;
	xCores::Vector<TaskInfo*> m_vecTaskInfo;
	//std::queue<TaskInfo*> m_vecTaskInfo;

	int Runner(int arg0)
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

			unsigned long tickNow = xUtils::SysUtil::GetCurrentTick();
			for (typename xCores::Vector<TaskInfo*>::Iterator it = m_vecTaskInfo.Begin(); it != m_vecTaskInfo.End();it++)
			{
				if (it->trigTick < tickNow)
				{
					(m_pHandler->*it->pfnProc)(it->id, it->arg0, it->arg1, it->arg2);
					delete m_vecTaskInfo.Erase(it);
					break;
				}
			}

			//delete pTaskInfo;
		} while (!m_bStop); // 经测试，continue可以执行到此条件

		return 0;
	}
};

}

#endif

