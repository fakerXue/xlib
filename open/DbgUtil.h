#ifndef __DBGUTIL_H__
#define __DBGUTIL_H__

#include "../depends/define.h"

namespace xlib
{
	class DbgUtil
	{
	public:
		DbgUtil();
		virtual ~DbgUtil();
		static DbgUtil *GetInstance();
		bool Init(char *szPath, int nLevel = 0);
		void Out(char *szFile, int nLine, char *szFmt, ...);
		void Box(char *szFile, int nLine, char *fmt, ...);
		// szOutFile - only file name, no path
		void OutB(char *szFile, int nLine, char *szOutFile, char *pData, unsigned int dwSize);

	private:
		char m_szFilePath[260];
		char m_szPath[260];
		char *m_pLog;
		char *m_pInfo;
		int m_nLevel;
#ifdef __WIN32__
		int m_mutex;
#elif defined __LINUX__
		pthread_mutex_t m_mutex;
#endif
	};


	

#define DBGINIT								DbgUtil::GetInstance()->Init
#ifdef __WIN32__
#define PRTOUT(fmt,...)						DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,__VA_ARGS__)
#elif defined __LINUX__
#define PRTOUT(fmt,args...)					DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,##args)
#endif


#ifdef __DEBUG__
#ifdef __WIN32__
#define DBGINT		__asm{int 3}
#define DBGOUT(fmt,...)						DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,__VA_ARGS__)
#define MSGBOX(fmt,...)						DbgUtil::GetInstance()->Box(__FILE__,__LINE__,fmt,__VA_ARGS__)
#elif defined __LINUX__
#define DBGOUT(fmt,args...)					DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,##args)
#define MSGBOX(fmt,args...)					DbgUtil::GetInstance()->Box(__FILE__,__LINE__,fmt,##args)
#endif
#define DBGOUTB(szOutFile,pData,dwSize)		DbgUtil::GetInstance()->OutB(__FILE__,__LINE__,szOutFile,pData,dwSize)

#ifdef __WIN32__

#define ENTER()				do{DBGOUT("-> %s \n", __FUNCTION__);} while (0)
#define EXIT()				do{DBGOUT("<- %s \n", __FUNCTION__);return;} while (0)
#define RETURN(_x_)			do{DBGOUT("<- %s return %d\n", __FUNCTION__, _x_);return _x_;} while (0)

#ifndef ASSERT
#define ASSERT(x) {if(!(x)) _asm{int 0x03}}
#endif
#ifndef VERIFY
#define VERIFY(x) {if(!(x)) _asm{int 0x03}} 
#endif

#elif defined __LINUX__ // __WIN32__

#define ENTER()				do{DBGOUT("-> %s::%s \n", __FILE__,__FUNCTION__);} while (0)
#define EXIT()				do{DBGOUT("<- %s::%s \n", __FILE__,__FUNCTION__);return;} while (0)
#define RETURN(_x_)			do{DBGOUT("<- %s::%s return %d\n",__FILE__, __FUNCTION__, _x_);return _x_;} while (0)

#endif // __WIN32__


#else // __DEBUG__

#ifdef __WIN32__
#define DBGOUT(fmt,...)						//DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,__VA_ARGS__)
#define MSGBOX(fmt,...)						//DbgUtil::GetInstance()->Box(__FILE__,__LINE__,fmt,__VA_ARGS__)
#elif defined __LINUX__
#define DBGOUT(fmt,args...)					//DbgUtil::GetInstance()->Out(__FILE__,__LINE__,fmt,##args)
#define MSGBOX(fmt,args...)					//DbgUtil::GetInstance()->Box(__FILE__,__LINE__,fmt,##args)
#endif
#define DBGOUTB(szOutFile,pData,dwSize)		//DbgUtil::GetInstance()->OutB(__FILE__,__LINE__,szOutFile,pData,dwSize)

#ifdef ASSERT
#undef ASSERT
#define ASSERT(x)
#endif
#ifdef VERIFY
#undef VERIFY
#define VERIFY(x)			x
#endif

#define ENTER()		
#define EXIT()		
#define RETURN(x)			return(x)

#endif // __DEBUG__


}

#endif

