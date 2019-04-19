
#include "StrUtil.h"

using namespace xlib;

char *StrUtil::ToLower(char *str, char *buff)
{
	char *orign = str;
	char *ch = str;
	if (buff != NULL)
	{
		strcpy(buff, str);
		orign = buff;
		ch = buff;
	}
	for (; *ch != 0; ch++)
	{
		if (*ch >= 'A' && *ch <= 'Z')
			*ch = *ch - 'A' + 'a';
	}
	return orign;
}

char *StrUtil::ToUpper(char *str, char *buff)
{
	char *orign = str;
	char *ch = str;
	if (buff != NULL)
	{
		strcpy(buff, str);
		orign = buff;
		ch = buff;
	}
	for (; *ch != 0; ch++)
	{
		if (*ch >= 'a' && *ch <= 'z')
			*ch = *ch - 'a' + 'A';
	}
	return orign;
}

bool StrUtil::Compare(const char *str1, const char *str2, bool bIgnoreCase)
{
	if (!str1 || !str2)
		return false;
	char *p1 = (char*)malloc(strlen(str1) + 1);
	char *p2 = (char*)malloc(strlen(str2) + 1);
	strcpy(p1, str1);
	strcpy(p2, str2);
	int iRet = 0;
	if (bIgnoreCase)
	{
		iRet = strcmp(ToLower(p1), ToLower(p2));
	}
	else
	{
		iRet = strcmp(str1, str2);
	}
	free(p1);
	free(p2);
	return 0 == iRet;
}

int StrUtil::IP2Int(const char *strIP)
{
	//return ntohl(inet_addr(strIP));
	return inet_addr(strIP);
}

char *StrUtil::IP2Str(int iIP, char *strIP)
{
	struct sockaddr_in addr;
#ifdef __WIN32__
	addr.sin_addr.S_un.S_addr = iIP;
#elif defined __LINUX__
	addr.sin_addr.s_addr = iIP;
#endif
	strcpy(strIP, (char*)inet_ntoa(addr.sin_addr));
	return strIP;
}

char *StrUtil::IP2Str(/*struct */sockaddr_in addr, char *strIP)
{
	strcpy(strIP, (char*)inet_ntoa(addr.sin_addr));
	return strIP;
}

wchar_t *StrUtil::AStr2WStr(const char *str, wchar_t *buff, int buffCnt)
{
	mbstowcs(buff, str, buffCnt);
	return buff;
}

char *StrUtil::WStr2AStr(const wchar_t *str, char *buff, int buffCnt)
{
	wcstombs(buff, str, buffCnt);
	return buff;
}

#if 0
DWORD StrUtil::CalcHash(const char *str)
{
#define ROTR32(value, shift)	(((DWORD) value >> (BYTE) shift) | ((DWORD) value << (32 - (BYTE) shift)))
	DWORD hash;
	const char *pTempChar;
	hash = 0;
	while (*str != '\0' && *str != ' ')
	{
		pTempChar = (str++);
		hash = ROTR32(hash, 13);
		if (*pTempChar >= 0x61)
			hash += *pTempChar - 0x20;
		else
			hash += *pTempChar;
	}
	return hash;
#undef ROTR32
}
#else
DWORD StrUtil::CalcHash(const char *pStr, bool isUnicode, bool isIgnoreCase)
{
	DWORD hash = 0;
	for (; pStr && *pStr; isUnicode ? pStr += 2 : pStr++)
	{
		// 也可以乘以31、131、1313、13131、131313..  
		hash = hash * 131 + (DWORD)((isIgnoreCase && (*pStr) >= 0x61) ? (*pStr) - 0x20 : (*pStr));
		// 有人说将乘法分解为位运算及加减法可以提高效率，如将上式表达为：hash = hash << 7 + hash << 1 + hash + ch;  
		// 但其实在Intel平台上，CPU内部对二者的处理效率都是差不多的，  
		// 我分别进行了100亿次的上述两种运算，发现二者时间差距基本为0（如果是Debug版，分解成位运算后的耗时还要高1/3）；  
		// 在ARM这类RISC系统上没有测试过，由于ARM内部使用Booth's Algorithm来模拟32位整数乘法运算，它的效率与乘数有关：  
		// 当乘数8-31位都为1或0时，需要1个时钟周期  
		// 当乘数16-31位都为1或0时，需要2个时钟周期  
		// 当乘数24-31位都为1或0时，需要3个时钟周期  
		// 否则，需要4个时钟周期  
		// 因此，虽然我没有实际测试，但是我依然认为二者效率上差别不大       
	}
	return hash;
}
#endif

