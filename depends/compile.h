#ifndef __COMPILE_H__
#define __COMPILE_H__

// 使用xlib.lib前先编译，可编译两种模式 __WIN32__[__DEBUG__] 或 __LINUX__[__DEBUG__]
// 除此两种宏配置外，其他不推荐使用，因为不成熟

// 可配置选项
// __WIN32__. __LINUX__, __DEBUG__, __STD_EN__, __MFC_EN__, __API_EN__, __XLIB_OPEN__, NO_WARN_STDAPI_DEPRECATED_XLIB


/* Win32或Linux使用xlib方法示例，以下为MyProject中的基础头文件 base.h
#define __WIN32__
#define __DEBUG__
#define __STD_EN__
#include "xlib.h"
#pragma comment(lib,"xlib.lib")
*/

#if defined(_WIN32) && !defined(__WIN32__)
#define __WIN32__
#endif

#if defined(_LINUX) && !defined(__LINUX__)
#define __LINUX__
#endif

#if !defined(__WIN32__) && !defined(__LINUX__)
#error You must define one of __WIN32__ or __LINUX__
#endif

#if defined(_DEBUG) && !defined(__DEBUG__)
#define __DEBUG__
#endif

#if !defined(NO_WARN_STDAPI_DEPRECATED_XLIB) && !defined(__API_EN__) && defined(__WIN32__)
#pragma warning(push)
#pragma warning(1 : 4996)
inline __declspec(deprecated("maybe you should define __API_EN__ for enable stdapi")) void StdApi_Disabled_Deprecated_In_xlib() { }
class StdAPI_Deprecated_xlib{ public:StdAPI_Deprecated_xlib() { StdApi_Disabled_Deprecated_In_xlib(); } };
#pragma warning(pop)
#endif

// 以下为VS配置仅做参考
//kernel32.lib;user32.lib;msvcrt.lib;ws2_32.lib;version.lib;%(AdditionalDependencies)
//kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)

// 不知道为什么以下代码不起作用
//写Win32程序的话，windows.h 是必须的，使用动态连接必要的系统库，可以把link下的链接库全部删除，需要就练级。
//#pragma comment(lib,"kernel32.lib") 
//#pragma comment(lib,"user32.lib")
//#pragma comment(lib,"winspool.lib")
//#pragma comment(lib,"advapi32.lib")
//#pragma comment(lib,"ws2_32.lib")
//#pragma comment(lib,"dbghelp.lib")//ImageRvaToVa  #include <Dbghelp.h> 
//#pragma comment(lib,"version.lib")
////这句是用来动态连接VC库的，使用VC6.0的可以加上，但是使用VS2008以后的就不要加了。我是用VS2010的时候，编译后需要 msvcr100.dll 文件。VS2010一旦静态链接了msvcrt.lib，体积立马上到28，如果复杂点的程序可能会更大。如个仅仅是一个HelloWorld，一个MessageBox的话，因为不需要用到msvcrt.lib，所以体积不会增加的，这个时候加不加这句话都无所谓。
//#pragma comment(lib,"msvcrt.lib")
//#pragma comment(lib,"msvcp100.lib")

//#define WIN32_LEAN_AND_MEAN	// to speed up the link speed and decreasing the exectable file size
//#pragma comment ( linker,"/ALIGN:4096" )	// ??????????????

//这句是用来说明是win32程序，不是控制台的程序，控制台的程序会有个黑窗口。可以再程序里写，也可以再配置里写。
//#pragma comment(linker, "/SUBSYSTEM:windows")

//这个用来更改入口函数，对vs2010编译的程序，效果很明显。一旦指定了入口函数，体积真的是立马下来了。
//#pragma comment(linker, "/ENTRY:WinMain")
//#pragma comment(linker, "/ENTRY:DllMain")

//对齐方式，这个我用VS2010测试的时候，修改这个，编译后的程序都提示不是有效的Win32程序。VC6.0上没问题。VS2010编译的同志就不要改了。VC6.0的可以修改测试，数值还可以减少。
//#pragma comment (linker, "/ALIGN:512")

//合并区段。我认为是没什么必要，编译的时候会有警告。VS2010编译后可能会出现程序不能正常运行，VC6.0的正常，不推荐。
//#pragma comment(linker, "/merge:.data=.text") 
//#pragma comment(linker, "/merge:.rdata=.text")

//不对Windows 98系统做优化。VS2010已经取消了这个选项，如果加上这句话，编译的时候会出现警告。VC6.0加着句话，也能减少不少的体积。VC6.0上，推荐使用。
//#pragma comment(linker, "/OPT:NOWIN98")

//OPtimization        Minimize Size(/O1)
//C/C++选项里的，最小体积编译，效果不明显。

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  

// 禁用一些优化，便于调试
//#pragma comment(compiler, "/Zi")  
//#pragma comment(compiler, "/GL")  

#ifdef __WIN32__
// 如果再VS属性页设置了“忽略所有默认库”，则一下语句将不会被编译
// 建议保持属性页 忽略所有默认库：否
#pragma comment(lib,"version.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"gdi32.lib")
//#pragma comment(lib,"libcmtd.lib")
//#pragma comment(lib,"libcmt.lib")
#pragma comment(linker,"/NODEFAULTLIB:libcmt.lib")
#pragma comment(linker,"/NODEFAULTLIB:libcmtd.lib")
#pragma comment(linker,"/NODEFAULTLIB:msvcrt.lib")
#pragma comment(linker,"/NODEFAULTLIB:msvcrtd.lib")
#pragma warning(disable:4996)
#pragma warning(disable:4995)
#endif

#endif

