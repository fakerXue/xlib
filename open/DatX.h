
/*************************************************************************
** Desc		: 为避免使用json和xml做网络传输，实现一种简单key-val数据类型，
**				内存结构如下： {[p,4][n,4][xCnt,4][(32+4+data)|xty,(32+4+data)|xty,...]}
**				cnt_seg: 数据个数段，iCnt,xCnt
**				int_key_seg: int/float型key
**				xty_key_seg: xty型key
**				int_dat: int型数据区
**				xty: xty区
**				xty_dat: xty.p的数据区
**				【注意】DatX会自动将int型key/val保存在xty型key/val前
**				【注意】DatX不能含有virtual成员，包括函数成员，且只能含有一个成员变量__p
** Author	: hicker@2017-3-19 11:11:57
*************************************************************************/

#ifndef __DATX_H__
#define __DATX_H__

#include "../depends/define.h"

namespace xlib
{

	// 思考两点：1.如何把DataPack的头尾[伪协议头尾]实现；2.如何实现指针赋值后依然函数或成员可调用成功
	typedef struct tagDatX
	{
		// 头结构
		struct INFO
		{
			DWORD p; // 保存 DatX 在内存中地址；当以对象创建DatX时，被初始化为(DWORD)this
			DWORD n; // 保存 DatX 中有效数据的大小
			DWORD e; // 是否允许扩增[Enlarge]，默认为允许，只有通过ToDatX构建时，此值会被强制设为否
			DWORD z; // 保存 DatX 的实际容量，会动态增长，但不会动态缩减，正常情况 z>=n 且 z<=__c_max
			DWORD c; // 保存 DatX 所含的 key-val 个数
			DWORD x; // 代表 DatX 中 xty 头；故 &x 即表示 xty 头的起点，必须发在最后一位
		};

		struct XTY
		{
			char k[32]; // 保存 xty 的 szKey
			DWORD n;	// 保存 xty 的 nData
			DWORD d;	// 代表 xty 的 pData；故 &d 即表示 xty 的数据起点
		};

		tagDatX();
		tagDatX(tagDatX &dat);
		tagDatX(const char *szKey, int nAlloc, char *strFmtVal, ...);

		/*virtual */~tagDatX(); // 不能使用virtual

		typedef struct tagxty
		{
			tagxty();
			char k[32]; // 不能包含 '\r'、'\n'、' '
			DWORD n;
			DWORD p;
			int i;
			int I;
			float F;
			char *S;
			friend tagDatX;
		}xty;

		tagDatX operator = (tagDatX &dat);

		int Cnt();
		bool Put(const char *szKey, int iVal);
		bool Put(const char *szKey, float fVal);
		bool Put(const char *szKey, char *szVal);
		bool Put(const char *szKey, int nAlloc, char *strFmtVal, ...);
		bool Put(const char *szKey, void *pData, int nData);
		xty Get(const char *szKey);
		xty Get(DWORD idx);
		bool Del(const char *szKey);

		bool Build(void *pData, int nData);

		xty operator[](DWORD i);

		xty operator[](const char *szKey);

		static void Print(tagDatX *pDat);
		int ToString(char *pData, int nData, int iDeep = 1);
		void FromString(char *szData);

		static bool IsValid(void *pData, int nData);

		// 必须通过此函数转换void*为DatX*
		static tagDatX *ToDatX(void *pData, int nData, int nZ = 0);
		static tagDatX *ToDatX(tagDatX *pDat, int nZ = 0);

		void *Mem()const;
		int Len()const;

	private:
		bool Put(const char *szKey, char *strFmtVal, va_list body);

	public:
		static const int __c_k; // szKey最大长度
		static const int __c_g; // 内存颗粒大小
		static const int __c_x; // szKey+nData
		static const int __c_max;
		static const int __c_v;
		static const int __c_min;

		void *__p; // 唯一成员变量，符合 &__p==this
	}DatX;

}

#endif

