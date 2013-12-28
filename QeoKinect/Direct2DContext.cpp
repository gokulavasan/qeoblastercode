#include "stdafx.h"
#include "Direct2DContext.h"

Direct2DContext::Direct2DContext(void) :
	m_pD2DFactory(nullptr)
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
}


Direct2DContext::~Direct2DContext(void)
{	
    // clean up Direct2D
    SafeRelease(m_pD2DFactory);
}
