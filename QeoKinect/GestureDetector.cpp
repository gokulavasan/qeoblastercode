#include "stdafx.h"
#include "GestureDetector.h"
#include "resource.h"
#include "GestureIDs.h"

const double PI = 3.1415926;
using namespace std;

bool images_loaded = false;
int count = 0;
bool window_registered = false;

std::vector<Gesture*> GestureDetector::gestures;
int bounds_offset = 25;

Gesture circleGesture;
Gesture forwardGesture;
Gesture reverseGesture;
Gesture powerGesture;
Gesture stopGesture;

#define MIN_BOUNDS 250

GestureDetector::GestureDetector() :
	m_pRenderTarget(nullptr),
	m_pCircleBrush(nullptr),
	bitmapsLoaded(false)
{	
}

GestureDetector::~GestureDetector()
{
	// clean up Direct2D objects
    DiscardDirect2DResources();
}

Gesture* GestureDetector::DetectGesture(std::deque<D2D1_POINT_2F> points)
{		
	HRESULT hr = EnsureDirect2DResources();
    if(FAILED(hr))    
        return NULL;
	if(m_pRenderTarget == NULL)
		return NULL;
	loadBitmaps(m_pRenderTarget);
	if(!bitmapsLoaded)
	{		
		circleBitmap = circleGesture.CreateBitmap(L"circle.bmp", m_pRenderTarget);
		forwardBitmap = forwardGesture.CreateBitmap(L"forward.bmp", m_pRenderTarget);
		reverseBitmap = reverseGesture.CreateBitmap(L"reverse.bmp", m_pRenderTarget);
		powerBitmap = powerGesture.CreateBitmap(L"power.bmp", m_pRenderTarget);
		stopBitmap = stopGesture.CreateBitmap(L"stop.bmp", m_pRenderTarget);
		bitmapsLoaded = true;		
	}
	int gesture_index = 0;
	Gesture* render_gesture = gestures[gesture_index];
	
	double bounds_left = 99999;
	double bounds_right = -99999;
	double bounds_top = 99999;
	double bounds_bottom = -99999;

	int num_pts = points.size();
	for(int i = 0; i < num_pts; ++i)
	{
		D2D1_POINT_2F pt = points[i];
		if(pt.x < bounds_left)
			bounds_left = pt.x;
		if(pt.x > bounds_right)
			bounds_right = pt.x;
		if(pt.y < bounds_top)
			bounds_top = pt.y;
		if(pt.y > bounds_bottom)
			bounds_bottom = pt.y;
	}
	int bounds_width = bounds_right - bounds_left + bounds_offset * 2;
	int bounds_height = bounds_bottom - bounds_top + bounds_offset * 2;
	
	if(bounds_width < bounds_height)
	{
		int old_width = bounds_width;
		bounds_width = bounds_height;
		bounds_left = (bounds_left + old_width / 2) - bounds_width / 2;
	} 
	else if(bounds_height < bounds_width)
	{
		int old_height = bounds_height;
		bounds_height = bounds_width;
		bounds_top = (bounds_top + old_height / 2) - bounds_height / 2;
	}


	if(bounds_width < MIN_BOUNDS || bounds_height < MIN_BOUNDS)
	{
		return NULL;
	}
	wstringstream out;
	out << "bounds " << bounds_width << " " << bounds_height << endl;
	OutputDebugString(out.str().c_str());
	
	float xScale = 100.0 / (float)(bounds_width);
	float yScale = 100.0 / (float)(bounds_height);	

	m_pRenderTarget->BeginDraw();
    m_pRenderTarget->Clear();

    RECT rct;
    GetClientRect(m_hWnd, &rct);
    int width = rct.right;
    int height = rct.bottom;

	// draw background	
	m_pRenderTarget->DrawBitmap(circleBitmap);

	D2D1_RECT_F off1;
	off1.left = 120;
	off1.top = 0;
	off1.right = 220;
	off1.bottom = 100;
	m_pRenderTarget->DrawBitmap(forwardBitmap, off1);

	off1.left = 240;
	off1.top = 0;
	off1.right = 360;
	off1.bottom = 100;
	m_pRenderTarget->DrawBitmap(reverseBitmap, off1);

	off1.left = 0;
	off1.top = 120;
	off1.right = 100;
	off1.bottom = 220;
	m_pRenderTarget->DrawBitmap(powerBitmap, off1);

	off1.left = 120;
	off1.top = 120;
	off1.right = 220;
	off1.bottom = 220;
	m_pRenderTarget->DrawBitmap(stopBitmap, off1);

	int num_gestures = gestures.size();
	vector<int> hit_counts(num_gestures);
	for(int i = 0; i < hit_counts.size(); ++i)
		hit_counts[i] = 0;
	// draw gesture dots
	for(int i = 0; i < num_pts; ++i)
	{
		points[i].x = (points[i].x - bounds_left + bounds_offset) * xScale;
		points[i].y = (points[i].y - bounds_top + bounds_offset) * yScale;		
		wstringstream o;
		int x = points[i].x;
		int y = points[i].y;
		if(x > 99) x = 99;
		if(x < 0) x = 0;
		if(y > 99) y = 99;
		if(y < 0) y = 0;		
		OutputDebugString(o.str().c_str());
		D2D1_ELLIPSE ellipse;
			ellipse.point.x = x;
			ellipse.point.y = y;
			ellipse.radiusX = 5;
			ellipse.radiusY = 5;
		if(circleGesture.hit_test(x, y)) 
		{
			++hit_counts[0];					
			m_pRenderTarget->DrawEllipse(&ellipse, m_pCircleBrush);						
		} 
		else 
		{
			m_pRenderTarget->DrawEllipse(&ellipse, m_pOutsideBrush);			
		}
		ellipse.point.x += 120;
		if(forwardGesture.hit_test(x, y))
		{			
			++hit_counts[1];
			m_pRenderTarget->DrawEllipse(&ellipse, m_pCircleBrush);
		}
		else
		{			
			m_pRenderTarget->DrawEllipse(&ellipse, m_pOutsideBrush);
		}
		ellipse.point.x += 120;
		if(reverseGesture.hit_test(x, y))
		{			
			++hit_counts[2];
			m_pRenderTarget->DrawEllipse(&ellipse, m_pCircleBrush);
		}
		else
		{			
			m_pRenderTarget->DrawEllipse(&ellipse, m_pOutsideBrush);
		}
		ellipse.point.x -= 240;
		ellipse.point.y += 120;
		if(powerGesture.hit_test(x, y))
		{			
			++hit_counts[3];
			m_pRenderTarget->DrawEllipse(&ellipse, m_pCircleBrush);
		}
		else
		{			
			m_pRenderTarget->DrawEllipse(&ellipse, m_pOutsideBrush);
		}
		ellipse.point.x += 120;
		if(stopGesture.hit_test(x, y))
		{			
			++hit_counts[4];
			m_pRenderTarget->DrawEllipse(&ellipse, m_pCircleBrush);
		}
		else
		{			
			m_pRenderTarget->DrawEllipse(&ellipse, m_pOutsideBrush);
		}
	}	
	wstringstream out2;
	out2 << L"hit count " << hit_counts[gesture_index] << endl;
	OutputDebugString(out2.str().c_str());	

	hr = m_pRenderTarget->EndDraw();
	
    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (D2DERR_RECREATE_TARGET == hr)
    {
        hr = S_OK;
        DiscardDirect2DResources();
    }
	for(int i = 0; i < gestures.size(); ++i)
	{
		if(hit_counts[i] > 38)
		{	
			return gestures[i];
		}
	}
	return NULL;
}

