/*************************************************************************
** Copyright(c) 2016-2020  hicker
** All rights reserved.
** Name		: xUtils.h
** Desc		: һЩ�Ƚ��ӵĹ��ߺ���
** Author	: hicker@2017-5-27 10:27:25
*************************************************************************/

#ifndef __XUTILS_H__
#define __XUTILS_H__

#include "../depends/define.h"

// ��дʱ��ѭ��MFC����������STD�ʹ�C������STD�����ô�C��������������

namespace xlib
{
	namespace xUtils
	{
		// ȡ������ַ
		template<typename dst_type, typename src_type>
		dst_type union_cast(src_type src)
		{
			union
			{
				src_type s;
				dst_type d;
			}u;
			u.s = src;
			return u.d;
		}

#define asm_cast(var,addr)		\
			{                       \
	__asm{						\
	mov var, offset addr		\
			}                   \
			}

		//int __GetAddr(int iNul, ...)
		//{
		//	va_list body;
		//	va_start(body, iNul);
		//	int iRet = va_arg(body, int);
		//	va_end(body);
		//	return iRet;
		//}
		//template<class T>
		//int GetAddr(T t)
		//{
		//	return __GetAddr(t);
		//}


	}
}

#endif


