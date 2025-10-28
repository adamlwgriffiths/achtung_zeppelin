#include "InputController.h"

CInputController * CInputController::pInputController;													// singleton pointer


const float	CInputController::s_Input_KB_ShipAccelAmount		= 75.0f;
const float	CInputController::s_Input_KB_ShipDecelAmount		= 75.0f;
const float	CInputController::s_Input_KB_ShipStrafeAmount		= 75.0f;
const float	CInputController::s_Input_KB_ShipPitchAmount		= 75.0f;
const float	CInputController::s_Input_KB_ShipYawAmount			= 75.0f;
const float	CInputController::s_Input_KB_ShipRollAmount			= 75.0f;

const float	CInputController::s_Input_KB_CameraOrbitAmount		= 75.0f;
const float	CInputController::s_Input_KB_CameraElevateAmount	= 75.0f;
const float	CInputController::s_Input_KB_CameraZoomAmount		= 75.0f;


const float	CInputController::s_Input_Mouse_RollScale			= 6.0f;
const float	CInputController::s_Input_Mouse_PitchScale			= 6.0f;




void * CInputController::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch(ID)
	{
		case MSGID_INIT:
			Init(Data);
			break;
		case MSGID_UPDATE:
			Update(Data);
			break;
		case MSGID_DESTROY:
			Destroy();
			break;

		case MSGID_INPUT_KEY_PRESSED:
			this->KeyPressed(Data);
			break;
		case MSGID_INPUT_KEY_RELEASED:
			this->KeyReleased(Data);
			break;


		case MSGID_GET_KEYS:
			result = (void*)&this->m_sKeys;
			break;

		case MSGID_CAMERA_SWITCHVIEW:
			this->m_sKeys.m_bCamera_Switch = !this->m_sKeys.m_bCamera_Switch;
			break;

		default:
			assert(0);		// should never get here!
			break;
	}

	return result;
}

CInputController::CInputController()
{

}

CInputController::~CInputController()
{

}


/////////////////////////////////////////////
// Opens an instance of the singleton.<P>
// If its already open, throw an assertion so we can track down
// any bad logic in our program.
void CInputController::Open()
{
	assert(!pInputController);			// shouldnt get here either!

	pInputController = new CInputController();
}

/////////////////////////////////////////////
// Kill our singleton
void CInputController::Close()
{
	assert(pInputController);

	delete pInputController;
	pInputController = NULL;
}

/////////////////////////////////////////////
// Get the singleton instance of the class
CTask * CInputController::Get()
{
	if (!pInputController)
		CInputController::Open();

	return (CTask*)pInputController;
}




void CInputController::Init(void * Data)
{
	// clear our keys
	this->ResetKeys();
	// we have to do this manually
	this->m_sKeys.m_bCamera_Switch = true;		// start in orbit camera mode
	this->m_sKeys.m_bCamera_CycleNext = false;
	this->m_sKeys.m_bCamera_CyclePrevious = false;
	this->m_sKeys.m_bExtraInfo = false;

// DirectInput
#ifdef INPUT_DIRECTINPUT
// Code taken from Direct Input Example

	this->m_iCameraSwitchCounter = 0;
	this->m_iCameraNextCounter = 0;
	this->m_iCameraPreviousCounter = 0;
	this->m_iExtraInfoCounter = 0;

	this->m_pDIDevice = NULL;
	this->m_pKeyboard = NULL;
	this->m_pMouse = NULL;

	// Set our Direct Input flags
	DWORD diFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

	// Cleanup any previous call first
	HWND hwnd = (HWND) CUFOMain::Get()->PostMessage(MSGID_GET_HWND, NULL);
	KillTimer( hwnd, 0 );    
	this->DestroyDirectInput();

	// Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
	if (   FAILED(  DirectInput8Create(	GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
										IID_IDirectInput8, (VOID**)&this->m_pDIDevice, NULL)  )   )
	{
		return;
	}


////////////////////////
// Mouse

	// Obtain an interface to the system mouse device.
	if( FAILED( this->m_pDIDevice->CreateDevice( GUID_SysMouse, &this->m_pMouse, NULL ) ) )
	{
		OutputDebugString("CInputController::Init - Could not create mouse device\n");
        return;
	}

	// set the data format
	if (  FAILED ( this->m_pMouse->SetDataFormat( &c_dfDIMouse2 ) )  )
	{
		OutputDebugString("CInputController::Init - Could not set data format\n");
        return;
	}

	if( this->m_pMouse->SetCooperativeLevel( hwnd, diFlags ) == DIERR_UNSUPPORTED )
	{
		OutputDebugString("CInputController::Init - Could not set Co-operative Level for mouse\n");
		this->DestroyDirectInput();
		return;
	}

	// aquire the device
	this->m_pMouse->Acquire();

    

////////////////////////
// Keyboard
	
	// Get a pointer to the keyboard device
	if (  FAILED( this->m_pDIDevice->CreateDevice(GUID_SysKeyboard, &this->m_pKeyboard, NULL) )  )
	{
		OutputDebugString("CInputController::Init - Could not create keyboard device\n");
		return;
	}

	// Set our data format for key retrieval using the pre-defined keyboard format in dinput.h
	if (  FAILED( this->m_pKeyboard->SetDataFormat(&c_dfDIKeyboard) )  )
	{
		OutputDebugString("CInputController::Init - Could not set keyboard data format\n");
		return;
	}

	// Send Direct Input our flags
	if (  this->m_pKeyboard->SetCooperativeLevel( hwnd, diFlags ) == DIERR_UNSUPPORTED  )
	{
		OutputDebugString("CInputController::Init - Could not set Co-operative Level for keyboard\n");
		this->DestroyDirectInput();
		return;
	}

	// Grab the keyboards `focus'
	this->m_pKeyboard->Acquire();

	// Set the time for each Direct Input poll of the devices
	// Poll 30 times a second
    SetTimer( hwnd, 0, 1000 / 30, NULL );


#endif // #ifdef INPUT_DIRECTPLAY
}


