/*************************************************************************
** Desc		: ��־�����
**				��ʵ�ֹ��ܣ����ǿ������ļ����м���ʱ�䣬�ļ��������������߳���
**				Ӧ�����ɶ�����ʽ���ɽ����ʱ���ʱ������⡿
** Author	: hicker@2016-10-22 18:10:51
*************************************************************************/

#include "DbgUtil.h"
#include "FileUtil.h"
#include "SysUtil.h"

using namespace xlib;

#define BUFF_SIZE		(32*1024) // һ����־��ռ���ռ�

DbgUtil *DbgUtil::GetInstance()
{
	static DbgUtil *pInstance = NULL;
	if (pInstance == NULL)
	{
		pInstance = new DbgUtil();
	}
	return pInstance;
	//static Debug instance; // ��ʹ�þ�̬���Ա���������ྲ̬��Ա������cpp�ļ���������ȱ��
	//return new Debug();// &instance;
}


/*************************************************************************
** Desc		: ��ʼ�����ɴ�ӡ10���Ŀ��m_nLevelλ��Ӧ���£�
**				��[10]����[9]����[8]��ʱ[7]����[6]����[5]������[4]���ļ���[3]���к�[2]���߳�id[1]
** Param	: [in] szPath
**			  [in] nLevel 0-��ӡ�����1-����ʱ�����ļ����к�
** Return	: >0��ʾ�ɹ���-1��ʾʧ��
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
** Desc		: ��ʽ�������ע����ʱ�������fopen/printf���ַ�����غ����������⣬�ǳ���֣��������������������ط��ڴ�Խ��
** Param	: [in] szFile
**			  [in] nLine
**			  [in] szFmt
**			  [in] ...
** Return	: >0��ʾ�ɹ���-1��ʾʧ��
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
	// ��֪Ϊ�Σ��ļ�������__FILE__��__TIME__��֮���ļ��ᴴ��ʧ�ܣ�
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
	// ��ȻGetCurrentThreadId����0����ֱ�Ӹ�0ֵ����ᵼ��PostThreadMessage�쳣
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
	printf(m_pInfo); // �����������������������Ʋ���
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




