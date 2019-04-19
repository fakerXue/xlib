#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	
	/*************************************************************************
	** Name		: StrUtil
	** Desc		: �ַ���������
	** Author	: hicker@2016-8-16 22:57:47
	*************************************************************************/
	class StrUtil
	{
	public:
		static char *ToLower(char *str, char *buff = NULL);
		static char *ToUpper(char *str, char *buff = NULL);
		static bool Compare(const char *str1, const char *str2, bool bIgnoreCase);
		static int IP2Int(const char *strIP);
		static char *IP2Str(int iIP, char *strIP);
		static char *IP2Str(/*struct */sockaddr_in addr, char *strIP);
		static wchar_t *AStr2WStr(const char *str, wchar_t *buff, int buffCnt);
		static char *WStr2AStr(const wchar_t *str, char *buff, int buffCnt);
		static char *Float2Str(float f, char buff[], int buffSize, const char *format = NULL);
		static char *Int2Str(int i, char buff[], int buffSize, const char *format = NULL);
		static int Str2Int(char *iStr);
		static float Str2Float(char *fStr);
#if 0
		static DWORD CalcHash(const char *str);
#else
		static DWORD CalcHash(const char *pStr, bool isUnicode, bool isIgnoreCase);
#endif
		// ���ض������ֽ���
		static int HexChars2Bytes(const char szChars[], char *pBytes, int nBytes);
		// �����ַ�����
		static int Bytes2HexChars(char *pBytes, int nBytes, char szChars[], int nChars);

		/*************************************************************************
		** Desc		: �ָ��ַ���
		** Param	: [str] ���ָ���ַ���
		**			  [div] �ָ���
		**			  [strOut] һ����ά�ַ������飬����ָ��Ĵ�
		**			  [rowCol] ���ֽڱ�ʾstrOut�����������ֽڱ�ʾstrOut������
		** Return	: ���طָ���м�����
		** Author	: hicker@2016-10-26 19:58:35
		*************************************************************************/
		static int Split(const char *str, const char *div, char *strOut[], int rowCol);
		static bool IsChineseHead(const char *str);
		static bool MatchWild(char *szWild, char *szDst, bool isIgnoreCase);
		static inline int matchstar(int c, char *regexp, char *text);
		static inline int matchhere(char *regexp, char *text);
		static char *StrRpl(const char *in, char *out, unsigned int outlen, const char *src, const char *dst);

#ifdef __STD_EN__
		static std::string Float2Str(float f, const char *format = NULL);
		static std::string Int2Str(int i, const char *format = NULL);
		static std::vector<std::string> Split(std::string str, std::string div);
		static std::wstring AStr2WStr(const char *str);
		static std::string WStr2AStr(const wchar_t *str);
		static std::string UTF8ToGBK(const std::string& strUTF8);
#endif
	};


}

#endif

