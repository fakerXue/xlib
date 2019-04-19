/*************************************************************************
** Desc		: 日志输出。
**				待实现功能：考虑可配置文件名中加入时间，文件名，进程名，线程名
**				应该做成队列形式【可解决定时输出时间戳问题】
** Author	: hicker@2016-10-22 18:10:51
*************************************************************************/

#include "DbgUtil.h"
#include "FileUtil.h"
#include "SysUtil.h"

using namespace xlib;

#define BUFF_SIZE		(32*1024) // 一条日志所占最大空间

DbgUtil *DbgUtil::GetInstance()
{
	static DbgUtil *pInstance = NULL;
	if (pInstance == NULL)
	{
		pInstance = new DbgUtil();
	}
	return pInstance;
	//static Debug instance; // 不使用静态类成员，避免了类静态成员必须在cpp文件中声明的缺陷
	//return new Debug();// &instance;
}


/*************************************************************************
** Desc		: 初始化，可打印10项，项目和m_nLevel位对应如下：
**				年[10]、月[9]、日[8]、时[7]、分[6]、秒[5]、毫秒[4]、文件名[3]、行号[2]、线程id[1]
** Param	: [in] szPath
**			  [in] nLevel 0-打印所有项，1-月日时分秒文件名行号
** Return	: >0表示成功，-1表示失败
** Author	: hicker@2017-2-26 13:03:10
*************************************************************************/
bool DbgUtil::Init(char *szPath, int nLevel)
{
	m_nLevel = nLevel;

	if (szPath != NULL)
	{
		strcpy(m_szPath, szPath); 
	}
	else
	{
		FileUtil::GetWorkDirectory(m_szPath, sizeof(m_szPath));
	}
	if (!FileUtil::MkDirs(m_szPath))
		return false;

	char strDateTime[64];
	sprintf(m_szFilePath, "%s%s.txt", m_szPath,SysUtil::GetFmtDateTime(strDateTime, sizeof(strDateTime),0,"%Y%m%d-%H%M%S"));
	return true;
}

/*************************************************************************
** Desc		: 格式化输出；注意有时候会遇到fopen/printf等字符串相关函数阻塞问题，非常奇怪，绝大多数问题出在其他地方内存越界
** Param	: [in] szFile
**			  [in] nLine
**			  [in] szFmt
**			  [in] ...
** Return	: >0表示成功，-1表示失败
** Author	: hicker@2017-2-26 13:03:10
*************************************************************************/
void DbgUtil::Out(char *szFile, int nLine, char *szFmt, ...)
{
	char szTemp[1024];
	char *pFile = FileUtil::SplitFilePath(szFile, szTemp,sizeof(szTemp),true);
#ifdef __WIN32__
	WaitForSingleObject((HANDLE)m_mutex, INFINITE);
#elif defined __LINUX__
	pthread_mutex_lock(&m_mutex);
#endif

	memset(m_pLog, 0, BUFF_SIZE);
	va_list body;
	va_start(body, szFmt);
	vsnprintf(m_pLog, BUFF_SIZE, szFmt, body);
	va_end(body);
#if 0
	char logout[256];
	memset(logout, 0, 256);
	// 不知为何，文件名加上__FILE__或__TIME__宏之后，文件会创建失败！
	sprintf(logout, "%s\\%s.txt", PATH_LOG, __DATE__);
	FILE *fp = fopen(logout, "at+");
#else
	FILE *fp = fopen(m_szFilePath, "at+");
#endif
	if (fp == NULL)
	{
#ifdef __WIN32__
		ReleaseMutex((HANDLE)m_mutex);
#elif defined __LINUX__
		pthread_mutex_unlock(&m_mutex);
#endif
		//exit(0);
		return;
	}
	char strDt[64];
	SysUtil::GetFmtDateTime(strDt, sizeof(strDt));
	memset(m_pInfo, 0, BUFF_SIZE);

#ifdef __WIN32__
	// 虽然GetCurrentThreadId返回0，但直接给0值好像会导致PostThreadMessage异常
	int tid=GetCurrentThreadId();
#elif defined __LINUX__
	int tid=pthread_self();
#endif  

	if (m_nLevel == 0)
		sprintf(m_pInfo, "[%s,%s,Tid:%d,L:%d] %s", strDt, pFile, tid, nLine, m_pLog);
	else if (m_nLevel == 1)
		sprintf(m_pInfo, "[%s,%s,L%04d] %s", strDt, pFile, nLine, m_pLog);
	fprintf(fp, m_pInfo);
	fclose(fp);

#ifdef __WIN32__
	OutputDebugStringA(m_pInfo);
#endif
	printf(m_pInfo); // 遇到过阻塞，可能是锁控制不佳
#ifdef __WIN32__
	ReleaseMutex((HANDLE)m_mutex);
#elif defined __LINUX__
	pthread_mutex_unlock(&m_mutex);
#endif
}

void DbgUtil::Box(char *szFile, int nLine, char *fmt, ...)
{
#ifndef __LINUX__
	char out[1024];
	va_list body;
	va_start(body, fmt);
	vsnprintf(out, sizeof(out),fmt, body);
	va_end(body);

	out[1023] = 0;
	Out(szFile,nLine,out);
	MessageBox(NULL, out, "MsgBox", 0);
#endif
}

// szOutFile - only file name, no path
void DbgUtil::OutB(char *szFile, int nLine, char *szOutFile, char *pData, unsigned int dwSize)
{
	char szPathName[260];
	sprintf(szPathName, "%s\\%s", m_szPath, szOutFile);
	bool bRet = FileUtil::ReadWrite(szPathName, pData, dwSize, false);
	Out(szFile, nLine, "[%d] DbgOutB:szPathName=%s,fileSize=%d\n",bRet, szPathName, dwSize);
}

DbgUtil::DbgUtil()
{
#ifdef __WIN32__
	m_mutex = (int)CreateMutex(NULL, FALSE, NULL);
#elif defined __LINUX__
	if (pthread_mutex_init(&m_mutex, NULL)<0)
	{
		perror("pthread_mutex_init");
		return;
	}
#endif
	memset(m_szFilePath, 0, sizeof(m_szFilePath));
	memset(m_szPath, 0, sizeof(m_szPath));
	m_pLog = (char*)malloc(BUFF_SIZE); 
	m_pInfo = (char*)malloc(BUFF_SIZE);
};
DbgUtil::~DbgUtil()
{
#ifdef __WIN32__
	CloseHandle((HANDLE)m_mutex);
#elif defined __LINUX__
	pthread_mutex_destroy(&m_mutex);
#endif
	free(m_pLog);
	free(m_pInfo);
};




