#pragma once
#include <afxwin.h>

class IWTitleBarBtn : public CButton {

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    virtual BOOL IWTitleBarBtn::OnEraseBkgnd(CDC* pDC);

protected:
    DECLARE_MESSAGE_MAP()

    virtual void DrawSymbol(CDC* pDC, CRect rect) = 0;

};