int StrUtil::HexChars2Bytes(const char szChars[], char *pBytes, int nBytes)
{
	char strHex[10] = { 0 };
	unsigned int i = 0;
	for (; i < strlen(szChars) && i < (unsigned int)nBytes; i++)
	{
		sprintf(strHex, "0x%c%c", szChars[i * 2], szChars[i * 2 + 1]);
		pBytes[i] = (char)StrUtil::Str2Int(strHex);
	}
	return i/2;
}

int StrUtil::Bytes2HexChars(char *pBytes, int nBytes, char szChars[], int nChars)
{
	char strHex[10] = { 0 };
	int i = 0;
	for (; i < nBytes && i < nChars; i++)
	{
		sprintf(&szChars[i*2], "%02x", pBytes[i]);
	}
	return i*2;
}

bool StrUtil::IsChineseHead(const char *str)
{
	//如果字符高位为1且下一字符高位也是1则有中文字符
	if ((str[0] & 0x80) && (str[1] & 0x80))
		return true;
	return false;
}

bool StrUtil::MatchWild(char *szWild, char *szDst, bool isIgnoreCase)
{
	if (szWild == NULL || szDst == NULL) return false;
	int len1 = strlen(szDst);
	int len2 = strlen(szWild);
	int mark = 0;//用于分段标记,'*'分隔的字符串
	int p1 = 0, p2 = 0;

	while (p1 < len1 && p2 < len2)
	{
		if (szWild[p2] == '?')
		{
			p1++;
			p2++;
			continue;
		}
		if (szWild[p2] == '*')
		{ // 如果当前是*号，则mark前面一部分已经获得匹配，从当前点开始继续下一个块的匹配
			p2++;
			mark = p2;
			continue;
		}

		char cDst[2] = { szDst[p1], 0 };
		char cWild[2] = { szWild[p2], 0 };
		if (isIgnoreCase)
		{
			StrUtil::ToLower(cDst);
			StrUtil::ToLower(cWild);
		}
		if (cDst[0] != cWild[0])
		{
			if (p1 == 0 && p2 == 0)
			{ // 如果是首字符，特殊处理，不相同即匹配失败
				return false;
			}
			/*
			* pattern: ...*bdef*...
			*       ^
			*       mark
			*        ^
			*        p2
			*       ^
			*       new p2
			* str1:.....bdcf...
			*       ^
			*       p1
			*      ^
			*     new p1
			* 如上示意图所示，在比到e和c处不想等
			* p2返回到mark处，
			* p1需要返回到下一个位置。
			* 因为*前已经获得匹配，所以mark打标之前的不需要再比较
			*/
			p1 -= p2 - mark - 1;
			p2 = mark;
			continue;
		}
		// 此处处理相等的情况
		p1++;
		p2++;
	}
	if (p2 == len2)
	{
		if (p1 == len1)
		{ // 两个字符串都结束了，说明模式匹配成功
			return true;
		}
		if (szWild[p2 - 1] == '*')
		{ // str1还没有结束，但pattern的最后一个字符是*，所以匹配成功
			return true;
		}
	}
	while (p2<len2)
	{ // pattern多出的字符只要有一个不是*,匹配失败
		if (szWild[p2] != '*')
			return false;
		p2++;
	}
	return true;
}

char *StrUtil::StrRpl(const char *in, char *out, unsigned int outlen, const char *src, const char *dst)
{
	char *temp = (char*)malloc(sizeof(char)*(strlen(in) + 1));
	memcpy(temp, in, strlen(in) + 1);
	memset(out, 0, outlen);
	char *p = temp;
	unsigned int len = outlen - 1;
	while ((*p != 0) && (len > 0))
	{
		if (strncmp(p, src, strlen(src)) != 0)
		{
			unsigned int n = strlen(out);
			out[n] = *p;
			out[n + 1] = 0;
			p++;
			len--;
		}
		else
		{
			strcat(out, dst);
			p += strlen(src);
			len -= strlen(dst);
		}
	}
	free(temp);
	return out;
}

char *StrUtil::Float2Str(float f, char buff[], int buffSize, const char *format)
{
	char temp[MAX_PATH];
	memset(temp, 0, MAX_PATH);
	if (format != NULL)
		sprintf(temp, format, f);
	else
		sprintf(temp, "%f", f);
	strncpy(buff, temp, buffSize);
	return buff;
}