void CInputController::Destroy()
{
#ifdef INPUT_DIRECTINPUT
	this->DestroyDirectInput();
#endif // #ifdef INPUT_DIRECTINPUT
}


void CInputController::ResetKeys()
{
	// clear our keys
	this->m_sKeys.m_bCamera_Up = 0.0f;
	this->m_sKeys.m_bCamera_Down = 0.0f;
	this->m_sKeys.m_bCamera_Left = 0.0f;
	this->m_sKeys.m_bCamera_Right = 0.0f;
	this->m_sKeys.m_bCamera_ZoomIn = 0.0f;
	this->m_sKeys.m_bCamera_ZoomOut = 0.0f;
//	this->m_sKeys.m_bCamera_Switch = false;

	this->m_sKeys.m_bShip_StrafeUp = 0.0f;
	this->m_sKeys.m_bShip_StrafeDown = 0.0f;
	this->m_sKeys.m_bShip_StrafeLeft = 0.0f;
	this->m_sKeys.m_bShip_StrafeRight = 0.0f;
	this->m_sKeys.m_bShip_TurnUp = 0.0f;
	this->m_sKeys.m_bShip_TurnDown = 0.0f;
	this->m_sKeys.m_bShip_TurnLeft = 0.0f;
	this->m_sKeys.m_bShip_TurnRight = 0.0f;
	this->m_sKeys.m_bShip_RollLeft = 0.0f;
	this->m_sKeys.m_bShip_RollRight = 0.0f;
	this->m_sKeys.m_bShip_Accel = 0.0f;
	this->m_sKeys.m_bShip_Decel = 0.0f;
	this->m_sKeys.m_bShip_Fire1 = false;
	this->m_sKeys.m_bShip_Fire2 = false;

	this->m_sKeys.m_bCamera_CycleNext = false;
	this->m_sKeys.m_bCamera_CyclePrevious = false;

//	this->m_sKeys.m_bExtraInfo = false;

	this->m_sKeys.m_bQuit = false;

	this->m_sKeys.m_dMouseX = 0.0;
	this->m_sKeys.m_dMouseY = 0.0;
	this->m_sKeys.m_dMouseScroll = 0.0;
	this->m_sKeys.m_bMouseButton1 = false;
	this->m_sKeys.m_bMouseButton2 = false;
}

