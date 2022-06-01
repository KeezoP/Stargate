#pragma once

//this struct is the information we want to pass thru to the game processing.  We never want to be dealing with the hardware directly.
//thats the job for this class.  The other benefit of this abstraction of input data is that when we want to change the input
//to different hardware.  its really easily done in this class by mapping to the inputcommands. 

struct InputCommands
{
	bool forward;
	bool back;
	bool right;
	bool left;
	bool drag;
	int	dragx;
	int dragy;
	bool strUp;
	bool strDown;
	bool beginDial;
	bool chev1;
	bool chev2;
	bool chev3;
	bool chev4;
	bool chev5;
	bool chev6;
	bool chev7;
	bool chev8;
	bool chev9;
	bool chevClear;
	bool direction;
	bool quatTest;
	bool portal;
};


class Input
{
public:
	Input();
	~Input();
	void Initialise(HWND window);
	void Update();
	bool Quit();
	int initMouseX = 0;
	int initMouseY = 0;
	InputCommands getGameInput();


private:
	bool m_quitApp;
	std::unique_ptr<DirectX::Keyboard>	m_keyboard;
	std::unique_ptr<DirectX::Mouse>		m_mouse;
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;
	DirectX::Mouse::ButtonStateTracker		m_MouseTracker;
	InputCommands							m_GameInput;
};

