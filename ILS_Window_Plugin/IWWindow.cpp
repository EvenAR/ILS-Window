#include "pch.h"
#include "IWWindow.h"
#include <cmath>

#define MAX_PROCEDURES 100

#define MENU_ITEM_FLIP                      10000
#define MENU_ITEM_SHOW_LABELS               10001
#define MENU_ITEM_CORRECT_FOR_TEMPERATURE   10002
#define MENU_ITEM_PROCEDURES_SEL_START      20000
#define MENU_ITEM_PROCEDURES_NEW_START      30000

BEGIN_MESSAGE_MAP(IWWindow, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_MOVE()
    ON_WM_SIZING()
    ON_MESSAGE(WM_UPDATE_DATA, &IWWindow::OnUpdateData)
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_NCACTIVATE()
    ON_WM_GETMINMAXINFO()
    ON_MESSAGE(WM_EXITSIZEMOVE, &IWWindow::OnExitSizeMove)
    ON_COMMAND_EX(MENU_ITEM_FLIP, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_EX(MENU_ITEM_SHOW_LABELS, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_EX(MENU_ITEM_CORRECT_FOR_TEMPERATURE, &IWWindow::OnMenuOptionSelected)
    ON_COMMAND_RANGE(MENU_ITEM_PROCEDURES_SEL_START, MENU_ITEM_PROCEDURES_SEL_START + MAX_PROCEDURES, &IWWindow::OnProcedureSelected)
    ON_COMMAND_RANGE(MENU_ITEM_PROCEDURES_NEW_START, MENU_ITEM_PROCEDURES_NEW_START + MAX_PROCEDURES, &IWWindow::OnProcedureSelected)
END_MESSAGE_MAP()

IWWindow::IWWindow(IWApproachDefinition selectedApproach, IWStyling styling) : titleBar(
    selectedApproach.title,
    RGB(styling.windowFrameColor.r, styling.windowFrameColor.g, styling.windowFrameColor.b),
    RGB(styling.windowFrameTextColor.r, styling.windowFrameTextColor.g, styling.windowFrameTextColor.b),
    RGB(styling.windowOuterFrameColor.r, styling.windowOuterFrameColor.g, styling.windowOuterFrameColor.b),
    this
), ilsVisualization(selectedApproach, styling, &this->font)
{
    this->selectedApproach = selectedApproach;

    this->windowBorderColor = RGB(styling.windowFrameColor.r, styling.windowFrameColor.g, styling.windowFrameColor.b);
    this->windowOuterBorderColor = RGB(styling.windowOuterFrameColor.r, styling.windowOuterFrameColor.g, styling.windowOuterFrameColor.b);

    float fontPointsSize = styling.fontSize * 72 / 96;
    this->font.CreatePointFont(int(fontPointsSize * 10), _T("EuroScope"));
}

IWWindow::~IWWindow()
{
    this->titleBar.DestroyWindow();
}

int IWWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect barRect(0, 0, lpCreateStruct->cx, TITLE_BAR_HEIGHT);
    if (!titleBar.CreateTopBar(this, barRect, IDC_TOPBAR))
    {
        AfxMessageBox(_T("Failed to create top bar"));
        return -1;
    }

    // Create the window content
    CRect contentRect = GetClientRectBelowTitleBar();
    if (!ilsVisualization.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, contentRect, this, 0))
    {
        AfxMessageBox(_T("Failed to create window content"));
        return -1;
    }

    return 0;
}

void IWWindow::OnPaint()
{
    CPaintDC dc(this); // Device context for painting

    // Create a memory DC for double buffering
    CRect rect;
    GetClientRect(&rect);
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);

    CBitmap bufferBitmap;
    bufferBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = memDC.SelectObject(&bufferBitmap);

    // Clear the background and draw window borders
    memDC.FillSolidRect(rect, windowOuterBorderColor);
    CRect innerRect = rect;
    innerRect.InflateRect(-WINDOW_OUTER_BORDER_WIDTH, -WINDOW_OUTER_BORDER_WIDTH);
    memDC.FillSolidRect(innerRect, windowBorderColor);

    // Copy the buffer to the screen
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // Cleanup
    memDC.SelectObject(pOldBitmap);
}

