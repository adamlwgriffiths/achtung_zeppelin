#include "UFOMain.h"

////////////////////////////////////
// Static Variables

bool	CUFOMain::m_bFlatWorld			= UFO_WORLDDATA_FLATWORLD;
int		CUFOMain::m_iTexSize			= UFO_WORLDDATA_TEXTURESIZE;
float	CUFOMain::m_fMeshHeightScale	= UFO_WORLDDATA_MESHHEIGHTSCALE;
int		CUFOMain::m_iAmplitude			= UFO_WORLDDATA_FRACTALAMPLITUDE;
float	CUFOMain::m_fRoughness			= UFO_WORLDDATA_FRACTALROUGHNESS;

		// Flat World
int		CUFOMain::m_iMeshWidth			= UFO_WORLDDATA_MESHWIDTH;
int		CUFOMain::m_iMeshLength			= UFO_WORLDDATA_MESHLENGTH;
int		CUFOMain::m_iMeshSpacing		= UFO_WORLDDATA_MESHSPACING;

		// Spherical World
float	CUFOMain::m_fRadius				= UFO_WORLDDATA_RADIUS;
int		CUFOMain::m_iNumSlices			= UFO_WORLDDATA_NUMSLICES;
int		CUFOMain::m_iNumStacks			= UFO_WORLDDATA_NUMSTACKS;

bool	CUFOMain::m_bFriendlyFire		= UFO_GAME_FRIENDLYFIRE;
bool	CUFOMain::m_bDemoMode			= UFO_GAME_DEMOMODE;

int		CUFOMain::m_iNumCities			= UFO_WORLDDATA_NUMINITIALCITIES;
int		CUFOMain::m_iMaxNumShips		= UFO_GAME_NUMSHIPS;
int		CUFOMain::m_iNumZeppelins		= UFO_GAME_NUMZEPPELINS;


CUFOMain * CUFOMain::ufomain = NULL;

////////////////////////////////////


void * CUFOMain::PostMessage(int ID, void * Data)
{
	void * result = 0;

	HRESULT hResult;
	INT intResult;

	switch (ID)
	{
		case MSGID_INIT:
			hResult = this->Create( *(HINSTANCE*)Data);		// initialise DX app
			result = (void*)&hResult;
			break;
		case MSGID_UPDATE:
			intResult = this->Run();							// run the DX app
			result = (void*)&intResult;
			break;

		case MSGID_RESTART:
			this->Restart();
			break;

		case MSGID_GET_TIMEDELTA:						// get the time delta
			result = (void*)&this->m_fTimeDelta;
			break;
		case MSGID_GET_TIME:								// get the current time
			result = (void*)&this->m_fTime;
			break;

		case MSGID_SET_HINST:
			hInst = *(HINSTANCE*)Data;
			break;
		case MSGID_GET_HWND:
			result = this->m_hWnd;
			break;

		case MSGID_GET_D3DDEVICE:
			result = (void*)this->m_pd3dDevice;
			break;

		case MSGID_GET_SCREENWIDTH:
			result = (void*)&m_d3dsdBackBuffer.Width;
			break;
		case MSGID_GET_SCREENHEIGHT:
			result = (void*)&m_d3dsdBackBuffer.Height;
			break;

		case MSGID_GET_CAPS:
			result = &this->m_d3dCaps;
			break;

		case MSGID_GET_CLEARCOLOUR:
			result = &this->m_kClearColour;
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}

//using namespace UFO;		// allow access to the #defines

CUFOMain::CUFOMain()
{
	// Setup any variables the D3DApp wants
	this->m_strWindowTitle	= _T("Achtung! Zeppelin!");

	this->m_d3dEnumeration.AppUsesDepthBuffer = TRUE;

	this->m_dwCreationWidth   = UFOMAIN_INITIAL_SCREENWIDTH;
	this->m_dwCreationHeight  = UFOMAIN_INITIAL_SCREENHEIGHT;

	this->m_kClearColour = NSColouri( 204, 234, 245 );
}

CUFOMain::~CUFOMain()
{
}

void CUFOMain::Open()
{
	if ( ufomain == NULL )
		ufomain = new CUFOMain();
	else
		assert(0);
}

void CUFOMain::Close()
{
	if ( ufomain != NULL )
		delete ufomain;
	else
		assert(0);
}

CTask * CUFOMain::Get()
{
	if ( ufomain == NULL)
		assert(0);

	return ufomain;
}



////////////////////////////////////////////////////
//           BEGIN UGLY DX FUNCTIONS

/**
* Called during device initialization, this code checks the device
* for some minimum set of capabilities
*/
HRESULT CUFOMain::ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT)
{
	// we dont have any special requirements, so anything will do
	return S_OK;
}

