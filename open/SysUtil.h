#ifndef __SYSUTIL_H__
#define __SYSUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	
	class SysUtil
	{
	public:
		// 此类函数声明不能加static
		static void RedirectIOToConsole();
		static int ReadLine(char buff[], bool mode = 0);

		/*************************************************************************
		** Desc		: 获取格式化时间字串
		** Param	: [in] szDt 用来保存格式化后的字串
		**			: [in] nDt szDt的大小
		**			: [in] tick 单位秒，0表示当前时间【自格林威治时间1970年01月01日00时00分00秒算起的秒数】
		**			: [in] szFmt 用来格式化的常量字串
		** Return	: 返回值扩展后的大小，是nAlignSize的整倍数，若iCurSize<=0，则返回0
		** Author	: hicker@2017-2-26 13:03:10
		*************************************************************************/
		static char *GetFmtDateTime(char *szDt, int nDt, time_t tick = 0, const char *szFmt = "%Y%m%d-%H:%M:%S");
		static unsigned long long GetCurrentTick(bool isMill = true);
		static char *Md5(void *pData, int nData, char szMd5Val[/*64*/]);
		static int Random(int nMax);
		/*************************************************************************
		** Desc		: 将iCurSize扩展为nAlignSize的整倍数
		** Param	: [in] iCurSize 被检测的大小
		**			: [in] nAlignSize 对其值
		** Return	: 返回值扩展后的大小，是nAlignSize的整倍数，若iCurSize<=0，则返回0
		** Author	: hicker@2017-2-26 13:03:10
		*************************************************************************/
		static int AlignUP(int iCurSize, unsigned int nAlignSize);
	};

}

#endif

