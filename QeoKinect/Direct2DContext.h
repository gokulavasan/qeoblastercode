#pragma once
struct Direct2DContext
{
	Direct2DContext(void);
	~Direct2DContext(void);

	ID2D1Factory* m_pD2DFactory;		
};

