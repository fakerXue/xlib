
/*************************************************************************
** Desc		: Ϊ����ʹ��json��xml�����紫�䣬ʵ��һ�ּ�key-val�������ͣ�
**				�ڴ�ṹ���£� {[p,4][n,4][xCnt,4][(32+4+data)|xty,(32+4+data)|xty,...]}
**				cnt_seg: ���ݸ����Σ�iCnt,xCnt
**				int_key_seg: int/float��key
**				xty_key_seg: xty��key
**				int_dat: int��������
**				xty: xty��
**				xty_dat: xty.p��������
**				��ע�⡿DatX���Զ���int��key/val������xty��key/valǰ
**				��ע�⡿DatX���ܺ���virtual��Ա������������Ա����ֻ�ܺ���һ����Ա����__p
** Author	: hicker@2017-3-19 11:11:57
*************************************************************************/

#ifndef __DATX_H__
#define __DATX_H__

#include "../depends/define.h"

namespace xlib
{

	// ˼�����㣺1.��ΰ�DataPack��ͷβ[αЭ��ͷβ]ʵ�֣�2.���ʵ��ָ�븳ֵ����Ȼ�������Ա�ɵ��óɹ�
	typedef struct tagDatX
	{
		// ͷ�ṹ
		struct INFO
		{
			DWORD p; // ���� DatX ���ڴ��е�ַ�����Զ��󴴽�DatXʱ������ʼ��Ϊ(DWORD)this
			DWORD n; // ���� DatX ����Ч���ݵĴ�С
			DWORD e; // �Ƿ���������[Enlarge]��Ĭ��Ϊ����ֻ��ͨ��ToDatX����ʱ����ֵ�ᱻǿ����Ϊ��
			DWORD z; // ���� DatX ��ʵ���������ᶯ̬�����������ᶯ̬������������� z>=n �� z<=__c_max
			DWORD c; // ���� DatX ������ key-val ����
			DWORD x; // ���� DatX �� xty ͷ���� &x ����ʾ xty ͷ����㣬���뷢�����һλ
		};

		struct XTY
		{
			char k[32]; // ���� xty �� szKey
			DWORD n;	// ���� xty �� nData
			DWORD d;	// ���� xty �� pData���� &d ����ʾ xty ���������
		};

		tagDatX();
		tagDatX(tagDatX &dat);
		tagDatX(const char *szKey, int nAlloc, char *strFmtVal, ...);

		/*virtual */~tagDatX(); // ����ʹ��virtual

		typedef struct tagxty
		{
			tagxty();
			char k[32]; // ���ܰ��� '\r'��'\n'��' '
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

		// ����ͨ���˺���ת��void*ΪDatX*
		static tagDatX *ToDatX(void *pData, int nData, int nZ = 0);
		static tagDatX *ToDatX(tagDatX *pDat, int nZ = 0);

		void *Mem()const;
		int Len()const;

	private:
		bool Put(const char *szKey, char *strFmtVal, va_list body);

	public:
		static const int __c_k; // szKey��󳤶�
		static const int __c_g; // �ڴ������С
		static const int __c_x; // szKey+nData
		static const int __c_max;
		static const int __c_v;
		static const int __c_min;

		void *__p; // Ψһ��Ա���������� &__p==this
	}DatX;

}

#endif