void CInputController::Update( void * Data )
{
#ifdef INPUT_DIRECTINPUT

	// reset our keys
	this->ResetKeys();

/////////////////////////
// Mouse

	HRESULT hr;
	DIMOUSESTATE2 dims2;

	if( this->m_pMouse == NULL ) 
	{
		OutputDebugString("CInputController::Update - Mouse device == NULL\n");
        return;
	}

	// get the mouse input
	ZeroMemory( &dims2, sizeof(dims2) );
    if( FAILED( this->m_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 ) ) )
	{
		// If input is lost then acquire and keep trying 
        hr = this->m_pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = this->m_pMouse->Acquire();
	}

	// store the input values
	this->m_sKeys.m_dMouseX = dims2.lX;
	this->m_sKeys.m_dMouseY = dims2.lY;
	this->m_sKeys.m_dMouseScroll = dims2.lZ;

	// Update the mouse input
	float mouseval = 0.0f;
	if ( this->m_sKeys.m_dMouseX != 0.0f )
	{
		mouseval = (float)this->m_sKeys.m_dMouseX * this->s_Input_Mouse_RollScale;
		if ( mouseval > 100.0f )
			mouseval = 100.0f;
		else if ( mouseval < -100.0f )
			mouseval = -100.0f;

		if ( mouseval > 0 )
			this->m_sKeys.m_bShip_RollRight = abs(mouseval);
		else
			this->m_sKeys.m_bShip_RollLeft = abs(mouseval);
	}

	if ( this->m_sKeys.m_dMouseY != 0.0f )
	{
		mouseval = (float)this->m_sKeys.m_dMouseY * this->s_Input_Mouse_PitchScale;
		if ( mouseval > 100.0f )
			mouseval = 100.0f;
		else if ( mouseval < -100.0f )
			mouseval = -100.0f;

		if ( mouseval > 0 )
			this->m_sKeys.m_bShip_TurnUp = abs(mouseval);
		else
			this->m_sKeys.m_bShip_TurnDown = abs(mouseval);
	}


	if (dims2.rgbButtons[0] & 0x80)
	{
//		OutputDebugString("CInputController::Update - Mouse Button 1\n");
		this->m_sKeys.m_bMouseButton1 = true;
		this->m_sKeys.m_bShip_Fire1 = true;
	}
	//if (dims2.rgbButtons[1] & 0x80)
	//{
	//	OutputDebugString("CInputController::Update - Mouse Button 2\n");
	//	this->m_sKeys.m_bMouseButton2 = true;
	//}

