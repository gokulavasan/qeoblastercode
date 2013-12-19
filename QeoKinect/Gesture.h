#pragma once
class Gesture
{
public:
	Gesture(void);
	~Gesture(void);
	void load(std::string command, int id, int resource_id);
	bool hit_test(int x, int y);
	int GetGestureID();
	ID2D1Bitmap* CreateBitmap(std::wstring filename,
		ID2D1HwndRenderTarget* pRenderTarget);	
	std::string GetCommand() { return command; }
private:
	bool bits[100 * 100];
	CImage bitmap;
	int id;	
	std::string command;
};

