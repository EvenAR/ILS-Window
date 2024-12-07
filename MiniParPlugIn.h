#include "lib/EuroScopePlugIn.h"
#include "ParWindow.h"
#include "ParDataTypes.h"

#pragma once
class MiniParPlugIn : public EuroScopePlugIn::CPlugIn, IParWindowEventListener {
private:
    std::vector<ParWindow*> windows;

    void OpenNewWindow();
    void RemoveWindowFromList(ParWindow* window);

public:
    MiniParPlugIn();
    void OnWindowClosed(ParWindow* window) override;
    void OnTimer(int seconds) override;

};
