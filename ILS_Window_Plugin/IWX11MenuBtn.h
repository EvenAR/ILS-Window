#pragma once
#include "IWTitleBarBtn.h"

class IWX11MenuBtn : public IWTitleBarBtn
{
    void DrawSymbol(CDC* pDC, CRect rect) override;

    DECLARE_MESSAGE_MAP()

    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

