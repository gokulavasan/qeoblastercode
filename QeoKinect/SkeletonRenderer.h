#pragma once

#include "NuiApi.h"
#include "resource.h"
#include "Direct2DContext.h"
#include "GestureDetector.h"

class CSkeletonRenderer
{
public:
	CSkeletonRenderer(void);
	~CSkeletonRenderer(void);
	void Initialize(HWND hWnd, Direct2DContext* d2d_context);
	int Render(NUI_SKELETON_FRAME& skeletonFrame, D2D1_POINT_2F points[], std::deque<D2D1_POINT_2F>& left_hand_track, std::deque<D2D1_POINT_2F>& right_hand_track,
		Gesture* last_gesture, bool last_left);

	void SetStatusMessage(WCHAR* szMessage);
private:	
	Direct2DContext* d2d_context;
	static const int cScreenWidth  = 320;
    static const int cScreenHeight = 240;
    static const int cStatusMessageMaxLen = MAX_PATH * 2;
	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	// Direct2D    
	HRESULT CreateDeviceIndependentResources();
	void DrawHandTracking(std::deque<D2D1_POINT_2F>& left_hand_track, std::deque<D2D1_POINT_2F>& right_hand_track);
    int DrawBone(const NUI_SKELETON_DATA & skel, D2D1_POINT_2F points[], NUI_SKELETON_POSITION_INDEX bone0, NUI_SKELETON_POSITION_INDEX bone1);    
    int DrawSkeleton(const NUI_SKELETON_DATA & skel, D2D1_POINT_2F points[], int windowWidth, int windowHeight);
    D2D1_POINT_2F SkeletonToScreen(Vector4 skeletonPoint, int width, int height);
	HRESULT EnsureDirect2DResources( );
    void DiscardDirect2DResources( );  

	// Skeletal drawing
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pBrushJointTracked;
    ID2D1SolidColorBrush* m_pBrushJointInferred;
    ID2D1SolidColorBrush* m_pBrushBoneTracked;
    ID2D1SolidColorBrush* m_pBrushBoneInferred;	
	ID2D1SolidColorBrush* m_pHandTrackingLeft;
	ID2D1SolidColorBrush* m_pHandTrackingRight;	

	HWND m_hWnd;	
};

