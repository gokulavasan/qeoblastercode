#include "stdafx.h"
#include "QeoBlasterApp.h"
#include "SkeletonTracker.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    CQeoBlasterApp application;
    application.Run(hInstance, nCmdShow);
}

int CQeoBlasterApp::Run(HINSTANCE hInstance, int nCmdShow)
{
	MSG msg = {0};
    WNDCLASS wc = {0};

    // Dialog custom window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra = DLGWINDOWEXTRA;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
    wc.lpfnWndProc = DefDlgProcW;
    wc.lpszClassName = L"QeoBlasterAppDlg";

    if (!RegisterClassW(&wc))    
        return 0;
   
    // Create main application window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_APP),
        NULL,
        (DLGPROC)CQeoBlasterApp::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, nCmdShow);    

    // Main message loop
    while (WM_QUIT != msg.message)
    {
		m_skeletonTracker.Update(m_skeletonRenderer);        

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // If a dialog message will be taken care of by the dialog proc
            if ((hWndApp != NULL) && IsDialogMessageW(hWndApp, &msg))           
                continue;            

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK CQeoBlasterApp::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CQeoBlasterApp* pThis = NULL;

    if(WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<CQeoBlasterApp*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else    
        pThis = reinterpret_cast<CQeoBlasterApp*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    

    if (pThis)    
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);    

    return 0;
}

LRESULT CALLBACK CQeoBlasterApp::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;
			m_skeletonRenderer.Initialize(hWnd, &d2d_context);
			m_skeletonTracker.Initialize(m_skeletonRenderer, &d2d_context);
        }
        break;

        // If the titlebar X is clicked, destroy app
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        // Quit the main message pump
        PostQuitMessage(0);
        break;

        // Handle button press
    case WM_COMMAND:
        // If it was for the near mode control and a clicked event, change near mode
        if (IDC_CHECK_SEATED == LOWORD(wParam) && BN_CLICKED == HIWORD(wParam))        
			m_skeletonTracker.ToggleSeated();
        
        break;
    }

    return FALSE;
}

