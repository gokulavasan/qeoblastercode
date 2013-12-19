#include "stdafx.h"
#include "Gesture.h"
#include "resource.h"

using namespace std;

Gesture::Gesture(void) 	
{
}


Gesture::~Gesture(void)
{
}

int Gesture::GetGestureID()
{
	return id;
}

bool Gesture::hit_test(int x, int y)
{
	return bits[x + y * 100];
}

void Gesture::load(string my_command_left, string my_command_right, int my_id, int resource_id)
{
	command_left = my_command_left;
	command_right = my_command_right;
	id = my_id;
	ZeroMemory(&(bits[0]), 100 * 100);		
	bitmap.LoadFromResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resource_id));
	for(int y = 0; y < 100; ++y)
	{
		for(int x = 0; x < 100; ++x)
		{
			if(bitmap.GetPixel(x, y) != 0)				
				bits[x + y * 100] = true;				
		}
	}					
}

ID2D1Bitmap* Gesture::CreateBitmap(wstring filename, ID2D1HwndRenderTarget* pRenderTarget)
{
   HRESULT hr = S_OK;
	
   
    IWICImagingFactory* spWICFactory;
    IWICBitmapDecoder* spDecoder;
    IWICFormatConverter* spConverter;
 
    D2D1_SIZE_U size = D2D1::SizeU(100, 100);
 
	/*
    //create a D2D render target
    hr = spD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            size),
			&spRT);*/
 
	//create WIC factory
	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void **>(&spWICFactory)
		);
 
	//load image using WIC
	spWICFactory->CreateDecoderFromFilename(
		filename.c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&spDecoder);
 
	IWICBitmapFrameDecode* spSource = NULL;
	//get the initial frame
	spDecoder->GetFrame(
		0,
		&spSource);
 
	//format convert to 32bppPBGRA -- which D2D expects
	spWICFactory->CreateFormatConverter(
		&spConverter);
 
	//initialize the format converter
	spConverter->Initialize(
		spSource,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.f,
		WICBitmapPaletteTypeMedianCut);
 
	ID2D1Bitmap* bitmap = nullptr;
	//create a D2D bitmap from the WIC bitmap.
	pRenderTarget->CreateBitmapFromWicBitmap(
		spConverter,
		NULL,
		&bitmap);
	
	return bitmap;
}