char *StrUtil::Int2Str(int i, char buff[], int buffSize, const char *format)
{
	char temp[MAX_PATH];
	memset(temp, 0, MAX_PATH);
	if (format != NULL)
		sprintf(temp, format, i);
	else
		sprintf(temp, "%d", i);

	strncpy(buff, temp, buffSize);

	return buff;
}

int StrUtil::Str2Int(char *iStr)
{
	int iRet = 0;
#if 1
	if (strncmp(iStr, "0x", 2) == 0 || strncmp(iStr, "0X", 2) == 0)
		sscanf((const char *)iStr, "%x", &iRet);
	else
		sscanf((const char *)iStr, "%d", &iRet);
#else
	iRet = _ttoi(iStr);
#endif
	return iRet;
}

float StrUtil::Str2Float(char *fStr)
{
	return (float)atof(fStr);
}

int StrUtil::Split(const char *str, const char *div, char *strOut[], int rowCol)
{
	int row = rowCol >> 16;
	int col = rowCol & 0x0000FFFF;
	int divLen = strlen(div);
	int i = 0;
	char *strClone = (char*)malloc(sizeof(char)*(strlen(str) + 1));
	memcpy(strClone, str, strlen(str) + 1);
	char *pStrClone = strClone;
	if (pStrClone != NULL)
	{
		char *p = pStrClone;
		do
		{
			if (strncmp(pStrClone, div, divLen) == 0)
			{
				memset(pStrClone, 0, divLen);
				if (*p) strcpy((char*)((unsigned int)strOut + ((i++)*col)), p);
				p += strlen(p) + divLen;
				pStrClone += divLen - 1;
			}
			if (i == row)
				break;
		} while (*(++pStrClone));
		if (*p && i != row)
		{
			strcpy((char*)((unsigned int)strOut + ((i++)*col)), p);
		}
	}
	free(strClone);

	return i;
}

#ifdef __STD_EN__
std::vector<std::string> StrUtil::Split(std::string str, std::string div)
{
	std::vector<std::string> vecStr;
	for (int i = 0, j = str.find(div.c_str(), 0);;j = str.find(div.c_str(), i))
	{
		if (j == std::string::npos)
		{
			vecStr.push_back(str.substr(i,str.length()-i));
			break;
		}
		else
		{
			vecStr.push_back(str.substr(i, j - i));
		}
		i = j+div.length();
	}
	return vecStr;
}

std::wstring StrUtil::AStr2WStr(const char *str)
{
	const std::string strA(str);
	const char* pStr = strA.c_str();
	int len = strA.length();

	// figure out how many wide characters we are going to get   
	int nChars = MultiByteToWideChar(CP_ACP, 0, pStr, len, NULL, 0);
	if (len == -1)
		--nChars;
	if (nChars == 0)
		return L"";

	// convert the narrow string to a wide string   
	// nb: slightly naughty to write directly into the string like this  
	std::wstring buf;
	buf.resize(nChars);
	::MultiByteToWideChar(CP_ACP, 0, pStr, len, const_cast<wchar_t*>(buf.c_str()), nChars);

	return buf;
}

std::string StrUtil::WStr2AStr(const wchar_t *str)
{
	const std::wstring strW(str);
	const wchar_t* pStr = strW.c_str();
	int len = strW.length();

	int nChars = WideCharToMultiByte(CP_ACP, 0, pStr, len, NULL, 0, NULL, NULL);
	if (len == -1)
		--nChars;
	if (nChars == 0)
		return "";

	// convert the wide string to a narrow string  
	// nb: slightly naughty to write directly into the string like this  
	std::string buf;
	buf.resize(nChars);
	WideCharToMultiByte(CP_ACP, 0, pStr, len, const_cast<char*>(buf.c_str()), nChars, NULL, NULL);
	return buf;
}

std::string StrUtil::Float2Str(float f, const char *format)
{
	char temp[MAX_PATH];
	return std::string(Float2Str(f, temp, MAX_PATH, format));
}

std::string StrUtil::Int2Str(int i, const char *format)
{
	char temp[MAX_PATH];
	return std::string(Int2Str(i, temp, MAX_PATH, format));
}

std::string StrUtil::UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = (unsigned short *)malloc(sizeof(unsigned short)*(len + 1));
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(strUTF8.c_str()), -1, (LPWSTR)wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = (char*)malloc(sizeof(char)*(len + 1));
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	free(szGBK);
	free(wszGBK);
	return strTemp;
}
#endif


