#pragma once
#include "IWDataTypes.h"
#include "IWWindow.h"
#include "IWSettings.h"

#define WINDOW_CLASS_NAME _T("IWWindow")

class IWWindowManager : IIWWndEventListener
{
public:
    IWWindowManager(IWSettings* settings);
    void SetTheme(IWTheme newTheme);
    void SyncWithActiveRunways(std::vector<IWActiveRunway> activeRunways);
    bool Open(std::string approachTitle);
    void HandleLiveData(IWLiveData liveData);
    
    // Destructor
    ~IWWindowManager();

private:
    std::vector<IWWindow*> windows;
    IWSettings* settings;

    // Events from a window
    void OnWindowClosed(IWWindow* window) override;
    void OnWindowMenuOpenNew(std::string approachTitle) override;
    void OnWindowRectangleChanged(IWWindow* window) override;
    void OpenApproachView(IWApproachDefinition approach);
};

