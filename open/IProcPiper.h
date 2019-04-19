#ifdef __WIN32__

#ifndef __PROCPIPER_H__
#define __PROCPIPER_H__

#include "../depends/define.h"

class IProcPiper
{
public:
	typedef struct tagDOVE
	{
	private:
		int __p; // 必须放在第1位
	public:
		int arg0;
		int arg1;
		int nData;
		int pData;

		tagDOVE(int arg0, int arg1, void *pData, int nData);
		~tagDOVE();

		static tagDOVE *Parse(void *pMem, int nLen);
		void *Mem();
		int Len();
		static const int __min = sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int);
		static const int __max = 1024 * 64;
	}DOVE;

public:
	IProcPiper();
	bool ExecPiper(char *szPipeName, DWORD dwBuffSize);
	void ExitPiper();
	int PipeSend(int arg0, int arg1, void *pData = NULL, int nData = 0);
	bool IsPipeConn();

protected:
	virtual void OnPipeConn(bool isConn) = 0;
	virtual int OnPipeRecv(int arg0, int arg1, void *pData, int nData) = 0;

private:
	bool m_bStop;
	bool m_isConn;
	HANDLE m_hPipeCs2Cpp;
	HANDLE m_hPipeCpp2Cs;
};


#endif

#endif
