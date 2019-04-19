/*************************************************************************
** Desc		: 便于跨平台的一些宏
** Author	: hicker@2016-11-18 20:49:54
*************************************************************************/

#ifndef __DEFINE_H__
#define __DEFINE_H__

#include "compile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>  
#include <ctype.h>  
#include <time.h>  
#include <math.h>
#include <malloc.h>

#ifdef __WIN32__
//#include <lmerr.h>	// ????????????
//#include <winsdkver.h>
//#include <SDKDDKVer.h>
#include <Windows.h>
#include <winternl.h>
#include <winsock.h> 
#include <intrin.h>
#include <Dbghelp.h>
#include <tlhelp32.h>
#include <process.h>
#include <direct.h>  
//#include <tlhelp32.h>  
//#include <shellapi.h>
#include <tchar.h>
#include <io.h>
#include <strsafe.h>
#include <stdarg.h>
#include <direct.h>  
#include <sys/timeb.h> 
#include <fcntl.h>
#include <conio.h> // _kbhit _getch _putch
#elif defined __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/procfs.h>  
#include <sys/stat.h>  
#include <sys/time.h>  
#include <sys/vfs.h>  
#include <sys/shm.h>
#include <sys/sem.h>//包含信号量定义的头文件  
#include <sys/stat.h>
#include <alsa/iatomic.h>  // apt-get install libasound2-dev
#include <arpa/inet.h>  
#include <netinet/ip_icmp.h>  
#include <netinet/in.h>  // socaddr_in
#include <netinet/ip.h>  
#include <netinet/tcp.h>  
#include <dirent.h>  
#include <errno.h>  
#include <sys/ioctl.h> // ioctl
#include <fcntl.h> // ioctl
#include <pthread.h>
#include <unistd.h> //slee
#include <netdb.h>  
#include <sched.h>  
#include <signal.h>  
#include <stropts.h>  
#include <stdarg.h>  
#endif

#ifdef __STD_EN__
#include <string>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <iostream>
using namespace std;
#endif

// window type to linux
#ifdef __LINUX__
typedef void				*HANDLE;
typedef void				*HMODULE;
typedef unsigned int		SOCKET;

typedef unsigned long       DWORD;
typedef unsigned int        UINT;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
//typedef void far            *LPVOID;
typedef void	            *LPVOID;
typedef unsigned int		WPARAM;
typedef long				LPARAM;

#define WINAPI
#ifndef NULL
#define NULL				0
#endif
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef MAX_PATH
#define MAX_PATH			260
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR		-1
#endif
#endif

// bRet init as TRUE; if bRet==0, then do not exec expr[true]
#define IF0BRET(bRet,expr)			(bRet= (bRet && (expr)))
// iRet init as 0; if iRet!=0, then do not exec expr[true], iRet saved the err
#define IF0IRET(err,iRet,expr)		((iRet==0)&&(iRet=((expr)?0:err)))
//#define IF0IRET(err,iRet,expr)		(iRet=(((iRet==0)&&(expr))?0:err))

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

#define SWAP32(x)	((x&0x000000ff) << 24 | (x&0x0000ff00) << 8 | (x&0x00ff0000) >> 8 | (x&0xff000000) >> 24)
#define SWAP16(x)	((x&0x00ff) << 8 | (x&0xff00) >> 8)

// strIP fmt "%03d.%03d.%03d.%03d", max "255.255.255.255"
#define IP2NET(strIP)			(((short)0x0102)==(short)(0x02<<8|0x01))\
	?(((100*(strIP[0]-'0')+10*(strIP[1]-'0')+(strIP[2]-'0'))<<24)|((100*(strIP[4]-'0')+10*(strIP[5]-'0')+(strIP[6]-'0'))<<16)|((100*(strIP[8]-'0')+10*(strIP[9]-'0')+(strIP[10]-'0'))<<8)|(100*(strIP[12]-'0')+10*(strIP[13]-'0')+(strIP[14]-'0'))) \
	:(((100*(strIP[0]-'0')+10*(strIP[1]-'0')+(strIP[2]-'0'))<<0)|((100*(strIP[4]-'0')+10*(strIP[5]-'0')+(strIP[6]-'0'))<<8)|((100*(strIP[8]-'0')+10*(strIP[9]-'0')+(strIP[10]-'0'))<<16)|((100*(strIP[12]-'0')+10*(strIP[13]-'0')+(strIP[14]-'0'))<<24))

// strPort fmt "%05d", max 65536
#define PORT2NET(strPort)		(((short)0x0102)==(short)(0x02<<8|0x01))\
	?((strPort[0] - '0') * 10000 + (strPort[1] - '0') * 1000 + (strPort[2] - '0') * 100 + (strPort[3] - '0') * 10 + (strPort[4] - '0'))\
	:SWAP16((strPort[0] - '0') * 10000 + (strPort[1] - '0') * 1000 + (strPort[2] - '0') * 100 + (strPort[3] - '0') * 10 + (strPort[4] - '0'))

// etc. printf("%d.%d.%d.%d",IP2FMT(nAddrIP));
#define IP2FMT(nAddrIP)	(((short)0x0102)==(short)(0x02<<8|0x01))\
	?(((int)nAddrIP) & 0xFF000000) >> 24, (((int)nAddrIP) & 0x00FF0000) >> 16, (((int)nAddrIP) & 0x0000FF00) >> 8, (((int)nAddrIP) & 0x000000FF) \
	:(((int)nAddrIP) & 0x000000FF), (((int)nAddrIP) & 0x0000FF00) >> 8, (((int)nAddrIP) & 0x00FF0000) >> 16, (((int)nAddrIP) & 0xFF000000) >> 24

// etc. printf("%d",PORT2FMT(nAddrPort));
#define PORT2FMT(nAddrPort)	(((short)0x0102)==(short)(0x02<<8|0x01))\
	?(0xFFFF&nAddrPort):SWAP16(0xFFFF&nAddrPort)

// etc. printf("%d.%d.%d.%d:%d",ADDRFMT(addr));
#ifdef __WIN32__
#define ADDRFMT(addr)	IP2FMT(addr.sin_addr.S_un.S_addr),PORT2FMT(addr.sin_port)
#define ADDRIP(addr)	addr.sin_addr.S_un.S_addr
#elif defined __LINUX__
#define ADDRFMT(addr)	IP2FMT(addr.sin_addr.s_addr),PORT2FMT(addr.sin_port)
#define ADDRIP(addr)	addr.sin_addr.s_addr
#endif
#define ADDRPORT(addr)	addr.sin_port

#endif
