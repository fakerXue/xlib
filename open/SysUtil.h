#ifndef __SYSUTIL_H__
#define __SYSUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	
	class SysUtil
	{
	public:
		// ���ຯ���������ܼ�static
		static void RedirectIOToConsole();
		static int ReadLine(char buff[], bool mode = 0);

		/*************************************************************************
		** Desc		: ��ȡ��ʽ��ʱ���ִ�
		** Param	: [in] szDt ���������ʽ������ִ�
		**			: [in] nDt szDt�Ĵ�С
		**			: [in] tick ��λ�룬0��ʾ��ǰʱ�䡾�Ը�������ʱ��1970��01��01��00ʱ00��00�������������
		**			: [in] szFmt ������ʽ���ĳ����ִ�
		** Return	: ����ֵ��չ��Ĵ�С����nAlignSize������������iCurSize<=0���򷵻�0
		** Author	: hicker@2017-2-26 13:03:10
		*************************************************************************/
		static char *GetFmtDateTime(char *szDt, int nDt, time_t tick = 0, const char *szFmt = "%Y%m%d-%H:%M:%S");
		static unsigned long long GetCurrentTick(bool isMill = true);
		static char *Md5(void *pData, int nData, char szMd5Val[/*64*/]);
		static int Random(int nMax);
		/*************************************************************************
		** Desc		: ��iCurSize��չΪnAlignSize��������
		** Param	: [in] iCurSize �����Ĵ�С
		**			: [in] nAlignSize ����ֵ
		** Return	: ����ֵ��չ��Ĵ�С����nAlignSize������������iCurSize<=0���򷵻�0
		** Author	: hicker@2017-2-26 13:03:10
		*************************************************************************/
		static int AlignUP(int iCurSize, unsigned int nAlignSize);
	};

}

#endif