/////////////////////////
// Keyboard

	BYTE diKeys[256];

	if( this->m_pKeyboard == NULL ) 
	{
		OutputDebugString("CInputController::Update - Keyboard device == NULL\n");
        return;
	}


	// poll the device
	if (  FAILED ( this->m_pKeyboard->Poll() )  )
	{
//		OutputDebugString("CInputController::Update - Polling failed\n");
		// try to re-aquire the device and poll again
		this->m_pKeyboard->Acquire();

		if (  FAILED ( this->m_pKeyboard->Poll() )  )
		{
//			OutputDebugString("CInputController::Update - Polling failed a second time\n");
			return;
		}
	}

	// get the keys from the device
	ZeroMemory( diKeys, sizeof(diKeys) );
	if (  FAILED( this->m_pKeyboard->GetDeviceState( sizeof(diKeys), diKeys ) )  ) 
	{
		// If input is lost then acquire and keep trying 
		hr = this->m_pKeyboard->Acquire();
		while( hr == DIERR_INPUTLOST ) 
			hr = this->m_pKeyboard->Acquire();
		return; 
	}

	// get our keys
	if ( diKeys[CInputController::SHIP_STRAFEUP] & 0x80)
		this->m_sKeys.m_bShip_StrafeUp = this->s_Input_KB_ShipStrafeAmount;
	if ( diKeys[CInputController::SHIP_STRAFEDOWN] & 0x80)
		this->m_sKeys.m_bShip_StrafeDown = this->s_Input_KB_ShipStrafeAmount;
	if ( diKeys[CInputController::SHIP_STRAFELEFT] & 0x80)
		this->m_sKeys.m_bShip_StrafeLeft = this->s_Input_KB_ShipStrafeAmount;
	if ( diKeys[CInputController::SHIP_STRAFERIGHT] & 0x80)
		this->m_sKeys.m_bShip_StrafeRight = this->s_Input_KB_ShipStrafeAmount;

	if ( diKeys[CInputController::SHIP_TURNUP] & 0x80)
		this->m_sKeys.m_bShip_TurnUp = this->s_Input_KB_ShipPitchAmount;
	if ( diKeys[CInputController::SHIP_TURNDOWN] & 0x80)
		this->m_sKeys.m_bShip_TurnDown = this->s_Input_KB_ShipPitchAmount;
	if ( diKeys[CInputController::SHIP_TURNLEFT] & 0x80)
		this->m_sKeys.m_bShip_TurnLeft = this->s_Input_KB_ShipYawAmount;
	if ( diKeys[CInputController::SHIP_TURNRIGHT] & 0x80)
		this->m_sKeys.m_bShip_TurnRight = this->s_Input_KB_ShipYawAmount;

	if ( diKeys[CInputController::SHIP_ROLLLEFT] & 0x80)
		this->m_sKeys.m_bShip_RollLeft = this->s_Input_KB_ShipRollAmount;
	if ( diKeys[CInputController::SHIP_ROLLRIGHT] & 0x80)
		this->m_sKeys.m_bShip_RollRight = this->s_Input_KB_ShipRollAmount;

	if ( diKeys[CInputController::SHIP_ACCELERATE] & 0x80)
		this->m_sKeys.m_bShip_Accel = this->s_Input_KB_ShipAccelAmount;
	if ( diKeys[CInputController::SHIP_DECELERATE] & 0x80)
		this->m_sKeys.m_bShip_Decel = this->s_Input_KB_ShipDecelAmount;

	if ( diKeys[CInputController::SHIP_FIRE1] & 0x80)
		this->m_sKeys.m_bShip_Fire1 = true;
	if ( diKeys[CInputController::SHIP_FIRE2] & 0x80)
		this->m_sKeys.m_bShip_Fire2 = true;



	if ( diKeys[CInputController::HUD_EXTRA_INFO] & 0x80 )
	{
		this->m_iExtraInfoCounter++;
		if (this->m_iExtraInfoCounter == 1)
			this->m_sKeys.m_bExtraInfo = !this->m_sKeys.m_bExtraInfo;
	}
	else
		this->m_iExtraInfoCounter = 0;



	if ( diKeys[CInputController::CAMERA_SWITCH] & 0x80 )
	{
		this->m_iCameraSwitchCounter++;
		if (this->m_iCameraSwitchCounter == 1)
			this->m_sKeys.m_bCamera_Switch = !this->m_sKeys.m_bCamera_Switch;
	}
	else
		this->m_iCameraSwitchCounter = 0;

	if ( diKeys[CInputController::CAMERA_CYCLE_NEXT] & 0x80 )
	{
		this->m_iCameraNextCounter++;
		if (this->m_iCameraNextCounter == 1)
			this->m_sKeys.m_bCamera_CycleNext = true;//!this->m_sKeys.m_bCamera_CycleNext;
	}
	else
		this->m_iCameraNextCounter = 0;


	if ( diKeys[CInputController::CAMERA_CYCLE_PREVIOUS] & 0x80 )
	{
		this->m_iCameraPreviousCounter++;
		if (this->m_iCameraPreviousCounter == 1)
			this->m_sKeys.m_bCamera_CyclePrevious = true;//!this->m_sKeys.m_bCamera_CyclePrevious;
	}
	else
		this->m_iCameraPreviousCounter = 0;



	if ( diKeys[CInputController::CAMERA_LEFT] & 0x80)
		this->m_sKeys.m_bCamera_Left = this->s_Input_KB_CameraOrbitAmount;
	if ( diKeys[CInputController::CAMERA_RIGHT] & 0x80)
		this->m_sKeys.m_bCamera_Right = this->s_Input_KB_CameraOrbitAmount;
	if ( diKeys[CInputController::CAMERA_UP] & 0x80)
		this->m_sKeys.m_bCamera_Up = this->s_Input_KB_CameraElevateAmount;
	if ( diKeys[CInputController::CAMERA_DOWN] & 0x80)
		this->m_sKeys.m_bCamera_Down = this->s_Input_KB_CameraElevateAmount;
	if ( diKeys[CInputController::CAMERA_ZOOM_IN] & 0x80)
		this->m_sKeys.m_bCamera_ZoomIn = this->s_Input_KB_CameraZoomAmount;
	if ( diKeys[CInputController::CAMERA_ZOOM_OUT] & 0x80)
		this->m_sKeys.m_bCamera_ZoomOut = this->s_Input_KB_CameraZoomAmount;

	if ( diKeys[CInputController::QUIT] & 0x80)
		this->m_sKeys.m_bQuit = true;

#endif // #ifdef INPUT_DIRECTINPUT
}

