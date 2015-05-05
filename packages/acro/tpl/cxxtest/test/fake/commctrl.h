#ifndef __FAKE__COMMCTRL_H__
#define __FAKE__COMMCTRL_H__

#include <windows.h>

#define PROGRESS_CLASS TEXT("PROGRESS_CLASS")
#define STATUSCLASSNAME TEXT("STATUSCLASSNAME")

enum { PBM_SETRANGE32, PBM_SETRANGE, PBM_SETPOS, PBM_SETSTEP, PBM_STEPIT, PBM_SETBARCOLOR,
       SB_SETTEXTA, SB_SETPARTS, BS_AUTOCHECKBOX, BM_SETCHECK, BST_UNCHECKED, BM_GETCHECK,
       BST_CHECKED, PBS_SMOOTH };

#define ICC_BAR_CLASSES 1
#define ICC_PROGRESS_CLASS 2

struct INITCOMMONCONTROLSEX
{
    DWORD dwSize;
    DWORD dwICC;
};

inline void InitCommonControls() {}
inline int InitCommonControlsEx(INITCOMMONCONTROLSEX *) { return 0; }

#endif // __FAKE__COMMCTRL_H__