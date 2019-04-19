#include "DatX.h"

using namespace xlib;

const int tagDatX::__c_k = 32; // szKey最大长度
const int tagDatX::__c_g = 1024 * 8; // 8K
const int tagDatX::__c_x = __c_k + sizeof(DWORD); // szKey+nData
const int tagDatX::__c_max = 1024 * 1024 * 512; // 理论上支持最大包512M，但实际传输应视具体情况，一般情况下网络包<64k
const int tagDatX::__c_v = 1024 * 1024 * 20; // 20M
const int tagDatX::__c_min = sizeof(tagDatX::INFO) - sizeof(DWORD); // 减去最后一个成员(x)


tagDatX::tagDatX()
{
	__p = malloc(__c_g);
	memset(__p, 0, __c_g); // 实质上 &__p==this
	INFO *info = (INFO*)__p;
	info->p = (DWORD)__p;
	info->n = __c_min;
	info->e = 1; // 允许扩增
	info->z = __c_g;
	info->c = 0;
}
tagDatX::tagDatX(tagDatX &dat) {tagDatX(); Build(dat.Mem(), dat.Len()); }
tagDatX::tagDatX(const char *szKey, int nAlloc, char *strFmtVal, ...)
{
	tagDatX();
	if (nAlloc > __c_v) return;
	va_list body;
	va_start(body, strFmtVal);
	Put(szKey, strFmtVal, body);
	va_end(body);
}

/*virtual */tagDatX::~tagDatX() // 不能使用virtual
{
	free(__p);
}

tagDatX::tagxty::tagxty() :I(0), F(0), S(NULL){ i = -1; p = 0; n = 0; k[0] = 0; }

tagDatX tagDatX::operator = (tagDatX &dat)
{
	if (this != &dat)
	{
		Build(dat.Mem(), dat.Len());
	}
	return *this;
}

int tagDatX::Cnt(){ return ((INFO*)__p)->c; }
bool tagDatX::Put(const char *szKey, int iVal)
{
	return Put(szKey, __c_v, "%d", iVal);
}

bool tagDatX::Put(const char *szKey, float fVal)
{
	return Put(szKey, __c_v, "%f", fVal);
}

bool tagDatX::Put(const char *szKey, char *szVal){ return Put(szKey, __c_v, szVal); }

bool tagDatX::Put(const char *szKey, int nAlloc, char *strFmtVal, ...)
{
#if 0
	if (nAlloc > __c_v)return false;
	char strVal[__c_v + 1];
	va_list body;
	va_start(body, strFmtVal);
	int n = vsprintf(strVal, strFmtVal, body);
	va_end(body);
	if (n < 0) return false;
	return Put(szKey, strVal, n + 1);
#else
	if (nAlloc > __c_v) return false;
	va_list body;
	va_start(body, strFmtVal);
	bool bRet = Put(szKey, strFmtVal, body);
	va_end(body);
	return bRet;
#endif
}

bool tagDatX::Put(const char *szKey, char *strFmtVal, va_list body)
{
	char *pData = (char*)malloc(__c_v+1);
	int nData = vsnprintf(pData, __c_v, strFmtVal, body);
	pData[nData] = 0;
	bool bRet=Put(szKey, pData, nData+1);
	free(pData);
	return bRet;
}

bool tagDatX::Put(const char *szKey, void *pData, int nData)
{
	Del(szKey);

	INFO *info = (INFO*)__p;
	DWORD dwNewSize = info->n + nData + __c_x;
	if (dwNewSize > __c_max || strlen(szKey) > __c_k)
		return false; // 内存不够或szKey不和规则

	if (dwNewSize > info->z)
	{ // 动态扩展内存
		if (!info->e) return false;
		info->z = (dwNewSize / __c_g + 1 ) * __c_g;
		__p = realloc(__p, info->z); // 会自动释放原内存
		info = (INFO*)__p;
		info->p = (DWORD)__p;
	}

	// 保存新数据
	strcpy((char*)(info->p + info->n), szKey);
	((DWORD*)(info->p + info->n + __c_k))[0] = nData;
	memcpy(&((DWORD*)(info->p + info->n + __c_k))[1], pData, nData);
	info->c++;
	info->n += (__c_x + nData);

	return true;
}

tagDatX::xty tagDatX::Get(const char *szKey){ return (*this)[szKey]; }
tagDatX::xty tagDatX::Get(DWORD idx){ return (*this)[idx]; }

bool tagDatX::Del(const char *szKey)
{
	INFO *info = (INFO*)__p;

	xty x1 = (*this)[szKey];
	if (!x1.p)return false;

	xty x2 = (*this)[x1.i + 1];
	x2.p ? memmove((void*)((DWORD)x1.p - __c_x), (void*)((DWORD)x2.p - __c_x), info->n - ((DWORD)x2.p - (__c_x + (DWORD)__p))) : NULL;
	info->c--;
	info->n -= (__c_x + x1.n);

	return true;
}

bool tagDatX::Build(void *pData, int nData)
{
	__p = realloc(__p, nData);
	memcpy(__p, pData, nData);
	if (!__p || NULL == DatX::ToDatX(__p, nData))
	{
		DatX dat;
		Build(dat.Mem(), dat.Len());
		return false;
	}
	((INFO*)__p)->e = 1; // 允许扩增
	return true;
}

