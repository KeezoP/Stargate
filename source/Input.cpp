#include "pch.h"
#include "Input.h"
#include "Mouse.h"

bool aCheck = false;
Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.drag		= false;
	m_GameInput.strUp		= false;
	m_GameInput.strDown		= false;
	m_GameInput.beginDial	= false;
	m_GameInput.beginDial	= false;
	m_GameInput.chev1		= false;
	m_GameInput.chev2		= false;
	m_GameInput.chev3		= false;
	m_GameInput.chev4		= false;
	m_GameInput.chev5		= false;
	m_GameInput.chev6		= false;
	m_GameInput.chev7		= false;
	m_GameInput.chev8		= false;
	m_GameInput.chev9		= false;
	m_GameInput.direction	= false;

}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;

	//U key
	if (kb.U || kb.Space)	m_GameInput.strUp = true;
	else		m_GameInput.strUp = false;
	
	//J key
	if (kb.J || kb.LeftShift)	m_GameInput.strDown = true;
	else		m_GameInput.strDown = false;
	
	//R key
	if (m_KeyboardTracker.pressed.R)	m_GameInput.beginDial = true;
	else		m_GameInput.beginDial = false;

	//T key
	if (m_KeyboardTracker.pressed.T) m_GameInput.direction = true;
	else m_GameInput.direction = false;

	//1 key
	if (kb.NumPad1 || kb.D1)	m_GameInput.chev1 = true;
	else			m_GameInput.chev1 = false;

	//2 key
	if (kb.NumPad2 || kb.D2)	m_GameInput.chev2 = true;
	else			m_GameInput.chev2 = false;

	//3 key
	if (kb.NumPad3 || kb.D3)	m_GameInput.chev3 = true;
	else			m_GameInput.chev3 = false;

	//4 key
	if (kb.NumPad4 || kb.D4)	m_GameInput.chev4 = true;
	else			m_GameInput.chev4 = false;

	//5 key
	if (kb.NumPad5 || kb.D5)	m_GameInput.chev5 = true;
	else			m_GameInput.chev5 = false;

	//6 key
	if (kb.NumPad6 || kb.D6)	m_GameInput.chev6 = true;
	else			m_GameInput.chev6 = false;

	//7 key
	if (kb.NumPad7 || kb.D7)	m_GameInput.chev7 = true;
	else			m_GameInput.chev7 = false;

	//8 key
	if (kb.NumPad8 || kb.D8)	m_GameInput.chev8 = true;
	else			m_GameInput.chev8 = false;

	//9 key
	if (kb.NumPad9 || kb.D9)	m_GameInput.chev9 = true;
	else			m_GameInput.chev9 = false;
	//0 key
	if (kb.NumPad0 || kb.D0 || kb.Back)	m_GameInput.chevClear = true;
	else			m_GameInput.chevClear = false;

	//q key
	if (m_KeyboardTracker.pressed.Q) m_GameInput.quatTest = true;
	else m_GameInput.quatTest = false;

	//p key
	if (m_KeyboardTracker.pressed.P) m_GameInput.portal = true;
	else m_GameInput.portal = false;


	// an attempt was made on 3d mouse rotations but failed
	
	/*
	if (m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED)
	{
		initMouseX = mouse.x;
		initMouseY = mouse.y;
		m_GameInput.drag = true;
		m_mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	}

	else if (m_MouseTracker.leftButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		m_GameInput.drag = false;
		m_mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
	}

	if (mouse.leftButton) {
		m_GameInput.dragx = -1 * (mouse.x - initMouseX);
		m_GameInput.dragy = -1 * (mouse.y - initMouseY);
	}
	*/

}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
