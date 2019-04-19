#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	class FileUtil
	{
	public:

		/*************************************************************************
		** Desc		: �л���ǰӦ�õĹ���Ŀ¼
		** Param	: [pDir] ��������Ŀ¼�������Ƕ༶������pDir�Ƿ��ԡ�/����\����β
		** Return	: �ɹ�����true��ʧ�ܷ���false
		** Author	: hicker@2016-10-30 21:26:21
		*************************************************************************/
		static bool MkDirs(char *pDir);

		/*************************************************************************
		** Desc		: ����Ŀ¼���̺���
		** Param	: [filePath] Ŀ¼���ļ���ȫ·���������Ŀ¼����ĩβ�����С�\����/������Ҫ��fileLen�Ƿ�Ϊ��-1�����ж�
		**			  [length] �ļ���С����ΪĿ¼�����ֵΪ��-1��
		** Return	:
		** Author	: hicker@2016-10-1 13:55:51
		*************************************************************************/
		typedef int(*ENUMFILESPROC)(char *filePath, DWORD fileLen, void *p);

		/*************************************************************************
		** Desc		: ����lpPath�µ��ļ���Ŀ¼���ǻص�ģʽ�£�pData�ڵ��ļ��ַ���֮���á�\n���ָ�
		** Param	: [lpPath]
		**				"*": search at all disks
		**				"X:\\": search at disk named "X"
		**				"X:\\tempFolder\\": search at disk path named "X:\\tempFolder\\"
		**				��lpPath�����ԡ�/����\����β������Զ������ĩβ
		**			  [wildCard]
		**				"t?mp*.txt": search file or folder match with "t?mp*.txt";
		**				"*": list all disks symbol;
		**			  [nPathDeep] ������Ŀ¼����
		**			  [EnumFilesProc] typedef int(*ENUMFILESPROC)(char *filePath, DWORD fileLen, void *p);
		**			  [pVoid]
		**				user pointer;
		** Return	:
		** Author	: hicker@2016-8-14 21:26:50
		*************************************************************************/
		static int EnumFiles(char * lpPath, char * wildCard, int nPathDeep, ENUMFILESPROC EnumFilesProc, void *pVoid);
		static int EnumFiles(char * lpPath, char * wildCard, int nPathDeep, char pData[],int nData);
		static DWORD GetFileSize(const char *szFilePath);
		static bool CopyFileTo(const char *src, const char *dst);
		static bool ReadWrite(const char *szFilePath, void *pData, int nData, bool bRead);

		/*************************************************************************
		** Desc		: �л���ǰӦ�õĹ���Ŀ¼
		** Param	: [curPath] ��ǰ·��
		**			  [relativePath] ���·����
		**					��.����ʾ��ǰĿ¼����..����ʾ��Ŀ¼
		**			  [curPathLen] curPath��������С
		**			  ����̭���˺����ѱ�ReckonDirectory��� hicker@2017-7-3 23:16:18
		** Return	: �ɹ�����true��ʧ�ܷ���false
		** Author	: hicker@2016-10-28 23:28:54
		*************************************************************************/
#if 0
		static bool ChangeDirectory(char *oldPath, char *newPath, char *szFull, int nFull);
		static bool TestDirectory(char *szDir, int mode = 0);
#endif
		/*************************************************************************
		** Desc		: ��·���л�ȡĿ¼���ֻ��ļ�������
		** Param	: [in] szFilePath ȫ·�������·��
		** Param	: [in] szOut ����Ŀ¼�����ļ�����
		** Param	: [in] nOut szOut�ĳ���
		** Param	: [in] bFile 0-��ȡĿ¼��1-��ȡ�ļ�����'/'��'\'
		** Return	: szOutָ��
		** Author	: hicker@2017-7-8 09:33:31
		*************************************************************************/
		static char *SplitFilePath(const char *szFilePath, char *szOut, int nOut, bool bFile = true);

		/*************************************************************************
		** Desc		: �ж�szFile���ļ�/Ŀ¼��Ƿ�·��
		** Param	: [in] szFile �����ȫ·�������·������֤
		** Return	: �ļ�(1)��Ŀ¼(-1)���Ƿ�(0)
		** Author	: hicker@2017-7-8 09:33:31
		*************************************************************************/
		static int JudgeDirOrFile(const char *szFile);

		/*************************************************************************
		** Desc		: ���ݵ�ǰ·�������·��������·��������֤·���ĺϷ���
		** Param	: [in] szOld ĩβ�ɸ��򲻸���/����\��
		** Param	: [in] szRel ���·����ĩβ�ɸ��򲻸���/����\����֧��ֱ���л����̣���.����..����
		** Param	: [out] szFull �ԡ�\����/����β
		** Param	: [in] nFull
		** Return	: �Ƿ����ɹ������ɹ�ʱszFull�Żᱻ�޸�
		** Author	: hicker@2017-7-3 23:12:42
		*************************************************************************/
		static bool ReckonDirectory(const char *szOld, const char *szRel, char *szFull, int nFull);

		/*************************************************************************
		** Desc		: ���ָ��Ŀ¼��ֻ���ָ��Ŀ¼��������ļ����ļ��У���ɾ����ǰĿ¼
		** Param	: [lpPath] ָ��Ŀ¼
		** Return	: �ɹ�����true��ʧ�ܷ���false
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static bool ClearDirFiles(char* lpPath);

		/*************************************************************************
		** Desc		: ɾ��ָ��Ŀ¼
		** Param	: [lpPath] ָ��Ŀ¼
		** Return	: �ɹ�����true��ʧ�ܷ���false
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static bool DeleteDirFiles(char* lpPath);
		static bool RemoveFile(char* lpFilePath);
		static bool GetWorkDirectory(char* lpPath, DWORD dwSize, bool onlyPath = TRUE);

		/*************************************************************************
		** Desc		: ���������ܣ���pIn��nLen-1��Ԫ����uKey��򣬽�pIn��nLen-2��Ԫ����pIn[nLen-1]�����������
		** Param	: [pIn] ��Ҫ���ܵ�����
		**			: [pOut] ������ɺ������
		**			: [nLen] ���ݳ���
		**			: [uKey] ��Կ
		** Return	: ����pOut
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static void* Encrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey);

		/*************************************************************************
		** Desc		: ���������ܣ���pIn[0]�õ�pIn[1]��򣬽�pIn[1]��pIn[2]����������Ƶ�pIn[nLen-1]����uKey���
		** Param	: [pIn] ��Ҫ���ܵ�����
		**			: [pOut] ������ɺ������
		**			: [nLen] ���ݳ���
		**			: [uKey] ��Կ
		** Return	: ����pOut
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static void* Decrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey);

		static BYTE* FindBytes(void *pData, DWORD nData, void *pFind, DWORD nFind);
		static void ReplaceBytes(void *pDataAddr, void *newArr, int arrLen);
		/*************************************************************************
		** Desc		: ��pData������oldArr�滻ΪnewArr
		** Param	:
		** Return	:
		** Author	: hicker@2016-11-26 22:10:15
		*************************************************************************/
		static bool ReplaceBytes(void *pData, int dataLen, const void *oldArr, const void *newArr, int arrLen);
	};

}

#endif

