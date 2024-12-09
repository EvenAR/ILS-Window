#include "pch.h"
#include "framework.h"
#include "ParPluginDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(ParApp, CWinApp)
END_MESSAGE_MAP()

ParApp::ParApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMiniPARApp object

ParApp theApp;


// ParApp initialization

BOOL ParApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

ParPlugin* pMyPlugIn;

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance) {     // allocate 
	*ppPlugInInstance = pMyPlugIn = new ParPlugin;

}

void __declspec (dllexport) EuroScopePlugInExit(void) { 
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	delete pMyPlugIn; 
}
