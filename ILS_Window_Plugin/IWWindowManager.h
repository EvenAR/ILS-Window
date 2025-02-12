#pragma once
#include "IWDataTypes.h"
#include "IWWindow.h"
#include "IWSettings.h"

#define WINDOW_CLASS_NAME _T("IWWindow")

class IWWindowManager : IIWWndEventListener
{
public:
    IWWindowManager(IWSettings* settings);
    void SyncWithActiveRunways(std::vector<IWActiveRunway> activeRunways);
    bool Open(std::string approachTitle);
    void HandleLiveData(IWLiveData liveData);
    
    // Destructor
    ~IWWindowManager();

private:
    std::vector<IWWindow*> windows;
    IWSettings* settings;

    void OpenApproachView(IWApproachDefinition approach);
    void OpenApproachViewWithTheme(IWApproachDefinition approach, IWTheme theme);

    // Callbacks from a window
    void OnWindowClosed(IWWindow* window) override;
    void OnWindowMenuOpenNew(std::string approachTitle) override;
    void OnWindowRectangleChanged(IWWindow* window) override;
    void OnToggleThemeClicked(IWWindow* window) override;

    // Utils 
    IWApproachDefinition LookupApproach(std::string approachName);
};