HRESULT GestureDetector::GetWICFactory(IWICImagingFactory** factory)
{
   static CComPtr<IWICImagingFactory> m_pWICFactory;
   HRESULT hr = S_OK;

   if (nullptr == m_pWICFactory)
   {
      hr = CoCreateInstance(
            CLSID_WICImagingFactory, nullptr,
            CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory) );
   }

   if (SUCCEEDED(hr))
   {
      *factory = m_pWICFactory;
   }

   return hr;
}

void GestureDetector::Initialize(Direct2DContext* context)
{
	d2d_context = context;
	MSG msg = {0};

	HINSTANCE hInstance = GetModuleHandle(NULL);
	if(!window_registered)
	{		
		WNDCLASS wc = {0};

		// Dialog custom window class
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.cbWndExtra = DLGWINDOWEXTRA;
		wc.hInstance = hInstance;
		wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wc.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_APP));
		wc.lpfnWndProc = DefDlgProcW;
		wc.lpszClassName = L"GestureDetectorDialog";

		if (!RegisterClassW(&wc))    
			return;
		window_registered = true;
	}

	// Create gesture display window
    HWND hWndApp = CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCE(IDD_GESTURE_DLG),
        NULL,
        (DLGPROC)GestureDetector::MessageRouter, 
        reinterpret_cast<LPARAM>(this));

    // Show window
    ShowWindow(hWndApp, SW_SHOW);    
}

void GestureDetector::loadBitmaps(ID2D1HwndRenderTarget* pRenderTarget)
{	
	if(!images_loaded)
	{			
		circleGesture.load("PLAY", "PAUSE", CIRCLE_GESTURE, IDB_CIRCLE);
		gestures.push_back(&circleGesture);				

		forwardGesture.load("NEXT", "NEXT", FORWARD_GESTURE, IDB_FORWARD);
		gestures.push_back(&forwardGesture);		

		reverseGesture.load("PREV", "PREV", REVERSE_GESTURE, IDB_REVERSE);
		gestures.push_back(&reverseGesture);		

		powerGesture.load("POWER", "POWER", POWER_GESTURE, IDB_POWER);
		gestures.push_back(&powerGesture);		

		stopGesture.load("STOP", "STOP", STOP_GESTURE, IDB_STOP);
		gestures.push_back(&stopGesture);		

		images_loaded = true;
	}
}

HRESULT GestureDetector::EnsureDirect2DResources()
{
    HRESULT hr = S_OK;

    // If there isn't currently a render target, we need to create one
    if (NULL == m_pRenderTarget)
    {
        RECT rc;
        GetWindowRect(m_hWnd, &rc);  

        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;
        D2D1_SIZE_U size = D2D1::SizeU( width, height );
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;

        // Create a Hwnd render target, in order to render to the window set in initialize
        hr = d2d_context->m_pD2DFactory->CreateHwndRenderTarget(
            rtProps,
            D2D1::HwndRenderTargetProperties(m_hWnd, size),
            &m_pRenderTarget
            );
        if ( FAILED(hr) )
        {            
            return hr;
        }
        
        m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f), &m_pCircleBrush);
		m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 1.0f), &m_pOutsideBrush);				
    }

    return hr;
}

void GestureDetector::DiscardDirect2DResources( )
{
    SafeRelease(m_pRenderTarget);
    SafeRelease(m_pCircleBrush);    
}

LRESULT CALLBACK GestureDetector::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    GestureDetector* pThis = NULL;

    if(WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<GestureDetector*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else    
        pThis = reinterpret_cast<GestureDetector*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
    

    if (pThis)    
        return pThis->DlgProc(hWnd, uMsg, wParam, lParam);    

    return 0;
}

LRESULT CALLBACK GestureDetector::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        {
            // Bind application window handle
            m_hWnd = hWnd;			
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
        break;
    }

    return FALSE;
}


 