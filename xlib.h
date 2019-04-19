#ifndef __XLIB_H__
#define __XLIB_H__

#include "depends/md5.h"
#include "depends/unzip.h"
#include "depends/zip.h"
#include "depends/define.h"
#include "open/open.h"
#ifndef __XLIB_OPEN__
#include "stdapi.h"
#include "xInets.h"
#include "EasyCmd.h"
#include "Infect.h"
#include "OSTool.h"
#include "PETool.h"
#endif

namespace xlib
{
	bool Initialize(char *strDbgPath, int iDbgLevel);
	void UnInitialize();
}

#endif