tagDatX::xty tagDatX::operator[](DWORD i)
{ // 无需考虑 i<0的情况，DWORD会把-1转为最大值
	INFO *info = (INFO*)__p;
	xty x;
	XTY *p = (XTY*)&info->x;
	for (DWORD j = 0; j < info->c && i < info->c/* && i>-1*/; j++)
	{
		if (i == j)
		{
			x.p = (DWORD)&p->d;
			x.n = p->n;
			x.i = i;
			strncpy(x.k, p->k, tagDatX::__c_k);
			if (x.n > 0)
			{
				char strTemp[32];
				strncpy(strTemp, (char*)x.p, sizeof(strTemp));
				strTemp[31] = 0;
				x.I = atoi(strTemp);
				x.F = (float)atof(strTemp);
				x.S = (char*)x.p;
			}
			return x;
		}
		p = (XTY*)((DWORD)&p->d + p->n);
	}

	return x;
}

tagDatX::xty tagDatX::operator[](const char *szKey)
{
	INFO *info = (INFO*)__p;
	for (DWORD i = 0; i < info->c; i++)
	{
		xty x = (*this)[i];
		if (strncmp(szKey, x.k, __c_k) == 0)
			return x;
	}
	return xty();
}

void tagDatX::Print(tagDatX *pDat)
{
	for (int i = 0; i < pDat->Cnt(); i++)
	{
#if 0
		DBGOUT("i=%02d,pDat[i]->k=%s,pDat[i]->i=%d,pDat[i]->p=0x%08X,pDat[i]->n=%d,pDat[i]->I=%d,pDat[i]->F=%f,pDat[i]->S=%s\n",
			i, pDat->Get(i).k, pDat->Get(i).i, pDat->Get(i).p, pDat->Get(i).n, pDat->Get(i).I, pDat->Get(i).F, pDat->Get(i).S);
#else
		printf("pDat[i]->k=%s,pDat[i]->i=%d,pDat[i]->p=0x%08X,pDat[i]->n=%d\n",
			pDat->Get(i).k, pDat->Get(i).i, pDat->Get(i).p, pDat->Get(i).n);
#endif
	}
}

int tagDatX::ToString(char *pData, int nData, int iDeep)
{
	int iRet = 0;
	iDeep = (iDeep > 1 ? iDeep : 1);
	memset(pData, 0, nData);
	for (int i = 0, j = 0; i < Cnt() && j>-1; i++)
	{
		xty x = Get(i);
		if (nData - x.n < 2)
			break;

		if (IsValid((void*)x.p, x.n))
		{
			memset(pData, '*', iDeep);
			j = sprintf(pData, "%s%s\r\n", pData, x.k);
			int n = ToString(pData + j, nData - j, iDeep + 1);
			pData += (iDeep + n);
			nData -= (iDeep + n);
			iRet += (iDeep + n);
		}
		else
		{
			j = sprintf(pData, "%s%s=%s\r\n", pData, x.k, x.S);
		}
		pData += j;
		nData -= j;
		iRet += j;
	}
	return iRet;
}

void tagDatX::FromString(char *szData)
{
	
}

bool tagDatX::IsValid(void *pData, int nData)
{
	INFO *info = (INFO*)pData;
	DWORD old = info->p;
	info->p = (DWORD)pData;
	bool bRet = true;
	// info->c * __c_k + __c_min > info->n 使用乘法容易溢出，应使用除法 (__c_max-__c_min) / info->c < __c_k
	if (info->n < __c_min || info->n > info->z || info->z > __c_max || (info->c && (__c_max - __c_min) / info->c < __c_k))
	{
		bRet = false;
	}
	tagDatX *pDat = (tagDatX*)pData;
	int sum = __c_min;
	for (int i = 0; i < (int)info->c && bRet && nData == pDat->Len(); i++)
	{
		xty x = pDat->Get(i);
		sum += __c_x + x.n;
		if (!x.p || !x.k[0] || x.k[__c_k-1] || sum > pDat->Len())
		{
			bRet = false;
		}
	}
	bRet &= (sum == pDat->Len());
	info->p = old;
	return bRet;
}

// 应该判断返回值，是否和传入的地址是否相同，不相同则表明是新内存，而原内存会被释放
tagDatX *tagDatX::ToDatX(void *pData, int nData, int nZ)
{
	if (DatX::IsValid(pData, nData))
	{
		INFO *info = (INFO*)pData;
		info->p = (DWORD)pData;
		info->e = 0; // 不允许扩增
		info->z = ((DWORD)nZ > info->n ? nZ : info->n);
		return (tagDatX*)pData;
	}

	return NULL;
}

tagDatX *tagDatX::ToDatX(tagDatX *pDat, int nZ)
{
	return ToDatX(pDat->Mem(), pDat->Len(), nZ);
}

void *tagDatX::Mem()const{ return (void*)((INFO*)__p)->p; }
int tagDatX::Len()const{ return ((INFO*)__p)->n; }


