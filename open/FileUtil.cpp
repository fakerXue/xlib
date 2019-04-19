
#include "FileUtil.h"
#include "StrUtil.h"

using namespace xlib;

/*************************************************************************/
/* Desc		: FileUtils类
/* Author	: hicker@2016-11-18 20:52:09
/*************************************************************************/
bool FileUtil::MkDirs(char *pDir)
{
	// hicker@2017-5-21 01:02:37 coding，应该优化为自动修正pDir【在尾部添加缺少的"/"或"\"】
	int iRet = 0;
	char szDirs[1024];
	memset(szDirs, 0, 1024);
	if (NULL == pDir)
		return false;

	strcpy(szDirs, pDir);
	int iLen = strlen(szDirs) - 1;
	if (szDirs[iLen] != '/' && szDirs[iLen] != '\\')
	{
		szDirs[iLen] = '/';
	}
#ifdef __LINUX__
	if (opendir(szDirs) != NULL)
		return true;
#endif

	for (DWORD i = 0; i < strlen(szDirs); i++)
	{
		if ((szDirs[i] == '\\' || szDirs[i] == '/') && i != 0) // etc /logout/writ/
		{
			szDirs[i] = '\0';
			if (0 != access(szDirs, 0))
			{
#ifdef __WIN32__
				if (0 != mkdir(szDirs))
					return false;
#elif defined __LINUX__
				if (-1 == mkdir(szDirs, 0755))
				{
					perror("err\n");
					return false;
				}
#endif
			}
			szDirs[i] = '/';
		}
	}

	return true;
}