/**
* Called during initial app startup, this function performs all the
* permanent initialization.
*/
HRESULT CUFOMain::OneTimeSceneInit()
{
	// since I dont have time to re-write the DX crap, Ill have to fit in with their flow..
	// ``when in redmond..''
	CGameController::Open();									// open the game controller singleton

	// we dont have any fancy menus so we can just initialise the entire game at this time
	CGameController::Get()->PostMessage(MSGID_INIT, NULL);		// initialise the game controller singleton

	return S_OK;
}





/**
* Initialise objects that are dependent on the device,
* ie, textures, vertex buffers etc.
*/
HRESULT CUFOMain::InitDeviceObjects()
{
	// Setup our Game Controller
	CGameController::Get()->PostMessage(MSGID_INIT_DEVICE_OBJECTS, NULL);

	// Turn on the zbuffer
//	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	// Turn on ambient lighting 
//	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );


	return S_OK;
}

/**
* Called to delete anything created by InitDeviceObjects
*/
HRESULT CUFOMain::DeleteDeviceObjects()
{
	CGameController::Get()->PostMessage(MSGID_DELETE_DEVICE_OBJECTS, NULL);

	return S_OK;
}





/**
* Called if the device is destroyed and we need to
* re-initialise our device dependent objects.
*/
HRESULT CUFOMain::RestoreDeviceObjects()
{
	CGameController::Get()->PostMessage(MSGID_RESTORE_DEVICE_OBJECTS, NULL);

	return S_OK;
}

/**
* Called to delete anything created by RestoreDeviceObjects
*/
HRESULT CUFOMain::InvalidateDeviceObjects()
{
	CGameController::Get()->PostMessage(MSGID_INVALIDATE_DEVICE_OBJECTS, NULL);

	return S_OK;
}




/**
* Called each frame to update the world.
*/
HRESULT CUFOMain::Update()
{
	// make sure our time delta isnt too big, just incase were on a slow computer or were DEBUGGING!
	if ( this->m_fTimeDelta > UFO_MAX_TIMEDELTA )
		this->m_fTimeDelta = UFO_MAX_TIMEDELTA;

	CGameController::Get()->PostMessage(MSGID_UPDATE, &this->m_fTimeDelta);

	return S_OK;
}


void CUFOMain::Restart()
{
	CGameController::Get()->PostMessage(MSGID_DELETE_DEVICE_OBJECTS, NULL);
	CGameController::Get()->PostMessage(MSGID_DESTROY, NULL);
	CGameController::Get()->PostMessage(MSGID_INIT, NULL);
	CGameController::Get()->PostMessage(MSGID_INIT_DEVICE_OBJECTS, NULL);
}



/**
* Called each frame to render the world.
*/
HRESULT CUFOMain::Render()
{
    // Clear the backbuffer and the zbuffer
	this->m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET  |  D3DCLEAR_ZBUFFER, 
							D3DCOLOR_XRGB( m_kClearColour.r, m_kClearColour.g, m_kClearColour.b ), 1.0f, 0 );

	// attempt to render the scene
	if(  SUCCEEDED( this->m_pd3dDevice->BeginScene() )  )
    {
		// set our vertice type so D3D can understand them
		// why is DirectX so low level one minute.. and then so annoyingly high level the next?!
//		this->m_pd3dDevice->SetFVF( D3DFVF_NSVERTEX );

		CGameController::Get()->PostMessage(MSGID_RENDER, NULL);

#ifdef DISPLAY_FRAMERATE
	// FIXME: display frame rate
#endif

		// finish rendering the scene
		this->m_pd3dDevice->EndScene();
	}

	// ``Present'' is called by the D3D Framework

	return S_OK;
}




/**
* Called just before app exit, used to clear any remaining objects.
*/
HRESULT CUFOMain::FinalCleanup()
{
	// Destroy all of our objects
	CGameController::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Close all of our singletons
	CGameController::Close();									// open the game controller singleton

	return S_OK;
}


//              END UGLY DX FUNCTIONS
/////////////////////////////////////////////////////







/********************************************************************
 * Main entry point, just creates an instance of our D3D app and
 * returns the final result back to the OS.
 ********************************************************************/
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE,  LPSTR, INT )
{
	CUFOMain::Open();				// open our singleton

	// send the game our HINSTANCE
	CUFOMain::Get()->PostMessage(MSGID_SET_HINST, (void*)&hInst);

	InitCommonControls();			// yet another shitty D3DApp function.. will they ever go away?!
									// actually its a fekin COMM function! grrr!

	if (  FAILED( *(HRESULT*)CUFOMain::Get()->PostMessage(MSGID_INIT, (void*)&hInst) )  )
		return 0;

	return *(INT*)CUFOMain::Get()->PostMessage(MSGID_UPDATE, NULL); //Run();
}
