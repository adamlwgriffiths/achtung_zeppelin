/******************************************************************************************
 *                                                                                        *
 *                Filename: InputController.h                                             *
 *                  Author: Adam Griffiths                                                *
 *             Description: Handles the dispatching of any input received.                *
 *                                                                                        *
 ******************************************************************************************/

#ifndef UFO_INPUTCONTROLLER_H
#define UFO_INPUTCONTROLLER_H

#include "UFO.h"


class CInputController : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CInputController();
		~CInputController();

		// Singleton functions
		static void		Open();							// creates the singleton object
		static void		Close();						// destroys the singleton object
		static CTask *	Get();							// returns an instance of this singleton

		struct Keys
		{
			float	m_bShip_StrafeLeft;
			float	m_bShip_StrafeRight;
			float	m_bShip_StrafeUp;
			float	m_bShip_StrafeDown;
			float	m_bShip_TurnLeft;
			float	m_bShip_TurnRight;
			float	m_bShip_TurnUp;
			float	m_bShip_TurnDown;
			float	m_bShip_RollLeft;
			float	m_bShip_RollRight;
			float	m_bShip_Accel;
			float	m_bShip_Decel;
			bool	m_bShip_Fire1;
			bool	m_bShip_Fire2;
			bool	m_bCamera_Switch;
			float	m_bCamera_Left;
			float	m_bCamera_Right;
			float	m_bCamera_Up;
			float	m_bCamera_Down;
			float	m_bCamera_ZoomIn;
			float	m_bCamera_ZoomOut;

			bool	m_bCamera_CycleNext;
			bool	m_bCamera_CyclePrevious;

			bool	m_bExtraInfo;

			bool	m_bQuit;

			double	m_dMouseX;
			double	m_dMouseY;
			double	m_dMouseScroll;
			bool	m_bMouseButton1;
			bool	m_bMouseButton2;
		};

	protected:

		void		Init(void * Data);
		void		Update( void * Data );
		void		Destroy();

		void		ResetKeys();

		void		KeyPressed(void * Data);				// sets the appropriate boolean to true
		void		KeyReleased(void * Data);				// sets the appropriate boolean to false

		Keys		m_sKeys;

		static CInputController *	pInputController;

		// Key definitions
#ifndef INPUT_DIRECTINPUT
		// Windows Events
		static const WPARAM	SHIP_STRAFEUP	= 'Q';
		static const WPARAM	SHIP_STRAFEDOWN	= 'E';
		static const WPARAM	SHIP_STRAFELEFT	= 'B';//'Z';
		static const WPARAM	SHIP_STRAFERIGHT= 'N';//'C';

		static const WPARAM	SHIP_TURNUP		= VK_DOWN;
		static const WPARAM	SHIP_TURNDOWN	= VK_UP;
		static const WPARAM	SHIP_TURNLEFT	= 'Z';//'B';//'Z';
		static const WPARAM	SHIP_TURNRIGHT	= 'C';//'N';//'C';

		static const WPARAM SHIP_ROLLLEFT	= VK_LEFT;
		static const WPARAM SHIP_ROLLRIGHT	= VK_RIGHT;

		static const WPARAM	SHIP_ACCELERATE	= 'S';
		static const WPARAM	SHIP_DECELERATE	= 'X';

		static const WPARAM	SHIP_FIRE1		= VK_SPACE;
		static const WPARAM	SHIP_FIRE2		= VK_CONTROL;

		static const WPARAM	CAMERA_SWITCH	= 'V';
		static const WPARAM	CAMERA_LEFT		= VK_NUMPAD4;
		static const WPARAM	CAMERA_RIGHT	= VK_NUMPAD6;
		static const WPARAM	CAMERA_UP		= VK_NUMPAD8;
		static const WPARAM	CAMERA_DOWN		= VK_NUMPAD2;
		static const WPARAM	CAMERA_ZOOM_IN	= 'A';
		static const WPARAM	CAMERA_ZOOM_OUT	= 'D';

		static const WPARAM	HUD_EXTRA_INFO	= 'W';

		static const WPARAM CAMERA_CYCLE_NEXT		= 'B';
		static const WPARAM CAMERA_CYCLE_PREVIOUS	= ',';

		static const WPARAM	QUIT			= VK_ESCAPE;

#else // INPUT_DIRECTINPUT
		// Direct Input
		int m_iCameraSwitchCounter;			// used by Direct Input version
		int m_iCameraNextCounter;
		int m_iCameraPreviousCounter;
		int m_iExtraInfoCounter;

		static const int	SHIP_STRAFEUP	= DIK_Q;
		static const int	SHIP_STRAFEDOWN	= DIK_E;
		static const int	SHIP_STRAFELEFT	= DIK_B;//DIK_Z;
		static const int	SHIP_STRAFERIGHT= DIK_N;//DIK_C;

		static const int	SHIP_TURNUP		= DIK_DOWN;
		static const int	SHIP_TURNDOWN	= DIK_UP;
		static const int	SHIP_TURNLEFT	= DIK_Z;//DIK_B;//DIK_Z;
		static const int	SHIP_TURNRIGHT	= DIK_C;//DIK_N;//DIK_C;

		static const int	SHIP_ROLLLEFT	= DIK_LEFT;
		static const int	SHIP_ROLLRIGHT	= DIK_RIGHT;

		static const int	SHIP_ACCELERATE	= DIK_S;
		static const int	SHIP_DECELERATE	= DIK_X;

		static const int	SHIP_FIRE1		= DIK_SPACE;
		static const int	SHIP_FIRE2		= DIK_LCONTROL;

		static const int	CAMERA_SWITCH	= DIK_V;
		static const int	CAMERA_LEFT		= DIK_NUMPAD4;
		static const int	CAMERA_RIGHT	= DIK_NUMPAD6;
		static const int	CAMERA_UP		= DIK_NUMPAD8;
		static const int	CAMERA_DOWN		= DIK_NUMPAD2;
		static const int	CAMERA_ZOOM_IN	= DIK_A;
		static const int	CAMERA_ZOOM_OUT	= DIK_D;

		static const int	HUD_EXTRA_INFO	= DIK_W;

		static const int	CAMERA_CYCLE_NEXT		= DIK_B;
		static const int	CAMERA_CYCLE_PREVIOUS	= DIK_COMMA;

		static const int	QUIT			= DIK_ESCAPE;



		void		DestroyDirectInput();

		LPDIRECTINPUT8			m_pDIDevice;		// The DirectInput object         
		LPDIRECTINPUTDEVICE8	m_pKeyboard;		// The keyboard device 
		LPDIRECTINPUTDEVICE8	m_pMouse;			// The mouse device

#endif // #ifdef INPUT_DIRECTINPUT


		static const float	s_Input_KB_ShipAccelAmount;//		= 75.0f;
		static const float	s_Input_KB_ShipDecelAmount;//		= 75.0f;
		static const float	s_Input_KB_ShipStrafeAmount;//		= 75.0f;
		static const float	s_Input_KB_ShipPitchAmount;//		= 75.0f;
		static const float	s_Input_KB_ShipYawAmount;//		= 75.0f;
		static const float	s_Input_KB_ShipRollAmount;//		= 75.0f;

		static const float	s_Input_KB_CameraOrbitAmount;//	= 75.0f;
		static const float	s_Input_KB_CameraElevateAmount;//	= 75.0f;
		static const float	s_Input_KB_CameraZoomAmount;//		= 75.0f;

		static const float	s_Input_Mouse_RollScale;
		static const float	s_Input_Mouse_PitchScale;

};

#endif // #ifndef UFO_INPUTCONTROLLER_H
