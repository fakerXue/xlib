#ifdef __WIN32__

#include "IProcPiper.h"

IProcPiper::tagDOVE::tagDOVE(int arg0, int arg1, void *pData, int nData)
{
	__p = (int)malloc(__max);
	memset((void*)__p, 0, __max);

	tagDOVE *pThis = (tagDOVE*)__p;
	pThis->arg0 = arg0;
	pThis->arg1 = arg1;
	pThis->nData = nData;
	pThis->pData = (int)&pThis->pData + sizeof(int);
	memcpy((void*)pThis->pData, pData, nData);

	this->arg0 = arg0;
	this->arg1 = arg1;
	this->nData = nData;
	this->pData = pThis->pData;
}

IProcPiper::tagDOVE::~tagDOVE()
{
	free((void*)__p);
}

IProcPiper::tagDOVE *IProcPiper::tagDOVE::Parse(void *pMem, int nLen)
{
	tagDOVE *pThis = (tagDOVE*)pMem;
	if (tagDOVE::__min + pThis->nData != nLen)
		return NULL;
	*((int*)pThis) = (int)(pMem); // 初始化__p
	pThis->pData = (int)((int*)&pThis->pData + 1);
	return (tagDOVE*)pMem;
};

void *IProcPiper::tagDOVE::Mem()
{
	// &__p其实就是(int)this
	// 如果(__p != (int)&__p)，则pDove一定是new出来的，否则是从Parse解析而来
	if (__p != (int)&__p)
	{
		tagDOVE *pThis = (tagDOVE*)__p;
		pThis->arg0 = this->arg0;
		pThis->arg1 = this->arg1;
		pThis->nData = this->nData;
		pThis->pData = (int)&pThis->pData + sizeof(int);
		//memcpy((void*)__p, this, sizeof(tagDOVE));
		//if (pData != NULL && nData > 0)
		//{
		//	memcpy((void*)(__p + sizeof(tagDOVE)), pData, nData);
		//}
	}
	return (void*)__p;
};

int IProcPiper::tagDOVE::Len()
{
	return sizeof(tagDOVE) + nData;
};

IProcPiper::IProcPiper()
{
	m_bStop = true;
	m_isConn = false;
	m_hPipeCs2Cpp = INVALID_HANDLE_VALUE;
	m_hPipeCpp2Cs = INVALID_HANDLE_VALUE;
}

bool IProcPiper::ExecPiper(char *szPipeName, DWORD dwBuffSize)
{
	char strPipeCpp2Cs[128] = { "\\\\.\\Pipe\\" };
	char strPipeCs2Cpp[128] = { "\\\\.\\Pipe\\" };
	strcat(strPipeCpp2Cs, szPipeName);
	strcat(strPipeCpp2Cs, "_cpp2cs");
	strcat(strPipeCs2Cpp, szPipeName);
	strcat(strPipeCs2Cpp, "_cs2cpp");

	m_bStop = false;

	do
	{
		// 创建客户端[发送]
		Sleep(1000);
		if (!WaitNamedPipe((LPCSTR)strPipeCpp2Cs, 3000))
			break;
		m_hPipeCpp2Cs = CreateFile((LPCSTR)strPipeCpp2Cs, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hPipeCpp2Cs == INVALID_HANDLE_VALUE)
		{
			break;
		}

		// 创建服务端[接收]
		// set nonblocking-mode
		m_hPipeCs2Cpp = CreateNamedPipe((LPCSTR)strPipeCs2Cpp, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 256, 256, NULL, NULL);
		DWORD dwNewMode = PIPE_READMODE_MESSAGE; // set for ReadFile/WriteFile change to blocking-mode
		if (m_hPipeCs2Cpp == INVALID_HANDLE_VALUE)
		{
			break;
		}
		if (!ConnectNamedPipe(m_hPipeCs2Cpp, NULL) && GetLastError() != ERROR_PIPE_CONNECTED)
		{
			break;
		}
		if (!SetNamedPipeHandleState(m_hPipeCs2Cpp, &dwNewMode, NULL, NULL))
		{
			break;
		}

		OnPipeConn(m_isConn = true);

		do
		{
			// read data from pipe
			DWORD dwReal = 0;
			char *buf = new char[dwBuffSize];
			BOOL bRet = ReadFile(m_hPipeCs2Cpp, buf, dwBuffSize, &dwReal, NULL);
			DOVE *pDove = DOVE::Parse(buf, dwReal);
			if (!bRet || dwReal < 1 || !pDove)
			{
				break;
			}
			OnPipeRecv(pDove->arg0, pDove->arg1, (void*)pDove->pData, pDove->nData);
			delete buf;

		} while (TRUE);

		OnPipeConn(m_isConn = false);
	} while (0);

	ExitPiper();

	return true;
}

void IProcPiper::ExitPiper()
{
	if (m_hPipeCpp2Cs != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_hPipeCpp2Cs);
		CloseHandle(m_hPipeCpp2Cs);
		m_hPipeCpp2Cs = INVALID_HANDLE_VALUE;
	}

	if (m_hPipeCs2Cpp != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_hPipeCs2Cpp);
		DisconnectNamedPipe(m_hPipeCs2Cpp);
		CloseHandle(m_hPipeCs2Cpp);
		m_hPipeCs2Cpp = INVALID_HANDLE_VALUE;
	}
	m_bStop = true;
}

int IProcPiper::PipeSend(int arg0, int arg1, void *pData, int nData)
{
	DOVE *pDove = new DOVE(arg0, arg1, pData, nData);
	DWORD dwReal = 0;
	BOOL bRet = WriteFile(m_hPipeCpp2Cs, pDove->Mem(), pDove->Len(), &dwReal, NULL);
	if ((!bRet || pDove->Len() != dwReal) && (m_hPipeCpp2Cs != INVALID_HANDLE_VALUE))
	{
		m_hPipeCpp2Cs = INVALID_HANDLE_VALUE;
		FlushFileBuffers(m_hPipeCpp2Cs);
		DisconnectNamedPipe(m_hPipeCpp2Cs);
		CloseHandle(m_hPipeCpp2Cs);
	}

	return dwReal;
}

bool IProcPiper::IsPipeConn(){ return m_isConn; }


#endif