void IWWindow::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // Recalculate gradient or redraw on resize
    if (nType != SIZE_MINIMIZED)  // Ignore if window is minimized
    {
        Invalidate(); // Mark the entire client area for repaint
    }

    if (titleBar.GetSafeHwnd())
    {
        CRect barRect(0, 0, cx, TITLE_BAR_HEIGHT);
        titleBar.MoveWindow(barRect);
    }

    if (ilsVisualization.GetSafeHwnd())
    {
        CRect contentRect = GetClientRectBelowTitleBar();
        ilsVisualization.MoveWindow(contentRect);
    }
}

LRESULT IWWindow::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
    // Notify listener about the end of the resize or move operation
    if (m_listener)
    {
        m_listener->OnWindowRectangleChanged(this);
    }
    return 0;
}

void IWWindow::OnSizing(UINT nSide, LPRECT lpRect)
{
    int width = lpRect->right - lpRect->left;
    int height = lpRect->bottom - lpRect->top;

    // Snap width and height to X px increments
    int snappedWidth = static_cast<int>(std::round(width / SIZE_SNAP_INCREMENTS) * SIZE_SNAP_INCREMENTS);
    int snappedHeight = static_cast<int>(std::round(height / SIZE_SNAP_INCREMENTS) * SIZE_SNAP_INCREMENTS);

    // Adjust the RECT based on the resizing side. 
    // It's always the top right corner in our case
    if (nSide == WMSZ_TOPRIGHT) {
        lpRect->top = lpRect->bottom - snappedHeight;
        lpRect->right = lpRect->left + snappedWidth;
    }

    CWnd::OnSizing(nSide, lpRect); // Call the base class handler
}

BOOL IWWindow::OnNcActivate(BOOL bActive)
{
    // Prevent Windows from redrawing the NC area
    return TRUE;
}

BOOL IWWindow::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    return TRUE;
}

BOOL IWWindow::OnEraseBkgnd(CDC* pDC)
{
    // Do nothing here to prevent background clearing
    return TRUE;
}

LRESULT IWWindow::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
    IWLiveData* pData = reinterpret_cast<IWLiveData*>(wParam);
    if (pData) {
        m_latestLiveData = *pData;
        ilsVisualization.SetLatestLiveData(&m_latestLiveData);
    }

    return 0;
}

CRect IWWindow::GetClientRectBelowTitleBar()
{
    CRect rect;
    GetClientRect(&rect);  // Get full client area

    // Adjust rect to exclude the top bar area (assume the top bar is 30 pixels high)
    CRect topBarRect;
    titleBar.GetWindowRect(&topBarRect);
    ScreenToClient(&topBarRect); // Convert to client coordinates

    rect.top = topBarRect.bottom; // Move the top to below the top bar
    rect.left += WINDOW_BORDER_WIDTH;
    rect.right -= WINDOW_BORDER_WIDTH;
    rect.bottom -= WINDOW_BORDER_WIDTH;

    return rect;
}

void IWWindow::OnResizeStart()
{
    SendMessage(WM_NCLBUTTONDOWN, HTTOPRIGHT, NULL); // Resize using the top right corner
}

void IWWindow::OnCloseButtonClicked()
{
    this->DestroyWindow();
}

void IWWindow::OnMenuButtonClicked()
{
    CPoint point;
    GetCursorPos(&point);
    CreatePopupMenu(point);
}

void IWWindow::OnDestroy()
{
    if (m_listener) {
        m_listener->OnWindowClosed(this);
    }
    // Call base class cleanup (this ensures MFC cleans up the window itself)
    CWnd::OnDestroy();
}

void IWWindow::SetListener(IIWWndEventListener* listener)
{
    m_listener = listener;
}

void IWWindow::SetAvailableApproaches(const std::vector<IWApproachDefinition>& approaches)
{
    this->availableApproaches = approaches;
}

void IWWindow::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 150; // Minimum width in pixels
    lpMMI->ptMinTrackSize.y = 100; // Minimum height in pixels
}

std::string IWWindow::GetActiveApproachName() const
{
    std::lock_guard<std::mutex> lock(approachDataMutex);
    return selectedApproach.title;
}

