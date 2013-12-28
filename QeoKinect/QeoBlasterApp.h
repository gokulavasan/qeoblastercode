#pragma once

#include "NuiApi.h"
#include "SkeletonTracker.h"
#include "SkeletonRenderer.h"

#include "resource.h"

class CQeoBlasterApp
{	
public:	
	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);    
    LRESULT CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    int Run(HINSTANCE hInstance, int nCmdShow);	
    
private:
	Direct2DContext d2d_context;
	void Update();	     
	CSkeletonTracker m_skeletonTracker;
	CSkeletonRenderer m_skeletonRenderer;	
	HWND m_hWnd;	
};

