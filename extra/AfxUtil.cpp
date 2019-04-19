#include "stdafx.h"
#include "AfxUtil.h"

void AfxUtil::PrintText(CEdit *edit, bool isClear, const TCHAR *fmtText, ...)
{
	va_list args;
	char buff[1024] = { 0 };

	va_start(args, fmtText);
	vsprintf(buff, fmtText, args);
	va_end(args);

	if (isClear)
	{
		edit->SetSel(0, -1);
		edit->Clear();
	}
	//TCHAR text[4096]={0};
	edit->SetSel(edit->GetWindowTextLength(), edit->GetWindowTextLength());

	edit->ReplaceSel(buff);
}
