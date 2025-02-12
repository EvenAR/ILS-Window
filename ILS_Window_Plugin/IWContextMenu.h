#pragma once
#include <afxwin.h>

class IWContextMenu : public CWnd
{
public:
    void SubclassMenu(HWND hMenuWnd);

protected:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};