void IWWindow::SetActiveApproach(const IWApproachDefinition& selectedApproach)
{
    std::unique_lock<std::mutex> lock(approachDataMutex);
    this->selectedApproach = selectedApproach;
    this->titleBar.SetTitle(selectedApproach.title);

    ilsVisualization.SetActiveApproach(this->selectedApproach);
    Invalidate();
    lock.unlock();

    m_listener->OnWindowRectangleChanged(this);
}

void IWWindow::CreatePopupMenu(CPoint point)
{
    // Create the main popup menu
    CMenu menu;
    menu.CreatePopupMenu();

    // Create the submenu with the available approaches
    CMenu subMenuSelect;
    CMenu subMenuOpenNew;
    subMenuSelect.CreatePopupMenu();
    subMenuOpenNew.CreatePopupMenu();

    int idCounter = 0;
    for (const IWApproachDefinition& approach : availableApproaches)
    {
        bool isActive = approach.title == this->selectedApproach.title;
        int menuItemID = idCounter++;
        if (isActive) {
            subMenuSelect.AppendMenu(MF_STRING | MF_CHECKED, MENU_ITEM_PROCEDURES_SEL_START + menuItemID, CString(approach.title.c_str()));
        }
        else {
            subMenuSelect.AppendMenu(MF_STRING, MENU_ITEM_PROCEDURES_SEL_START + menuItemID, CString(approach.title.c_str()));
        }
        subMenuOpenNew.AppendMenu(MF_STRING, MENU_ITEM_PROCEDURES_NEW_START + menuItemID, CString(approach.title.c_str()));
    }

    menu.AppendMenu(MF_POPUP, (UINT_PTR)subMenuSelect.m_hMenu, _T("View"));
    menu.AppendMenu(MF_POPUP, (UINT_PTR)subMenuOpenNew.m_hMenu, _T("Open"));

    // Add static menu items
    menu.AppendMenu(
        MF_STRING | (ilsVisualization.GetShowTagsByDefault() ? MF_CHECKED : MF_UNCHECKED),
        MENU_ITEM_SHOW_LABELS,
        _T("Show labels by default")
    );

    auto airportTemperature = m_latestLiveData.airportTemperatures.find(selectedApproach.airport);
    auto airportTemperatureMenuText =
        "Apply temperature correction (" 
        + selectedApproach.airport + ": " 
        + (airportTemperature != m_latestLiveData.airportTemperatures.end() ? std::to_string(airportTemperature->second) + "°C" : "N/A") 
        + ")";

    menu.AppendMenu(
        MF_STRING | (ilsVisualization.GetApplyTemperatureCorrection() ? MF_CHECKED : MF_UNCHECKED),
        MENU_ITEM_CORRECT_FOR_TEMPERATURE,
        _T(airportTemperatureMenuText.c_str())
    );
    menu.AppendMenu(
        MF_STRING,
        MENU_ITEM_FLIP,
        _T("Change orientation")
    );

    // Display the menu
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL IWWindow::OnMenuOptionSelected(UINT nID)
{
    if (nID == MENU_ITEM_FLIP)
    {
        ilsVisualization.SetLeftToRight(!ilsVisualization.GetLeftToRight());
        Invalidate();
    }
    else if (nID == MENU_ITEM_SHOW_LABELS)
    {
        ilsVisualization.SetShowTagsByDefault(!ilsVisualization.GetShowTagsByDefault());
        Invalidate();
    }
    else if (nID == MENU_ITEM_CORRECT_FOR_TEMPERATURE)
    {
        ilsVisualization.SetApplyTemperatureCorrection(!ilsVisualization.GetApplyTemperatureCorrection());
        Invalidate();
    }
    return TRUE;
}

void IWWindow::OnProcedureSelected(UINT nID)
{
    int index = nID - MENU_ITEM_PROCEDURES_SEL_START;
    if (index >= 0 && index < availableApproaches.size())
    {
        // Set the selected approach
        IWApproachDefinition selectedApproach = availableApproaches[index];
        SetActiveApproach(selectedApproach);
    }
    else
    {
        // Open a new window with the selected approach
        index = nID - MENU_ITEM_PROCEDURES_NEW_START;
        if (index >= 0 && index < availableApproaches.size())
        {
            IWApproachDefinition selectedApproach = availableApproaches[index];
            m_listener->OnWindowMenuOpenNew(selectedApproach.title);
        }
    }
    Invalidate();
}

