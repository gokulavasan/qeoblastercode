#include "stdafx.h"
#include <strsafe.h>
#include "SkeletonTracker.h"
#include "resource.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

const int MAX_POINTS = 40;
int ignore_frames = 0;
using namespace std;

CSkeletonTracker::CSkeletonTracker() :        
    m_pSkeletonStreamHandle(INVALID_HANDLE_VALUE),
    m_bSeatedMode(false),
	m_pNuiSensor(nullptr),
	m_hNextFrameEvent(INVALID_HANDLE_VALUE),
	lastGesture(nullptr)
{    
	ZeroMemory(m_Points,sizeof(m_Points));
}

CSkeletonTracker::~CSkeletonTracker()
{       
	if (m_hNextFrameEvent && (m_hNextFrameEvent != INVALID_HANDLE_VALUE))    
        CloseHandle(m_hNextFrameEvent);

	if(m_pNuiSensor)    
		m_pNuiSensor->NuiShutdown();		

    SafeRelease(m_pNuiSensor);
}

void CSkeletonTracker::Update(CSkeletonRenderer& renderer)
{
	const int eventCount = 1;
    HANDLE hEvents[eventCount];

	hEvents[0] = m_hNextFrameEvent;

    // Check to see if we have either a message (by passing in QS_ALLEVENTS)
    // Or a Kinect event (hEvents)
    // Update() will check for Kinect events individually, in case more than one are signalled
    MsgWaitForMultipleObjects(eventCount, hEvents, FALSE, INFINITE, QS_ALLINPUT);

    // Explicitly check the Kinect frame event since MsgWaitForMultipleObjects
    // can return for other reasons even though it is signaled.
    if(!Ready())    
		return;

	if(WAIT_OBJECT_0 == WaitForSingleObject(m_hNextFrameEvent, 0))
		ProcessSkeleton(renderer);
}

bool CSkeletonTracker::Ready()
{
	return m_pNuiSensor != nullptr;
}

#define DEFAULT_BUFLEN 512
SOCKET ConnectSocket = INVALID_SOCKET;

void CSkeletonTracker::Initialize(CSkeletonRenderer& renderer, Direct2DContext* d2d_context)
{
	// Look for a connected Kinect, and create it if found
    CreateFirstConnected(renderer);
	left_gesture_detector.Initialize(d2d_context);
	right_gesture_detector.Initialize(d2d_context);

	WSADATA wsaData;    
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;    
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        AfxMessageBox(L"WSAStartup failed");        
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("192.168.1.104", "999", &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();        
		AfxMessageBox(L"Failed to get address info");
    }
	// Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            AfxMessageBox(L"Socket failed");
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        AfxMessageBox(L"Unable to connect to server!");
    }    
}

void CSkeletonTracker::ToggleSeated()
{
	// Toggle out internal state for near mode
    m_bSeatedMode = !m_bSeatedMode;

    if (nullptr != m_pNuiSensor)
    {
        // Set near mode for sensor based on our internal state
        m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextFrameEvent, m_bSeatedMode ? NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT : 0);
    }
}

void CSkeletonTracker::ProcessSkeleton(CSkeletonRenderer& renderer)
{	
    NUI_SKELETON_FRAME skeletonFrame = {0};

    HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
    if(FAILED(hr))    
        return;

    // smooth out the skeleton data
    m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

	renderer.Render(skeletonFrame, m_Points, left_hand_track, right_hand_track, (ignore_frames > 0) ? lastGesture : NULL);    

	if(ignore_frames > 0)
	{
		ignore_frames--;
		return;
	}
	D2D1_POINT_2F left_hand_point = m_Points[NUI_SKELETON_POSITION_HAND_LEFT];
	D2D1_POINT_2F right_hand_point = m_Points[NUI_SKELETON_POSITION_HAND_RIGHT];

	left_hand_track.push_back(left_hand_point);
	right_hand_track.push_back(right_hand_point);

	if(left_hand_track.size() > MAX_POINTS)	
		left_hand_track.pop_front();
	if(right_hand_track.size() > MAX_POINTS)
		right_hand_track.pop_front();		

	/*NUI_SKELETON_DATA& skel = skeletonFrame.SkeletonData[0];
	for(int i = 0; i < 12; ++i)
	{			
		if (skel.eSkeletonPositionTrackingState[i] == NUI_SKELETON_POSITION_NOT_TRACKED)
			return;    
	}*/
	Gesture* left_gesture = left_gesture_detector.DetectGesture(left_hand_track);
	if(left_gesture == NULL)
	{
		Gesture* right_gesture = right_gesture_detector.GestureDetector::DetectGesture(right_hand_track);
		HandleGesture(false, right_gesture, renderer);
		lastGesture = right_gesture;
	}
	else
	{
		HandleGesture(true, left_gesture, renderer);		
		lastGesture = left_gesture;
	}
}

void CSkeletonTracker::HandleGesture(bool isLeft, Gesture* gesture, CSkeletonRenderer& renderer)
{
	if(gesture != NULL)
	{
		string sendbuf = gesture->GetCommand();
		if(sendbuf != "PLAY")
			return;
		if(isLeft && sendbuf == "PLAY")
		{
			sendbuf = "PAUSE";
		}
		wstringstream out;
		out << "Got Gesture: " << wstring(sendbuf.begin(), sendbuf.end()) << endl;
		wstring msg = out.str();
		renderer.SetStatusMessage((WCHAR*)msg.c_str());
		left_hand_track.clear();
		right_hand_track.clear();
		ignore_frames = 50;

		int iResult;
		// Send an initial buffer
		iResult = send( ConnectSocket, sendbuf.c_str(), sendbuf.length(), 0 );
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			AfxMessageBox(L"Failed to send to server!");
		}

		printf("Bytes Sent: %ld\n", iResult);
	}
}

HRESULT CSkeletonTracker::CreateFirstConnected(CSkeletonRenderer& renderer)
{
    INuiSensor * pNuiSensor;

    int iSensorCount = 0;
    HRESULT hr = NuiGetSensorCount(&iSensorCount);
    if (FAILED(hr))    
        return hr;    

    // Look at each Kinect sensor
    for (int i = 0; i < iSensorCount; ++i)
    {
        // Create the sensor so we can check status, if we can't create it, move on to the next
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))        
            continue;        

        // Get the status of the sensor, and if connected, then we can initialize it
        hr = pNuiSensor->NuiStatus();
        if (S_OK == hr)
        {
            m_pNuiSensor = pNuiSensor;
            break;
        }

        // This sensor wasn't OK, so release it since we're not using it
        pNuiSensor->Release();
    }

    if(nullptr != m_pNuiSensor)
    {		
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON); 
        if (SUCCEEDED(hr))
        {
            // Create an event that will be signaled when skeleton data is available
            m_hNextFrameEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

            // Open a skeleton stream to receive skeleton data
            hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextFrameEvent, 0); 		
        }
    }

    if (nullptr == m_pNuiSensor || FAILED(hr))
    {
        renderer.SetStatusMessage(L"No ready Kinect found!");
        return E_FAIL;
    }

    return hr;
}