int FileUtil::EnumFiles(char *szPath, char *szWild, int nDirDepth, ENUMFILESPROC EnumFilesProc, void *pVoid)
{
	char strFind[1024];
	char szFindDir[1024];
	if (szPath[0] == '*')
	{// 等待实现
		for (char c = 'A'; c <= 'Z'; c++)
		{
			sprintf(szFindDir, "%C:\\", c);
			EnumFiles(szFindDir, szWild, nDirDepth, EnumFilesProc, pVoid);
		}
		return 0;
	}
	if (szPath[strlen(szPath) - 1] != '\\' && szPath[strlen(szPath) - 1] != '/')
		sprintf(szFindDir, "%s\\", szPath);
	else
		sprintf(szFindDir, "%s", szPath);
	sprintf(strFind, "%s*", szFindDir);
#ifdef __WIN32__
	WIN32_FIND_DATA findData;
	HANDLE hFind = ::FindFirstFile(strFind, &findData);//szFindDir
	if (INVALID_HANDLE_VALUE == hFind) return 1;
	do
	{
		if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
			continue;
		char strFullFileName[1024];
		sprintf(strFullFileName, "%s%s", szFindDir, findData.cFileName);
		bool isDir = (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
		if (isDir && nDirDepth>0)
		{
			EnumFiles(strFullFileName, szWild, nDirDepth-1, EnumFilesProc, pVoid);
		}
		if (StrUtil::MatchWild(szWild, strFullFileName,true))
		{
			EnumFilesProc(strFullFileName, isDir ? 0xFFFFFFFF : findData.nFileSizeLow, pVoid);
		}
	} while (FindNextFile(hFind, &findData));
	FindClose(hFind);
#elif defined __LINUX__
	struct dirent *ptr = NULL;
	DIR *dir = NULL;
	if ((dir = opendir(szPath)) == NULL)
		return 0;
	while ((ptr = readdir(dir)) != NULL)
	{
		//current dir OR parrent dir
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		if (ptr->d_type == 4 && nDirDepth>0)    ///dir
		{
			char szFile[MAX_PATH];
			sprintf(szFile, "%s%s", szFindDir, ptr->d_name);
			EnumFiles(szFile, szWild, nDirDepth-1, EnumFilesProc, pVoid);
			EnumFilesProc(szFile, 0xFFFFFFFF, pVoid);
		}
		else if (ptr->d_type == 10)    ///link file ？？？？
			printf("d_name:%s/%s\n", szPath, ptr->d_name);
		else
		{
			char fullFileName[MAX_PATH];
			memset(fullFileName, 0, MAX_PATH);
			sprintf(fullFileName, "%s%s", szFindDir, ptr->d_name);
			struct stat statbuff;
			if (stat(ptr->d_name, &statbuff) < 0)
				EnumFilesProc(fullFileName, 0xFFFFFFFF, pVoid);
			else
				EnumFilesProc(fullFileName, statbuff.st_size, pVoid);
		}
		//if (EnumFilesProc && StringUtils::MatchWildCard(szWild, ptr->d_name))
		//{//ptr->d_type == 8 
		//	char fullFileName[MAX_PATH];
		//	sprintf(fullFileName, "%s%s", szPath, ptr->d_name);
		//	struct stat statbuff;
		//	if (stat(ptr->d_name, &statbuff) < 0)	// #include <sys/stat.h>
		//		EnumFilesProc(fullFileName, -1, p);
		//	else
		//		EnumFilesProc(fullFileName, statbuff.st_size, p);
		//}
	}
	closedir(dir);
#endif
	return 0;
}

int FileUtil::EnumFiles(char * szPath, char * wildCard, int nPathDeep, char pData[], int nData)
{
	int nLength = 1024;
	char strFind[1024];
	char szFindDir[1024];
	memset(pData, 0, nData);
	if (szPath[0] == '*')
	{// 等待实现
		for (char c = 'A'; c <= 'Z'; c++)
		{
			sprintf(szFindDir, "%C:\\", c);
			EnumFiles(szFindDir, wildCard, nPathDeep, pData, nData);
		}
		return 0;
	}
	if (szPath[strlen(szPath) - 1] != '\\' && szPath[strlen(szPath) - 1] != '/')
		sprintf(szFindDir, "%s\\", szPath);
	else
		sprintf(szFindDir, "%s", szPath);
	sprintf(strFind, "%s%s", szFindDir, wildCard);
#ifdef __WIN32__
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindPath = ::FindFirstFile(strFind, &FindFileData);//szFindDir
	if (INVALID_HANDLE_VALUE == hFindPath)return 1;
	do
	{
		if (strcmp(FindFileData.cFileName, ".") == 0 || strcmp(FindFileData.cFileName, "..") == 0)
			continue;
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char szFile[MAX_PATH];
			memset(szFile, 0, MAX_PATH);
			sprintf(szFile, "%s%s", szFindDir, FindFileData.cFileName);
			if (nPathDeep>0)
				EnumFiles(szFile, wildCard, --nPathDeep, pData, nData);
			nLength += sprintf(pData, "%s%s\n",pData, szFile);
		}
		else
		{
			char fullFileName[MAX_PATH];
			memset(fullFileName, 0, MAX_PATH);
			sprintf(fullFileName, "%s%s", szFindDir, FindFileData.cFileName);
			nLength += sprintf(pData, "%s%s\n",pData, fullFileName);
		}
	} while (FindNextFile(hFindPath, &FindFileData) && nLength<nData);
	FindClose(hFindPath);
#elif defined __LINUX__
	struct dirent *ptr = NULL;
	DIR *dir = NULL;
	if ((dir = opendir(szPath)) == NULL)
		return 0;
	while ((ptr = readdir(dir)) != NULL && nLength < nData)
	{
		//current dir OR parrent dir
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
			continue;
		if (ptr->d_type == 4 && nPathDeep>0)    ///dir
		{
			char szFile[MAX_PATH];
			sprintf(szFile, "%s%s", szFindDir, ptr->d_name);
			EnumFiles(szFile, wildCard, --nPathDeep, pData, nData);
			nLength += sprintf(pData, "%s%s\n", pData, szFile);
		}
		else if (ptr->d_type == 10)    ///link file ？？？？
			printf("d_name:%s/%s\n", szPath, ptr->d_name);
		else
		{
			char fullFileName[MAX_PATH];
			memset(fullFileName, 0, MAX_PATH);
			sprintf(fullFileName, "%s%s", szFindDir, ptr->d_name);
			struct stat statbuff;
			nLength += sprintf(pData, "%s%s\n", pData, fullFileName);
		}
		//if (EnumFilesProc && StringUtils::MatchWildCard(wildCard, ptr->d_name))
		//{//ptr->d_type == 8 
		//	char fullFileName[MAX_PATH];
		//	sprintf(fullFileName, "%s%s", szPath, ptr->d_name);
		//	struct stat statbuff;
		//	if (stat(ptr->d_name, &statbuff) < 0)	// #include <sys/stat.h>
		//		EnumFilesProc(fullFileName, -1, p);
		//	else
		//		EnumFilesProc(fullFileName, statbuff.st_size, p);
		//}
	}
	closedir(dir);
#endif
	return 0;
}


