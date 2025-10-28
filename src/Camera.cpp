#include "Camera.h"

const float	CCamera::s_fCockpitZoom				= 0.36f;//0.44f;
const float	CCamera::s_fCockpitLookAtOffset		= 5.3f;


void * CCamera::PostMessage(int ID, void * Data)
{
	void * result = 0;

	NSNode * target;
	NSVector3df vec;

	switch(ID)
	{
		case MSGID_INIT:
			this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			break;

		case MSGID_SET_TARGET:
			// this is done by NSOrbitCamera
			// detach the camera from its parent node
			//target = this->m_pCamera->GetTarget();
			//target->DetachChild( m_pCamera );
			this->m_pCTaskTarget = (CTask*)Data;
			if ( this->m_pCTaskTarget != NULL )
			{
				target = (NSNode*)this->m_pCTaskTarget->PostMessage(MSGID_GET_NODE, NULL);
				this->m_bShowDescription = true;
			}
			else
			{
				target = NULL;
			}

//			target = (NSNode*)( (CTask*)Data )->PostMessage(MSGID_GET_NODE, NULL);
			// set the new target and attach the camera to it
			this->m_pCamera->SetTarget( target );

			// this is done by NSOrbitCamera
			//target->AttachChild( m_pCamera );
			break;


		case MSGID_GET_UPAXIS:
			vec = *m_pCamera->GetUpAxis();
			result = &vec;
			break;
		case MSGID_GET_LOOKAT:
			vec = *m_pCamera->GetLookAt();
			result = &vec;
			break;
		case MSGID_GET_TRANSLATION:
			vec = *m_pCamera->GetTranslation();
			result = &vec;
			break;

		case MSGID_GET_WORLDTRANSLATION:
			vec = *m_pCamera->GetWorldTranslation();
			result = &vec;
			break;

		case MSGID_GET_NODE:
			result = this->m_pCamera;
			break;

		default:
			assert(0);		// should never get here!
			break;
	}

	return result;
}


CCamera::CCamera()
{
}

CCamera::~CCamera()
{
}


void CCamera::Init( void * Data )
{
	NSNode * target = (NSNode*)Data;

	m_fPreviousZoom = 10.0f;
	m_fPreviousElevation = 0.0f;
	m_fPreviousRotation = 0.0f;

	this->m_bOrbitMode = true;
	this->m_pCamera = new NSOrbitCamera( target );

	// attach the camera to the child so that we get our orientation from it

	// set our default values
	this->m_pCamera->SetZoom( 10.0f );
	this->m_pCamera->SetUpAxis( 0.0f, 1.0f, 0.0f );		// this will be incorrect if the node starts with its original up axis in a different direction

	this->m_pCamera->SetElevation( -0.2f );

	// translate to behind the target
	this->m_pCamera->SetRotation( NS_PI );

	this->m_pCTaskTarget = NULL;
	this->m_bShowDescription = false;
}

void CCamera::Update( void * Data )
{
	float fTimeDelta = *(float*)Data;

	// get the players input
	CInputController::Keys keys = *(CInputController::Keys*)CInputController::Get()->PostMessage(MSGID_GET_KEYS, NULL);

	if ( this->m_pCamera->GetTarget() != NULL )
	{
		// if were in orbit mode, then take input and store the values
		if ( this->m_bOrbitMode )
		{
			if (keys.m_bCamera_Right != 0.0f)
				this->m_pCamera->Orbit( -keys.m_bCamera_Right * CAMERA_ORBIT_SCALE * fTimeDelta );
			if (keys.m_bCamera_Left != 0.0f)
				this->m_pCamera->Orbit(  keys.m_bCamera_Left * CAMERA_ORBIT_SCALE * fTimeDelta );
			if (keys.m_bCamera_Up != 0.0f)
				this->m_pCamera->Elevate( -keys.m_bCamera_Up * CAMERA_ORBIT_SCALE * fTimeDelta );
			if (keys.m_bCamera_Down != 0.0f)
				this->m_pCamera->Elevate(  keys.m_bCamera_Down * CAMERA_ORBIT_SCALE * fTimeDelta );

			if ( keys.m_bCamera_ZoomIn != 0.0f )
			{
				// avoid camera jutter and complex code, the margine of error is acceptable
				if ( this->m_pCamera->GetZoom() > CAMERA_ZOOM_MIN )
					this->m_pCamera->Zoom( -keys.m_bCamera_ZoomIn * CAMERA_ZOOM_SCALE * fTimeDelta );
			}
			if (keys.m_bCamera_ZoomOut != 0.0f )
			{
				// avoid camera jutter and complex code, the margine of error is acceptable
				if ( this->m_pCamera->GetZoom() < CAMERA_ZOOM_MAX )
					this->m_pCamera->Zoom(  keys.m_bCamera_ZoomOut * CAMERA_ZOOM_SCALE * fTimeDelta );
			}

			// store the current values so we can go back to them once we switch modes
			m_fPreviousZoom = this->m_pCamera->GetZoom();
			m_fPreviousElevation = this->m_pCamera->GetElevation();
			m_fPreviousRotation = this->m_pCamera->GetRotation();
		}

		// if weve changed modes
		if (keys.m_bCamera_Switch != this->m_bOrbitMode)
		{
			this->m_bOrbitMode = keys.m_bCamera_Switch;

			// if we just switched into orbit mode
			if ( this->m_bOrbitMode )
			{
				this->m_pCamera->SetZoom( this->m_fPreviousZoom  );
				this->m_pCamera->SetElevation( this->m_fPreviousElevation );
				this->m_pCamera->SetRotation( this->m_fPreviousRotation );
				this->m_pCamera->SetLookAt( 0.0f, 0.0f, 0.0f );
			}
			else	// if we just switched to cockpit mode
			{
				// set our values
				this->m_pCamera->SetZoom( this->s_fCockpitZoom );
				this->m_pCamera->SetElevation( CAMERA_COCKPIT_ELEVATION );
				this->m_pCamera->SetRotation( CAMERA_COCKPIT_ROTATION );
				// make the target forward
				NSVector3df templookat = this->m_pCamera->GetFacingVector();
				templookat.SetLength( this->s_fCockpitLookAtOffset );
				this->m_pCamera->SetLookAt( templookat );
			}
		}

	} // if ( this->m_pCamera->GetTarget() != NULL )

	// the camera will get updated with the scene
	// calling this now will ruin our world translations
	// since they were worked our just previous to this
//	this->m_pCamera->Update();


	// update the hud
	CHUD::CameraData camdata;
	camdata.externalcamera = this->m_bOrbitMode;

	// if our ship is dead, dont update
	if ( this->m_pCTaskTarget != NULL )
	{
		if ( ! *(bool*)this->m_pCTaskTarget->PostMessage( MSGID_IS_ALIVE, NULL ) )
		{
			camdata.currentDescription = new char[10];
			camdata.currentDescription = "Dead";
		}
		else
		{
			camdata.currentDescription = (char*)this->m_pCTaskTarget->PostMessage(MSGID_GET_STATE_DESCRIPTION, NULL );
		}
	}
	else
	{
		camdata.currentDescription = new char[1];
		camdata.currentDescription[0] = '\0';
	}

	CHUD::Get()->PostMessage( MSGID_UPDATE_CAMERASTATS, &camdata );
}

void CCamera::Destroy()
{
	// dont delete the node since it will cause our scene graph to die
	this->m_pCTaskTarget = NULL;
}

