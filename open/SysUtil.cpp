
#include "SysUtil.h"
#include "../depends/md5.h"

using namespace xlib;


/*************************************************************************
** Desc		: ����
** Author	: hicker@2016-11-18 20:51:07
**************************************************************************/
#ifdef __STD_EN__
void SysUtil::RedirectIOToConsole()
{
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;

	// ����һ������̨����  
	AllocConsole();

	// �����㹻�����Ļ������������ǻ�����\E6\9C?
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);

	// ���ÿ���̨��Ļ�ĸ߶�
	coninfo.dwSize.Y = 500;

	// ���ÿ���̨��Ļ�����\E5\B0?
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// ��ȡ��׼������
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);

	//�򿪱�׼�����������ƴ��ļ��ķ�ʽ��fopen,����һ���ļ�������
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	// �Կ�д�ķ�ʽ��
	fp = _fdopen(hConHandle, "w");

	*stdout = *fp;

	setvbuf(stdout, NULL, _IONBF, 0);

	// redirect unbuffered STDIN to the console  
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "r");
	*stdin = *fp;
	setvbuf(stdin, NULL, _IONBF, 0);

	// redirect unbuffered STDERR to the console  
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

	fp = _fdopen(hConHandle, "w");
	*stderr = *fp;
	setvbuf(stderr, NULL, _IONBF, 0);

	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog  
	// point to console as well  
	std::ios::sync_with_stdio();
}
#endif

int SysUtil::ReadLine(char buff[], bool mode)
{
	// ����ģʽ
	if (0 == mode)
	{
		int len = 0;
		char ch = getchar();
		while (ch != '\n' && ch != EOF)
			buff[len++] = ch;
		buff[len] = '\0';
		return len;
	}

	// ������ģʽ
	static char strInput[4096];
	static int idx = 0;
	if (idx > sizeof(strInput))
		return -1;

	int iRet = 0;
	//fflush((&__iob_func()[0]));
	if (!kbhit())
	{
#ifdef __WIN32__
		Sleep(50);
#elif defined __LINUX__
		usleep(50 * 1000);
#endif
		return 0;
	}
	// ���⣺winϵͳ����ͨ��ֱ��*stdin=*fp�����ض���stdin
	char ch = getch();
#ifdef __WIN32__
	putch(ch == '\r' ? '\n' : ch);
#endif
	if (ch == 8)
	{ // �˸��
#ifdef __WIN32__
		putch(' ');
		putch(8);
#elif defined __LINUX__ // hicker@2018-6-3 00:38:37 coding ��Ч��δ���linux��ⲻ���˸��������
		printf("\b \b");
#endif
		strInput[idx>0 ? (--idx) : 0] = 0;
	}
	else
	{
		strInput[idx++] = ch;
		strInput[idx] = 0;
		if (ch == '\n' || ch == '\r' || ch == EOF)
		{
			strInput[--idx] = 0;
			iRet = idx;
			idx = 0;
			strcpy(buff, strInput);
		}
	}
	return iRet;
}

char *SysUtil::GetFmtDateTime(char *szDt, int nDt, time_t tick, const char *szFmt)
{
	tick = (0 == tick ? time(NULL) : tick);
	strftime(szDt, nDt, szFmt, localtime(&tick));
	return szDt;
}

unsigned long long SysUtil::GetCurrentTick(bool isMill)
{
	unsigned long long tick;
#ifdef __WIN32__
	struct __timeb64 tp;
	_ftime64(&tp);
	tick = ((unsigned long long)time(NULL) * 1000 + tp.millitm);
#elif defined __LINUX__
	struct timeval tmv;
	gettimeofday(&tmv, NULL);
	tick = (unsigned long long)time(NULL) * 1000 + tmv.tv_usec / 1000;
#endif 
	return (isMill ? tick : (tick/1000));
}

char *SysUtil::Md5(void *pData, int nData, char szMd5Val[/*64*/])
{
	MD5_CTX mdContext;
	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char*)pData, nData);
	MD5Final(&mdContext);

	szMd5Val[0] = 0;
	for (int i = 0; i < sizeof(mdContext.digest); i++)
	{
		sprintf(szMd5Val, "%s%02x", szMd5Val, mdContext.digest[i]);
	}

	return szMd5Val;
}

int SysUtil::Random(int nMax)
{
	static int i = 0;
	if (GetCurrentTick() - i > 1000 * 60)
	{
		i = (int)GetCurrentTick();
		srand(i);
	}
	return rand() % nMax;
}

int SysUtil::AlignUP(int iCurSize, unsigned int nAlignSize)
{
	if (iCurSize <= 0) return 0;
	if (iCurSize%nAlignSize == 0)return iCurSize;
	return (iCurSize / nAlignSize + 1)*nAlignSize;
}