DWORD FileUtil::GetFileSize(const char *szFilePath)
{
	if (FILE *fp = fopen(szFilePath, "rb"))
	{
		fseek(fp, 0, SEEK_END);
		DWORD size = ftell(fp);
		fclose(fp);
		return size;
	}
	return 0;
}

bool FileUtil::CopyFileTo(const char *src, const char *dst)
{
#ifdef __WIN32__
	return TRUE == ::CopyFileA(src, dst, FALSE);
#else
	FILE *fp1 = fopen(src, "rb");
	FILE *fp2 = fopen(dst, "wb");
	char szBuff[1024];
	DWORD dwReal;
	if (!fp1 || !fp2)
		return false;
	while ((dwReal = fread(szBuff, 1, 1024, fp1)) > 0)
		fwrite(szBuff, dwReal, 1, fp2);
	fclose(fp1);
	fclose(fp2);
	return true;
#endif
}

bool FileUtil::ReadWrite(const char *szFilePath, void *pData,int nData,bool bRead)
{
	DWORD dwReal = 0;
	if (FILE *fp = fopen(szFilePath, bRead?"rb":"wb"))
	{
		if (bRead)
			dwReal = fread(pData, 1, nData, fp);
		else
			dwReal = fwrite(pData, 1, nData, fp);
		fclose(fp);
	}
	return dwReal >= (DWORD)nData;
}
#if 0
// 此函数不应该存在，需修改 hicker@2017-5-7 15:37:18
bool FileUtil::ChangeDirectory(char *szOld, char *szRel, char *szFull, int nFull)
{
	char szCurPath[512];
	char szOldCurPath[512];

	// 检测oldPath是否合法，并获取其绝对路径
#ifdef __WIN32__
	GetCurrentDirectory(sizeof(szOldCurPath), szOldCurPath);
	if (SetCurrentDirectory(szOld))
		GetCurrentDirectory(sizeof(szCurPath), szCurPath);
	SetCurrentDirectory(szOldCurPath);
#elif defined __LINUX__
	{
		FILE *pp = popen("pwd", "r");
		fgets(szOldCurPath, sizeof(szOldCurPath), pp);
		chdir(szOld);
		getcwd(szCurPath, sizeof(szCurPath));
		chdir(szOldCurPath);
		pclose(pp);
	}
#endif
	// 修正szCurPath，并附加newPath
	int nCur = strlen(szCurPath);
	if (szCurPath[nCur - 1] != '/' && szCurPath[nCur - 1] != '\\')
		strcat(szCurPath, "/");
	if (strrchr(szRel, ':') != NULL)
	{
		strcpy(szCurPath, szRel);
	}
	else
	{
		if (szRel[0] == '/' || szRel[0] == '\\')
			strcat(szCurPath, &szRel[1]);
		else
			strcat(szCurPath, szRel);
	}
	// 二次修正szCurPath
	nCur = strlen(szCurPath);
	if (szCurPath[nCur - 1] != '/' && szCurPath[nCur - 1] != '\\')
		strcat(szCurPath, "/");

	// 检测szCurPath是否合法，并获取其绝对路径
#ifdef __WIN32__
	GetCurrentDirectory(sizeof(szOldCurPath), szOldCurPath);
	if (SetCurrentDirectory(szCurPath))
		GetCurrentDirectory(sizeof(szCurPath), szCurPath);
	SetCurrentDirectory(szOldCurPath);
#elif defined __LINUX__
	{
		FILE *pp = popen("pwd", "r");
		fgets(szOldCurPath, sizeof(szOldCurPath), pp);
		chdir(szCurPath);
		getcwd(szCurPath, sizeof(szCurPath));
		chdir(szOldCurPath);
		pclose(pp);
	}
#endif
	nCur = strlen(szCurPath);
	if (szCurPath[nCur - 1] != '/' && szCurPath[nCur - 1] != '\\')
		strcat(szCurPath, "/");

	strcpy(szFull, szCurPath);
#if 0
#ifdef __WIN32__
	if (SetCurrentDirectory(szCurPath))
		return GetCurrentDirectory(curPathLen, curPath)>0;
#elif defined __LINUX__
	if (chdir(szCurPath) == 0)
		return 0 != getcwd(curPath, curPathLen);
#endif
#endif
	return true;
}

