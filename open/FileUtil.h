#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	class FileUtil
	{
	public:

		/*************************************************************************
		** Desc		: 切换当前应用的工作目录
		** Param	: [pDir] 欲创建的目录，可以是多级；忽略pDir是否以“/”或“\”结尾
		** Return	: 成功返回true，失败返回false
		** Author	: hicker@2016-10-30 21:26:21
		*************************************************************************/
		static bool MkDirs(char *pDir);

		/*************************************************************************
		** Desc		: 遍历目录过程函数
		** Param	: [filePath] 目录或文件的全路径，如果是目录，则末尾不会有“\”或“/”，需要跟fileLen是否为“-1”来判断
		**			  [length] 文件大小，若为目录，则此值为“-1”
		** Return	:
		** Author	: hicker@2016-10-1 13:55:51
		*************************************************************************/
		typedef int(*ENUMFILESPROC)(char *filePath, DWORD fileLen, void *p);

		/*************************************************************************
		** Desc		: 遍历lpPath下的文件和目录，非回调模式下，pData内的文件字符串之间用“\n”分隔
		** Param	: [lpPath]
		**				"*": search at all disks
		**				"X:\\": search at disk named "X"
		**				"X:\\tempFolder\\": search at disk path named "X:\\tempFolder\\"
		**				若lpPath不是以“/”或“\”结尾，则会自动添加至末尾
		**			  [wildCard]
		**				"t?mp*.txt": search file or folder match with "t?mp*.txt";
		**				"*": list all disks symbol;
		**			  [nPathDeep] 查找子目录级别
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
		** Desc		: 切换当前应用的工作目录
		** Param	: [curPath] 当前路径
		**			  [relativePath] 相对路径。
		**					“.”表示当前目录，“..”表示父目录
		**			  [curPathLen] curPath缓冲区大小
		**			  已淘汰！此函数已被ReckonDirectory替代 hicker@2017-7-3 23:16:18
		** Return	: 成功返回true，失败返回false
		** Author	: hicker@2016-10-28 23:28:54
		*************************************************************************/
#if 0
		static bool ChangeDirectory(char *oldPath, char *newPath, char *szFull, int nFull);
		static bool TestDirectory(char *szDir, int mode = 0);
#endif
		/*************************************************************************
		** Desc		: 从路径中获取目录部分或文件名部分
		** Param	: [in] szFilePath 全路径或相对路径
		** Param	: [in] szOut 保存目录串或文件名串
		** Param	: [in] nOut szOut的长度
		** Param	: [in] bFile 0-获取目录，1-获取文件，带'/'或'\'
		** Return	: szOut指针
		** Author	: hicker@2017-7-8 09:33:31
		*************************************************************************/
		static char *SplitFilePath(const char *szFilePath, char *szOut, int nOut, bool bFile = true);

		/*************************************************************************
		** Desc		: 判断szFile是文件/目录或非法路径
		** Param	: [in] szFile 最好是全路径，相对路径待验证
		** Return	: 文件(1)，目录(-1)，非法(0)
		** Author	: hicker@2017-7-8 09:33:31
		*************************************************************************/
		static int JudgeDirOrFile(const char *szFile);

		/*************************************************************************
		** Desc		: 根据当前路径和相对路径计算新路径，会验证路径的合法性
		** Param	: [in] szOld 末尾可跟或不跟“/”或“\”
		** Param	: [in] szRel 相对路径，末尾可跟或不跟“/”或“\”，支持直接切换磁盘，“.”“..”等
		** Param	: [out] szFull 以“\”或“/”结尾
		** Param	: [in] nFull
		** Return	: 是否计算成功，仅成功时szFull才会被修改
		** Author	: hicker@2017-7-3 23:12:42
		*************************************************************************/
		static bool ReckonDirectory(const char *szOld, const char *szRel, char *szFull, int nFull);

		/*************************************************************************
		** Desc		: 清空指定目录；只清空指定目录里的所有文件和文件夹，不删除当前目录
		** Param	: [lpPath] 指定目录
		** Return	: 成功返回true，失败返回false
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static bool ClearDirFiles(char* lpPath);

		/*************************************************************************
		** Desc		: 删除指定目录
		** Param	: [lpPath] 指定目录
		** Return	: 成功返回true，失败返回false
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static bool DeleteDirFiles(char* lpPath);
		static bool RemoveFile(char* lpFilePath);
		static bool GetWorkDirectory(char* lpPath, DWORD dwSize, bool onlyPath = TRUE);

		/*************************************************************************
		** Desc		: 依次异或加密；将pIn第nLen-1个元素用uKey异或，将pIn第nLen-2个元素用pIn[nLen-1]异或，依次类推
		** Param	: [pIn] 需要加密的数据
		**			: [pOut] 加密完成后的数据
		**			: [nLen] 数据长度
		**			: [uKey] 秘钥
		** Return	: 返回pOut
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static void* Encrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey);

		/*************************************************************************
		** Desc		: 依次异或解密；将pIn[0]用第pIn[1]异或，将pIn[1]用pIn[2]异或，依次类推到pIn[nLen-1]，用uKey异或
		** Param	: [pIn] 需要解密的数据
		**			: [pOut] 解密完成后的数据
		**			: [nLen] 数据长度
		**			: [uKey] 秘钥
		** Return	: 返回pOut
		** Author	: hicker@2016-10-30 21:57:02
		*************************************************************************/
		static void* Decrypt(void *pIn, void *pOut, DWORD nLen, unsigned char uKey);

		static BYTE* FindBytes(void *pData, DWORD nData, void *pFind, DWORD nFind);
		static void ReplaceBytes(void *pDataAddr, void *newArr, int arrLen);
		/*************************************************************************
		** Desc		: 将pData中所有oldArr替换为newArr
		** Param	:
		** Return	:
		** Author	: hicker@2016-11-26 22:10:15
		*************************************************************************/
		static bool ReplaceBytes(void *pData, int dataLen, const void *oldArr, const void *newArr, int arrLen);
	};

}

#endif

