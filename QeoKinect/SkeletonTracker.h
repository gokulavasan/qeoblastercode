#pragma once

#include "resource.h"
#include "NuiApi.h"
#include "SkeletonRenderer.h"
#include "GestureDetector.h"

class CSkeletonTracker
{    

public:
    CSkeletonTracker();
    ~CSkeletonTracker();
	void Update(CSkeletonRenderer& renderer);
	void ProcessSkeleton(CSkeletonRenderer& renderer);
	bool Ready();
	void Initialize(CSkeletonRenderer& renderer, Direct2DContext* d2d_context);	
	void ToggleSeated();
	std::deque<D2D1_POINT_2F> left_hand_track;
	std::deque<D2D1_POINT_2F> right_hand_track;	
private:    
	void HandleGesture(bool isLeft, Gesture* gesture, CSkeletonRenderer& renderer);
	GestureDetector left_gesture_detector;
	GestureDetector right_gesture_detector;
	HANDLE m_hNextFrameEvent;    
	HRESULT CreateFirstConnected(CSkeletonRenderer& renderer);
	D2D1_POINT_2F m_Points[NUI_SKELETON_POSITION_COUNT];        
    bool m_bSeatedMode;           
    HANDLE m_pSkeletonStreamHandle;  
	INuiSensor* m_pNuiSensor;
	Gesture* lastGesture;
};