bool FileUtil::TestDirectory(char *szDir, int mode)
{
	return 0 == access(szDir, mode);
}
#endif
char *FileUtil::SplitFilePath(const char *szFilePath, char *szOut, int nOut, bool bFile)
{
	strncpy(szOut, szFilePath,nOut);

	char *pFile = strrchr(szOut, '\\');
	if (pFile == NULL)
		pFile = strrchr(szOut, '/');

	if (bFile)
	{
		if (pFile) strcpy(szOut, pFile + 1);
	}
	else
	{
		if (!pFile)
			strcpy(szOut, ".");
		else
			*(pFile + 1) = 0;
	}

	return szOut;
}

int FileUtil::JudgeDirOrFile(const char *szFile)
{
#ifdef __WIN32__ 
	DWORD dwAttr = ::GetFileAttributes(szFile);  //得到文件属性
	if (dwAttr == 0xFFFFFFFF)    // 文件或目录不存在
		return 0;
	else if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)  // 如果是目录
		return -1;
	else
		return 1;
#elif defined __LINUX__
	struct stat info;
	if (0 != lstat(szFile, &info))
		return 0;
	if (S_ISDIR(info.st_mode))
		return -1;
	else// if (S_ISREG(info.st_mode))
		return 1;
	//else
	//return 0;
#endif
	return 0;
}

bool FileUtil::ReckonDirectory(const char *szOld, const char *szRel, char *szFull, int nFull)
{
	char strOld[1024], strRel[1024];
	bool bRet = true;

	// 统一使用“/”代替“\”
	StrUtil::StrRpl(szOld, strOld, sizeof(strOld), "\\", "/");
	StrUtil::StrRpl(szRel, strRel, sizeof(strRel), "\\", "/");
	int nOld = strlen(strOld);
	int nRel = strlen(strRel);
	if (strOld[nOld - 1] == '/')
	{
		strOld[--nOld] = 0;
	}
	if (strRel[nRel - 1] == '/')
	{
		strRel[--nRel] = 0;
	}

#ifdef __WIN32__
	if (strchr(strRel, ':'))
	{
		strcpy(strOld, strRel);
	}
#elif defined __LINUX__
	if (strRel[0] == '/')
	{
		strcpy(strOld, strRel);
	}
#endif
	else
	{
		for (int i = 0; i < nRel && !strchr(strRel, ':'); i++)
		{
			if (&strRel[i] == strstr(&strRel[i], ".."))
			{ // 上一级
				char *p = strrchr(strOld, '/');
				if (p) *p = 0;
				i += 1;
			}
			else if (strRel[i] != '.' && strRel[i] != '/')
			{ // 下一级
				char *p = strrchr(&strRel[i], '/');
				if (p != NULL) *p = 0;
				sprintf(strOld, "%s/%s", strOld, &strRel[i]);
				i += strlen(&strRel[i]);
			}
		}
	}

	IF0BRET(bRet, 0 != JudgeDirOrFile(strOld));

	strcat(strOld, "/");
#ifdef __WIN32__
	StrUtil::StrRpl(strOld, strOld, sizeof(strOld), "/", "\\");
#endif
	if (bRet) strncpy(szFull, strOld,nFull);

	return bRet;
}

int deleteFileProc(char *filePath, DWORD fileLen, void *p)
{
	bool *pRet = (bool*)p;
	if (fileLen == 0xFFFFFFFF)
	{
		FileUtil::EnumFiles(filePath, "*", true, deleteFileProc, p);
#ifdef __WIN32__
		RemoveDirectory(filePath);
#elif defined __LINUX__
		remove(filePath);
#endif
	}
	else
	{
#ifdef __WIN32__
		DeleteFile(filePath);
#elif defined __LINUX__
		remove(filePath);
#endif
	}
	return 0;
}

bool FileUtil::ClearDirFiles(char* lpPath)
{
	bool bRet = false;
	//return 0==remove(lpPath);
	EnumFiles(lpPath, "*", false, deleteFileProc, &bRet);
	return bRet;
}

