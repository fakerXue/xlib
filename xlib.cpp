#include "xlib.h"

namespace xlib
{
	bool Initialize(char *strDbgPath, int iDbgLevel)
	{
#ifdef __WIN32__
#ifndef __XLIB_OPEN__
		if (!WinAPI::Load_Coredll())
		{
			//MessageBox(NULL, "核心库初始化失败！", "严重错误", MB_OK|MB_ICONERROR);
			return false;
		}
#endif
#endif
		if (strDbgPath) DBGINIT(strDbgPath, 1);
		return true;
	}

	void UnInitialize(){};

}