#ifdef INPUT_DIRECTINPUT
void CInputController::DestroyDirectInput()
{
	// Free our device contexts
	// Keyboard
    if( this->m_pKeyboard != NULL )
	{
        this->m_pKeyboard->Unacquire();
		this->m_pKeyboard->Release();
		this->m_pKeyboard = NULL;
	}

	// Mouse
	if( this->m_pMouse != NULL )
	{
        this->m_pMouse->Unacquire();
		this->m_pMouse->Release();
		this->m_pMouse = NULL;
	}

	// Free our main DirectInput device
	if ( this->m_pDIDevice != NULL )
	{
		this->m_pDIDevice->Release();
		this->m_pDIDevice = NULL;
	}
}
#endif // #ifdef INPUT_DIRECTINPUT




/////////////////////////////////////////////
// Handle a key being pressed
void CInputController::KeyPressed(void * Data)
{
/////////////////////////////////////
// I havent bothered to put in windows _mouse_ events since
// i just found out that we get the marks for it if we just
// implement direct input!
// Anyway.. windows events for mouse!! bleugh!
/////////////////////////////////////
#ifdef INPUT_WINEVENT
	//////////////////////////
	// Windows Events

	WPARAM keystroke = *(WPARAM*)Data;
	assert(keystroke);

	// this isnt the neatest.. but since we need to put the values in a struct
	// so that we can have different keys for a function, this is the only simple
	// way..
	switch (keystroke)
	{
		// if its a key we care about, update the keys struct
		case CInputController::SHIP_STRAFEUP:
			this->m_sKeys.m_bShip_StrafeUp = this->s_Input_KB_ShipStrafeAmount; break;
		case CInputController::SHIP_STRAFEDOWN:
			this->m_sKeys.m_bShip_StrafeDown = this->s_Input_KB_ShipStrafeAmount; break;
		case CInputController::SHIP_STRAFELEFT:
			this->m_sKeys.m_bShip_StrafeLeft = this->s_Input_KB_ShipStrafeAmount; break;
		case CInputController::SHIP_STRAFERIGHT:
			this->m_sKeys.m_bShip_StrafeRight = this->s_Input_KB_ShipStrafeAmount; break;

		case CInputController::SHIP_TURNUP:
			this->m_sKeys.m_bShip_TurnUp = this->s_Input_KB_ShipPitchAmount; break;
		case CInputController::SHIP_TURNDOWN:
			this->m_sKeys.m_bShip_TurnDown = this->s_Input_KB_ShipPitchAmount; break;
		case CInputController::SHIP_TURNLEFT:
			this->m_sKeys.m_bShip_TurnLeft = this->s_Input_KB_ShipYawAmount; break;
		case CInputController::SHIP_TURNRIGHT:
			this->m_sKeys.m_bShip_TurnRight = this->s_Input_KB_ShipYawAmount; break;

		case CInputController::SHIP_ROLLLEFT:
			this->m_sKeys.m_bShip_RollLeft = this->s_Input_KB_ShipRollAmount; break;
		case CInputController::SHIP_ROLLRIGHT:
			this->m_sKeys.m_bShip_RollRight = this->s_Input_KB_ShipRollAmount; break;

		case CInputController::SHIP_ACCELERATE:
			this->m_sKeys.m_bShip_Accel = this->s_Input_KB_ShipAccelAmount; break;
		case CInputController::SHIP_DECELERATE:
			this->m_sKeys.m_bShip_Decel = this->s_Input_KB_ShipDecelAmount; break;

		case CInputController::SHIP_FIRE1:
			this->m_sKeys.m_bShip_Fire1 = true; break;
		case CInputController::SHIP_FIRE2:
			this->m_sKeys.m_bShip_Fire2 = true; break;

		case CInputController::HUD_EXTRA_INFO:	// switch between the different modes
			this->m_sKeys.m_bExtraInfo = !this->m_sKeys.m_bExtraInfo; break;

		case CInputController::CAMERA_SWITCH:	// switch between the different modes
			this->m_sKeys.m_bCamera_Switch = !this->m_sKeys.m_bCamera_Switch; break;
		case CInputController::CAMERA_CYCLE_NEXT:
			this->m_sKeys.m_bCamera_CycleNext = true; break;//!this->m_sKeys.m_bCamera_CycleNext; break;
		case CInputController::CAMERA_CYCLE_PREVIOUS:
			this->m_sKeys.m_bCamera_CyclePrevious = true; break;//!this->m_sKeys.m_bCamera_CyclePrevious; break;
		case CInputController::CAMERA_LEFT:
			this->m_sKeys.m_bCamera_Left = this->s_Input_KB_CameraOrbitAmount; break;
		case CInputController::CAMERA_RIGHT:
			this->m_sKeys.m_bCamera_Right = this->s_Input_KB_CameraOrbitAmount; break;
		case CInputController::CAMERA_UP:
			this->m_sKeys.m_bCamera_Up = this->s_Input_KB_CameraElevateAmount; break;
		case CInputController::CAMERA_DOWN:
			this->m_sKeys.m_bCamera_Down = this->s_Input_KB_CameraElevateAmount; break;
		case CInputController::CAMERA_ZOOM_IN:
			this->m_sKeys.m_bCamera_ZoomIn = this->s_Input_KB_CameraZoomAmount; break;
		case CInputController::CAMERA_ZOOM_OUT:
			this->m_sKeys.m_bCamera_ZoomOut = this->s_Input_KB_CameraZoomAmount; break;

		case CInputController::QUIT:
			this->m_sKeys.m_bQuit = true;
			//SendMessage(hWnd, WM_CLOSE, 0,0);
			break;

		default:
			break;									// its not a key we care about
	}

#endif // #ifdef WINEVENT_INPUT

	return;
}