/*
EROFS  欲写入的文件为只读文件。
EFAULT  参数filename 指针超出可存取内存空间。
ENAMETOOLONG  参数filename 太长。
ENOMEM  核心内存不足。
ELOOP  参数filename 有过多符号连接问题。
EIO I/O  存取错误。
*/
bool FileUtil::DeleteDirFiles(char* lpPath)
{
	bool bRet = ClearDirFiles(lpPath);
#ifdef __WIN32__
	return bRet && RemoveDirectory(lpPath);
#elif defined __LINUX__
	return bRet && remove(lpPath);
#endif
}

bool FileUtil::RemoveFile(char* lpFilePath)
{
	return remove(lpFilePath) == 0;
}

// 兼容WinCE,[GetCurrentDirectory()适用于XP等系统,在WinCE上不能使用]
bool FileUtil::GetWorkDirectory(char* lpPath, DWORD dwSize, bool onlyPath)
{
#ifdef __WIN32__
	if (!GetModuleFileName(NULL, lpPath, dwSize))
		return false;
#elif defined __LINUX__
#if 0
	getcwd(lpPath, dwSize);
#else
	int rslt = readlink("/proc/self/exe", lpPath, dwSize);
	if (rslt < 0)
		return false;
	lpPath[rslt] = 0;
#endif
#endif
	if (onlyPath)
	{
		char *p = strrchr(lpPath, '\\');
		if (p == NULL)
			p = strrchr(lpPath, '/');
		if (p == NULL)
			return false;
		*(p + 1) = 0;
	}
	return true;
}
#if 0
void* FileUtil::Encrypt(void *out, void *in, DWORD size, BYTE key)
{
	for (DWORD i = 0; i < size; i++)
		((BYTE*)out)[i] = ((BYTE*)in)[i] ^ key;

	return out;
}

void* FileUtil::Decrypt(void *out, void *in, DWORD size, BYTE key)
{
	for (DWORD i = 0; i < size; i++)
		((BYTE*)out)[i] = ((BYTE*)in)[i] ^ key;

	return out;
}
#else
void* FileUtil::Encrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey)
{
	memcpy(pOut, pIn, nLen);
	int i = nLen - 1;
	((char*)pOut)[i] ^= uKey;
	for (; i>0; i--)
	{
		((char*)pOut)[i - 1] ^= ((char*)pOut)[i];
	}

	return pOut;
}

void* FileUtil::Decrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey)
{
	memcpy(pOut, pIn, nLen);
	for (DWORD i = 0; i<nLen - 1; i++)
	{
		((char*)pOut)[i] ^= ((char*)pOut)[i + 1];
	}
	((char*)pOut)[nLen - 1] ^= uKey;

	return pOut;
}
#endif

BYTE* FileUtil::FindBytes(void *pData, DWORD nData, void *pFind, DWORD nFind)
{
	BYTE *p = NULL;
	int nPos = 0;
	while (pData && (p = (BYTE*)memchr((BYTE*)pData + nPos, ((BYTE*)pFind)[0], nData - nPos)) && (nPos = (DWORD)p - (DWORD)pData + 1) <= (int)(nData - nFind))
	{
		if (0 == memcmp(p, pFind, nFind))
		{
			return p;
		}
	}

	return NULL;
}

bool FileUtil::ReplaceBytes(void *pData, int dataLen, const void *oldArr, const void *newArr, int arrLen)
{
	bool isFind = false;
	int idxFind = -1;
	for (int i = 0; i < dataLen - arrLen; i++)
	{
		for (int j = 0; j<arrLen; j++)
		{
			if (((char*)pData)[i + j] == ((char*)oldArr)[j])
			{
				if (j == arrLen - 1)
				{
					isFind = true;
					idxFind = i;
					break;
				}
				continue;
			}
			break;
		}
		if (isFind)
			break;
	}
	if (isFind)
	{
#if 0 // 此段代码会被自动编译为memcpy
		for (int i = idxFind; i < idxFind + arrLen; i++)
			pData[i] = newArr[i - idxFind];
#else
		memcpy(&((char*)pData)[idxFind], newArr, arrLen);
#endif
		return ReplaceBytes(&((char*)pData)[idxFind + arrLen], dataLen - idxFind - 1, oldArr, newArr, arrLen) || isFind;
	}

	return isFind;
}

void FileUtil::ReplaceBytes(void *pDataAddr, void *newArr, int arrLen)
{
	for (int i = 0; i < arrLen; i++)
		((char*)pDataAddr)[i] = ((char*)newArr)[i];
}

