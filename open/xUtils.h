/*************************************************************************
** Copyright(c) 2016-2020  hicker
** All rights reserved.
** Name		: xUtils.h
** Desc		: 一些比较杂的工具函数
** Author	: hicker@2017-5-27 10:27:25
*************************************************************************/

#ifndef __XUTILS_H__
#define __XUTILS_H__

#include "../depends/define.h"

// 编写时遵循，MFC函数可饮用STD和纯C函数，STD可引用纯C函数，但反向不行

namespace xlib
{
	namespace xUtils
	{
		// 取函数地址
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


