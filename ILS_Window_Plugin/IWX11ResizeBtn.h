#pragma once
#include "IWX11TitleBarBtnBase.h"

class IWX11ResizeBtn : public IWX11TitleBarBtnBase
{
    using IWX11TitleBarBtnBase::IWX11TitleBarBtnBase;

private:
    void DrawIcon(CDC* pDC, CRect rect) override;
};
