#pragma once

#include "Direct2DContext.h"
#include "Gesture.h"

class GestureDetector
{
public:
	GestureDetector(void);
	~GestureDetector();	
	void Initialize(Direct2DContext* context);
	Gesture* DetectGesture(std::deque<D2D1_POINT_2F> points);
	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);    
    LRESULT CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:		
	static HRESULT GetWICFactory(IWICImagingFactory** factory);
	Direct2DContext* d2d_context;
	HRESULT EnsureDirect2DResources( );
    void DiscardDirect2DResources( );  
	HWND m_hWnd;
	static void loadBitmaps(ID2D1HwndRenderTarget* pRenderTarget);
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pCircleBrush;
	ID2D1SolidColorBrush* m_pOutsideBrush;
	HRESULT CreateDeviceResources();	
	static std::vector<Gesture*> gestures;

	bool bitmapsLoaded;
	ID2D1Bitmap* circleBitmap;
	ID2D1Bitmap* reverseBitmap;
	ID2D1Bitmap* forwardBitmap;
	ID2D1Bitmap* stopBitmap;
	ID2D1Bitmap* powerBitmap;
};