/////////////////////////////////////////////
// Handle a key being released
void CInputController::KeyReleased(void * Data)
{
#ifdef INPUT_WINEVENT
	//////////////////////////
	// Windows Events

	WPARAM keystroke = *(WPARAM*)Data;
	assert(keystroke);

	switch (keystroke)
	{
		// if its a key we care about, update the keys struct
		case CInputController::SHIP_STRAFEUP:
			this->m_sKeys.m_bShip_StrafeUp = 0.0f; break;
		case CInputController::SHIP_STRAFEDOWN:
			this->m_sKeys.m_bShip_StrafeDown = 0.0f; break;
		case CInputController::SHIP_STRAFELEFT:
			this->m_sKeys.m_bShip_StrafeLeft = 0.0f; break;
		case CInputController::SHIP_STRAFERIGHT:
			this->m_sKeys.m_bShip_StrafeRight = 0.0f; break;

		case CInputController::SHIP_TURNUP:
			this->m_sKeys.m_bShip_TurnUp = 0.0f; break;
		case CInputController::SHIP_TURNDOWN:
			this->m_sKeys.m_bShip_TurnDown = 0.0f; break;
		case CInputController::SHIP_TURNLEFT:
			this->m_sKeys.m_bShip_TurnLeft = 0.0f; break;
		case CInputController::SHIP_TURNRIGHT:
			this->m_sKeys.m_bShip_TurnRight = 0.0f; break;

		case CInputController::SHIP_ROLLLEFT:
			this->m_sKeys.m_bShip_RollLeft = 0.0f; break;
		case CInputController::SHIP_ROLLRIGHT:
			this->m_sKeys.m_bShip_RollRight = 0.0f; break;

		case CInputController::SHIP_ACCELERATE:
			this->m_sKeys.m_bShip_Accel = 0.0f; break;
		case CInputController::SHIP_DECELERATE:
			this->m_sKeys.m_bShip_Decel = 0.0f; break;

		case CInputController::SHIP_FIRE1:
			this->m_sKeys.m_bShip_Fire1 = false; break;
		case CInputController::SHIP_FIRE2:
			this->m_sKeys.m_bShip_Fire2 = false; break;

//		case CInputController::CAMERA_SWITCH:
//			this->m_sKeys.m_bCamera_Switch = false; break;
		case CInputController::CAMERA_LEFT:
			this->m_sKeys.m_bCamera_Left = 0.0f; break;
		case CInputController::CAMERA_RIGHT:
			this->m_sKeys.m_bCamera_Right = 0.0f; break;
		case CInputController::CAMERA_UP:
			this->m_sKeys.m_bCamera_Up = 0.0f; break;
		case CInputController::CAMERA_DOWN:
			this->m_sKeys.m_bCamera_Down = 0.0f; break;
		case CInputController::CAMERA_ZOOM_IN:
			this->m_sKeys.m_bCamera_ZoomIn = 0.0f; break;
		case CInputController::CAMERA_ZOOM_OUT:
			this->m_sKeys.m_bCamera_ZoomOut = 0.0f; break;

		case CInputController::QUIT:
			this->m_sKeys.m_bQuit = false;
			//SendMessage(hWnd, WM_CLOSE, 0,0);
			break;

		default:
			break;									// its not a key we care about
	}

#endif // #ifdef WINEVENT_INPUT

	return;
}