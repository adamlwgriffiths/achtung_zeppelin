#include "GameController.h"

CGameController * CGameController::pGameController = NULL;


void * CGameController::PostMessage(int ID, void * Data)
{
	void * result = 0;

	HRESULT hResult;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init(Data);							// intialise game structures
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;
		case MSGID_RENDER:
			this->Render();
			break;

		case MSGID_INIT_DEVICE_OBJECTS:
			hResult = this->InitDeviceObjects();
			result = (void*)&hResult;
			break;
		case MSGID_DELETE_DEVICE_OBJECTS:
			hResult = this->DeleteDeviceObjects();
			result = (void*)&hResult;
			break;
		case MSGID_RESTORE_DEVICE_OBJECTS:
			hResult = this->RestoreDeviceObjects();
			result = (void*)&hResult;
			break;
		case MSGID_INVALIDATE_DEVICE_OBJECTS:
			hResult = this->InvalidateDeviceObjects();
			result = (void*)&hResult;
			break;

		case MSGID_INPUT_KEY_PRESSED:
			CInputController::Get()->PostMessage(MSGID_INPUT_KEY_PRESSED, Data);
			break;
		case MSGID_INPUT_KEY_RELEASED:
			CInputController::Get()->PostMessage(MSGID_INPUT_KEY_RELEASED, Data);
			break;

		case MSGID_GET_CAMERA:
			result = this->m_pCamera;
			break;

		//case MSGID_GET_NODE:
		//	result = this->m_pScene;
		//	break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CGameController::CGameController()
{
}

CGameController::~CGameController()
{
}


void CGameController::Open()
{
	if ( pGameController == NULL )
		pGameController = new CGameController();
	else
		assert(0);
}

void CGameController::Close()
{
	if ( pGameController != NULL )
	{
		delete pGameController;
		pGameController = NULL;
	}
	else
		assert(0);
}

CTask * CGameController::Get()
{
	if ( pGameController == NULL)
		assert(0);

	return pGameController;
}



/**
* Initialise other classes
*/
void CGameController::Init(void * Data)
{
	// create our sound manager
	CSoundController::Open();

	// open our mesh manager
	CMeshManager::Open();

	// create our input handler
	CInputController::Open();

	// create our particle system
	CParticleManager::Open();

	// create our ship controller
	CShipController::Open();

	// create our world manager
	CWorldManager::Open();

	// create our city manager
	CCityManager::Open();
}


/**
* Destroy this object, and pass the message to any objects it created
*/
void CGameController::Destroy()
{
	// close our input manager
	CInputController::Close();

	// close our particle manager
	CParticleManager::Close();

	// close our mesh manager
	CMeshManager::Close();

	// close our ship controller
	CShipController::Close();

	// close our world manager
	CWorldManager::Close();

	// close our city manager
	CCityManager::Close();

	// close our sound manager
	CSoundController::Close();
}



/**
* Called to update our game
*/
void CGameController::Update(void * Data)
{
	// get the time delta
	this->m_fTimeDelta = *(float*)Data;

	// update our input, needed for Direct Input
	CInputController::Get()->PostMessage(MSGID_UPDATE, NULL);

	CParticleManager::Get()->PostMessage(MSGID_UPDATE, &this->m_fTimeDelta);

	// update our ships
	CShipController::Get()->PostMessage(MSGID_UPDATE, &this->m_fTimeDelta);

	// update our cities
	CCityManager::Get()->PostMessage(MSGID_UPDATE, &this->m_fTimeDelta);

	// update our scene nodes
	NSMatrix4 modeltrans;
	modeltrans.SetIdentity();
	NSVector3df trans = NSVector3df( 0.0f, 0.0f, 0.0f );

	// because the camera is part of the scene.. we need to update our WORLD translation before
	// we render things, or the camera will be looking at the previous position, as the render loop
	// occurs AFTER the set view matrix..
	// and if the set view matrix happens after the rendering, it only updates the view for the
	// next loop!!!!

	// now i understand why we need to STORE local AND world translations for nodes!

	// update our camera now so that our look at targets etc will be rotated with the parent nodes this frame
	// this does not cause any lag with the camera since it gets updated along with its parent
	m_pCamera->PostMessage(MSGID_UPDATE, &this->m_fTimeDelta);


	// update the local transforms
	this->m_pScene->Update();

	// update the world transforms
	this->m_pScene->_Update( modeltrans, trans );

	// update our skybox
	// get the camera's translation
	NSNode * camnode = (NSNode*)this->m_pCamera->PostMessage(MSGID_GET_NODE, NULL);
	NSVector3df camtrans = *camnode->GetWorldTranslation();

	//this->m_pSkyBox->SetTranslate( camtrans );

	NSMatrix4 skymat;
	skymat.SetIdentity();
	camtrans.v.y -= 40.0f;
	this->m_pSkyBox->_Update( skymat, camtrans );


	//////////////////////////////////////
	// Perform collision detection

	// check for collisions Plane Vs Plane
	CShipController::Get()->PostMessage(MSGID_CHECK_COLLISIONSSPHEREVSSHIP, NULL);

	// check for collisions Plane Vs Zepplin
	CShipController::Get()->PostMessage(MSGID_CHECK_COLLISIONSSPHEREVSZEPPELIN, NULL);

	// check for collisions Plane Vs World
	CShipController::Get()->PostMessage(MSGID_CHECK_COLLISIONSSPHEREVSGROUND, NULL);

	// check for collisions Plane Vs City
	CShipController::Get()->PostMessage(MSGID_CHECK_COLLISIONSHIPVSCITY, NULL);



	////////////////////////
	// Sound Controller
	CSoundController::Get()->PostMessage( MSGID_UPDATE, &this->m_fTimeDelta );
}




/**
* Called to render our game
*/
void CGameController::Render()
{
	// get the device
	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	// setup our world view matrix based on the camera's position
	this->SetupWorldViewMatrix( pd3dDevice );

	// get the view from the camera last, since its position is updated during the render loop!
	this->SetupCameraViewMatrix( pd3dDevice );


	// set the world transform with an identity matrix
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );

	NSMatrix4 modeltrans;
	modeltrans.SetIdentity();
	NSVector3df trans = NSVector3df( 0.0f, 0.0f, 0.0f );

	pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


	////////////////////////////
	// render our sky box
	// turn off the Z Buffer, turn off Fog turn off lighting
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// get the camera's translation
	NSNode * camnode = (NSNode*)this->m_pCamera->PostMessage(MSGID_GET_NODE, NULL);
	NSVector3df camtrans = *camnode->GetWorldTranslation();

	NSMatrix4 cammat;
	cammat.SetIdentity();
	this->m_pSkyBox->Render( pd3dDevice, cammat, camtrans );

	// turn on the Z Buffer, turn on Fog, turn on lighting
	pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	////////////////////////////


	////////////////////////////
	// Render our water plane
	this->m_pWaterPlane->PostMessage(MSGID_RENDER, NULL);
	////////////////////////////

	////////////////////////////
	// render our world
	CWorldManager::Get()->PostMessage(MSGID_RENDER, NULL);
	////////////////////////////


	////////////////////////////
	// render our scene
	m_pScene->Render( pd3dDevice, modeltrans, trans );
	////////////////////////////

	////////////////////////////
	// render our HUD
	CHUD::Get()->PostMessage(MSGID_RENDER, NULL);
	////////////////////////////
}


void CGameController::SetupCameraViewMatrix( LPDIRECT3DDEVICE9 pd3dDevice )
{
	// get our world view from the camera
	const NSVector3df upAxis = *(NSVector3df*)m_pCamera->PostMessage(MSGID_GET_UPAXIS, NULL);
	const NSVector3df target = *(NSVector3df*)m_pCamera->PostMessage(MSGID_GET_LOOKAT, NULL);
	const NSVector3df translation = *(NSVector3df*)m_pCamera->PostMessage(MSGID_GET_WORLDTRANSLATION, NULL);

	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView,	&D3DXVECTOR3( translation.v.x, translation.v.y, translation.v.z ),	// Camera Translation
									&D3DXVECTOR3( target.v.x, target.v.y, target.v.z),						// Look At Target
									&D3DXVECTOR3( upAxis.v.x, upAxis.v.y, upAxis.v.z) );	// UP Axis
	pd3dDevice->SetTransform( D3DTS_VIEW, &matView );


	// setup our frustrum
	float screenWidth = (float)*(UINT*)CUFOMain::Get()->PostMessage(MSGID_GET_SCREENWIDTH, NULL);
	float screenHeight = (float)*(UINT*)CUFOMain::Get()->PostMessage(MSGID_GET_SCREENHEIGHT, NULL);

	D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, screenWidth / screenHeight, 0.3f, VIEW_DISTANCE );
    pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


void CGameController::SetupWorldViewMatrix( LPDIRECT3DDEVICE9 pd3dDevice )
{
	// setup our world view matrix
    D3DXMATRIXA16 matWorld;
    D3DXMatrixRotationY( &matWorld, 0.0f );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}


void CGameController::SetupFog()
{
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	// Setup our fog
	// Check our capabilities for rendering
	const D3DCAPS9 caps = *(D3DCAPS9*)CUFOMain::Get()->PostMessage(MSGID_GET_CAPS, NULL);

	//const NSColouri colour = *(NSColouri*)CUFOMain::Get()->PostMessage(MSGID_GET_CLEARCOLOUR, NULL);
	// 204, 234, 245

	//D3DCOLOR d3dcolour = D3DCOLOR_XRGB( colour.r, colour.g, colour.b );
	//D3DCOLOR d3dcolour = D3DCOLOR_XRGB( colour.r + 20, colour.g + 20, colour.b + 10 );
	//D3DCOLOR d3dcolour = D3DCOLOR_XRGB( 235, 235, 255 );
	//D3DCOLOR d3dcolour = D3DCOLOR_XRGB( 235, 245, 255 );
//	D3DCOLOR d3dcolour = D3DCOLOR_XRGB( 215, 225, 255 );
	D3DCOLOR d3dcolour = D3DCOLOR_XRGB( 93, 164, 206 );

	float density = 0.0002f;

	//device->SetRenderState( D3DRS_FOGENABLE, TRUE );
	device->SetRenderState( D3DRS_FOGCOLOR, d3dcolour );

//	device->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_EXP );
	device->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_EXP2 );
	device->SetRenderState( D3DRS_FOGDENSITY, *(DWORD *)(&density) );
}


void CGameController::SetupLights()
{
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	// Set our ambient lighting
//	device->SetRenderState( D3DRS_AMBIENT, 0xff0f0f0f );

	NSVector3df dir = NSVector3df( 1.0f, -2.0f, 1.0f );
	dir.Normalise();

	NSVector3df pos = NSVector3df( 0.0f, 0.0f, 0.0f );

	//D3DLIGHT9 dirlight;
	ZeroMemory( &dirlight, sizeof(D3DLIGHT9) );
	dirlight.Type = D3DLIGHT_DIRECTIONAL;
	dirlight.Diffuse.r  = 1.0f;
	dirlight.Diffuse.g  = 1.0f;
	dirlight.Diffuse.b  = 1.0f;
	dirlight.Diffuse.a	= 1.0f;
	dirlight.Position   = pos.v;
	dirlight.Direction  = dir.v;
	dirlight.Range      = 100000.0f;
//	dirlight.Attenuation0 = 0.0f;	//0.1f;
//	dirlight.Falloff = 0.0f;
	device->SetLight( 1, &dirlight );
	device->LightEnable( 1, TRUE );
	device->SetRenderState( D3DRS_LIGHTING, TRUE );
}






////////////////////////////////////////////////////
//           BEGIN UGLY DX FUNCTIONS



HRESULT CGameController::InitDeviceObjects()
{
	///////////////////////////
	// Initialise any Objects

	// Sound Controller
	CSoundController::Get()->PostMessage(MSGID_INIT, NULL);

	// Input Controller
	CInputController::Get()->PostMessage(MSGID_INIT, NULL);


///////////////////////////
// Create our Scene
///////////////////////////

	// setup our ambient lighting
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	// Enable fog
//	device->SetRenderState( D3DRS_FOGENABLE, TRUE );

	this->SetupFog();

	// Setup our lights
	this->SetupLights();
	

/**** Our Scene Hierarchy ****
*
*      ships branch
*                      ,- 0
*        ,- 0 ------ < -- 0  ships
*        |             '- 0
* root   |
*  0 ----+- 0  planet  // this is no longer used
*        |
*        |             ,- 0
*        '- 0 ------ < -- 0  cities
*                      '- 0
*      City branch
*
*******************************/

	// create our scene root
	char buff[25];
	sprintf(buff, "root");
	this->m_pScene = new NSNode( buff, 0.0f, 0.0f, 0.0f );

	// create a branch for our ships
	sprintf(buff, "shipbranch");
	this->m_pShipBranch = new NSNode( buff, 0.0f, 0.0f, 0.0f );

	// create a node for our planet
//	sprintf(buff, "planetbranch");
//	this->m_pPlanetBranch = new NSNode( buff, 0.0f, 0.0f, 0.0f );

	// create a node for our cities
	sprintf(buff, "citybranch");
	this->m_pCityBranch = new NSNode( buff ,0.0f, 0.0f, 0.0f );

	// create a node for our bullets
	sprintf(buff, "bulletbranch");
	this->m_pBulletBranch = new NSNode( buff, 0.0f, 0.0f, 0.0f );


	sprintf(buff, "zeppelinbranch");
	this->m_pZeppelinBranch = new NSNode( buff, 0.0f, 0.0f, 0.0f );

	sprintf(buff, "zeppelinbulletbranch");
	this->m_pZeppelinBulletBranch = new NSNode( buff, 0.0f, 0.0f, 0.0f );


	sprintf(buff, "particlebranch");
	this->m_pParticleBranch = new NSNode( buff ,0.0f, 0.0f, 0.0f );



	// attach our branches
//	this->m_pScene->AttachChild( this->m_pPlanetBranch );
	this->m_pScene->AttachChild( this->m_pShipBranch );
	this->m_pScene->AttachChild( this->m_pCityBranch );
	this->m_pScene->AttachChild( this->m_pBulletBranch );
	this->m_pScene->AttachChild( this->m_pParticleBranch );
	this->m_pScene->AttachChild( this->m_pZeppelinBranch );
	this->m_pScene->AttachChild( this->m_pZeppelinBulletBranch );


	// Initialise our mesh manager so we can use it
	CMeshManager::Get()->PostMessage(MSGID_INIT, NULL);

	// Initialise our particle manager
	CParticleManager::Get()->PostMessage(MSGID_INIT, this->m_pParticleBranch );

	// Initialise our HUD
	CHUD::Get()->PostMessage(MSGID_INIT, NULL);


	// Initialise our planet
	CTask * world;

	CWorldFlat::WorldFlatSetup wfdata;
	//CWorldSphere::WorldSphereSetup wsdata;

	//if ( CUFOMain::m_bFlatWorld )
	//{
		// if flat world
		wfdata = CWorldFlat::WorldFlatSetup();

		wfdata.iTexSize = CUFOMain::m_iTexSize;//this->m_iTexSize;
		wfdata.iMeshWidth = CUFOMain::m_iMeshWidth;//this->m_iMeshWidth;
		wfdata.iMeshLength = CUFOMain::m_iMeshLength;//this->m_iMeshLength;
		wfdata.iMeshSpacing = CUFOMain::m_iMeshSpacing;//this->m_iMeshSpacing;
		wfdata.fMeshHeightScale = CUFOMain::m_fMeshHeightScale;//this->m_fMeshHeightScale;
		wfdata.iAmplitude = CUFOMain::m_iAmplitude;//this->m_iAmplitude;
		wfdata.fRoughness = CUFOMain::m_fRoughness;//this->m_fRoughness;

		world = new CWorldFlat();
		world->PostMessage(MSGID_INIT, &wfdata);
	//}
	//else
	//{
	//	// if spherical world
	//	wsdata = CWorldSphere::WorldSphereSetup();

	//	wsdata.iTexSize = CUFOMain::m_iTexSize;//this->m_iTexSize;
	//	wsdata.fRadius = CUFOMain::m_fRadius;//this->m_fRadius;
	//	wsdata.iNumSlices = CUFOMain::m_iNumSlices;//this->m_iNumSlices;
	//	wsdata.iNumStacks = CUFOMain::m_iNumStacks;//this->m_iNumStacks;
	//	wsdata.fMeshHeightScale = CUFOMain::m_fMeshHeightScale;//this->m_fMeshHeightScale;
	//	wsdata.iAmplitude = CUFOMain::m_iAmplitude;//this->m_iAmplitude;
	//	wsdata.fRoughness = CUFOMain::m_fRoughness;//this->m_fRoughness;

	//	world = new CWorldSphere();
	//	world->PostMessage(MSGID_INIT, &wsdata);
	//}


	CWorldManager::WorldSetup wmdata = CWorldManager::WorldSetup();
	wmdata.pWorld = world;
	wmdata.bIsFlatWorld = CUFOMain::m_bFlatWorld;//this->m_bFlatWorld;
//	wmdata.pPlanetBranch = m_pPlanetBranch;
	CWorldManager::Get()->PostMessage(MSGID_INIT, &wmdata);



	// Initialise our city manager
	CCityManager::Get()->PostMessage(MSGID_INIT, (void*)this->m_pCityBranch);



	// Initialise our ship manager and send it the ships branch
	CShipController::ShipControllerInitData scdata;
	scdata.shipBranch = this->m_pShipBranch;
	scdata.bulletBranch = this->m_pBulletBranch;
	scdata.zeppelinBranch = this->m_pZeppelinBranch;
	scdata.zeppelinBulletBranch = this->m_pZeppelinBulletBranch;
	CShipController::Get()->PostMessage(MSGID_INIT, &scdata );


// our forces are now spawned dynamically, so this entir section is no longer needed
// it remains purely because i want it too... behold its annoyingly large size!



//	// create a player ship
//	NSVector3df trans;
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( 0.0f, wfdata.fMeshHeightScale * wfdata.iAmplitude + 40, 0.0f);	// flat land
//	else
//		trans = NSVector3df( 0.0f, wsdata.fRadius + 180.0f, 0.0f );	// spherical
//	NSVector3df up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	ALLIANCE alliance = FRIENDLY;
//	CShip::InitData shipdata;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = true;
//	NSMatrix4 planepmat;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CTask * ship = (CTask*)CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//	// get the ships node
//	NSNode * shipnode = (NSNode*) ( (CShip*) ship )->PostMessage(MSGID_GET_NODE, NULL);
//
//	// ships are AUTOMATICALLY added to our scene branch
////	this->m_pShipBranch->AttachChild( shipnode );
//
//
//	float flatMinHeight = wfdata.fMeshHeightScale * wfdata.iAmplitude + 100.0f;
//
//	float maxrandomheight = 300.0f;
//	float maxrandomxz = 500.0f;
//
//	float randx, randy, randz;
//
//	///////////////////////////
//	// Friendly Forces
//	// Ship 1
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 20.0f, wsdata.fRadius + 160.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = FRIENDLY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//	// Ship 2
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 60.0f, wsdata.fRadius + 220.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = FRIENDLY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//
//	// Ship 3
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 90.0f, wsdata.fRadius + 240.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = FRIENDLY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//
//
//
//	///////////////////////////
//	// Enemy Forces
//	// Ship 1
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 80.0f, wsdata.fRadius + 240.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = ENEMY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//
//
//	// Ship 2
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 80.0f, wsdata.fRadius + 240.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = ENEMY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);
//
//
//	// Ship 3
//	randy = ( (float)rand() / (float)RAND_MAX ) * maxrandomheight;
//	randy += flatMinHeight;
//	randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randx *= -1.0f;
//	randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
//	if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
//		randz *= -1.0f;
//
//
//	if ( CUFOMain::m_bFlatWorld )
//		trans = NSVector3df( randx, randy, randz);		// flat land
//	else
//		trans = NSVector3df( 120.0f, wsdata.fRadius + 240.0f, 0.0f );	// spherical
//
//	up = NSVector3df( 0.0f, 1.0f, 0.0f );
//	alliance = ENEMY;
//	shipdata.trans = trans;
//	shipdata.up = up;
//	shipdata.alliance = alliance;
//	shipdata.isPlayer = false;
//	planepmat.SetIdentity();
//	planepmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
//	shipdata.mat = planepmat;
//
//	CShipController::Get()->PostMessage(MSGID_CREATE_SHIP, &shipdata);




	//////////////////////////////
	//// Create our zeppelins
	//float fZeppelinHeight = wfdata.fMeshHeightScale * wfdata.iAmplitude + this->s_fZeppelinHeight;

	//CZeppelin::InitData zeppdata;
	//NSVector3df zepptrans;

	////////////////////////
	//// Friendly Zeppelins

	//// Zeppelin 1
	//zeppdata.alliance = FRIENDLY;
	//// create a random Y rotation
	//NSMatrix4 zeppmat;
	//zeppmat.SetIdentity();
	//zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
	//zeppdata.mat = zeppmat;

	//randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randx *= -1.0f;
	//randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randz *= -1.0f;


	//zepptrans = NSVector3df( randx, fZeppelinHeight, randz);		// flat land
	//zeppdata.trans = zepptrans;

	//CShipController::Get()->PostMessage(MSGID_CREATE_ZEPPELIN, &zeppdata);




	//// Zeppelin 2
	//zeppdata.alliance = FRIENDLY;
	//// create a random Y rotation
	//zeppmat.SetIdentity();
	//zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
	//zeppdata.mat = zeppmat;

	//randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randx *= -1.0f;
	//randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randz *= -1.0f;


	//zepptrans = NSVector3df( randx, fZeppelinHeight, randz);		// flat land
	//zeppdata.trans = zepptrans;

	//CShipController::Get()->PostMessage(MSGID_CREATE_ZEPPELIN, &zeppdata);








	////////////////////////
	//// Enemy Zeppelins

	//// Zeppelin 1
	//zeppdata.alliance = ENEMY;
	//// create a random Y rotation
	//zeppmat.SetIdentity();
	//zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
	//zeppdata.mat = zeppmat;

	//randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randx *= -1.0f;
	//randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randz *= -1.0f;


	//zepptrans = NSVector3df( randx, fZeppelinHeight, randz);		// flat land
	//zeppdata.trans = zepptrans;

	//CShipController::Get()->PostMessage(MSGID_CREATE_ZEPPELIN, &zeppdata);





	//// Zeppelin 2
	//zeppdata.alliance = ENEMY;
	//// create a random Y rotation
	//zeppmat.SetIdentity();
	//zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
	//zeppdata.mat = zeppmat;

	//randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randx *= -1.0f;
	//randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
	//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
	//	randz *= -1.0f;


	//zepptrans = NSVector3df( randx, fZeppelinHeight, randz);		// flat land
	//zeppdata.trans = zepptrans;

	//CShipController::Get()->PostMessage(MSGID_CREATE_ZEPPELIN, &zeppdata);


	// create our zeppelins
	CShipController::Get()->PostMessage( MSGID_SPAWN_ZEPPELINS, NULL );



	// update our scene or the spawning will be feked!
	this->m_pScene->Update();
	NSMatrix4 scenemat;
	scenemat.SetIdentity();
	NSVector3df scenetrans = NSVector3df( 0.0f, 0.0f, 0.0f );
	this->m_pScene->_Update( scenemat, scenetrans );




	// create our camera
	this->m_pCamera = new CCamera();
	this->m_pCamera->PostMessage(MSGID_INIT, NULL);//shipnode);



	// spawn our our first wave
	CShipController::Get()->PostMessage(MSGID_SPAWN_SHIPS, NULL);

	CTask * player = (CTask*)CShipController::Get()->PostMessage(MSGID_GET_PLAYERSHIP, NULL);

	// if the player doesnt exist, chose a normal ship
	if ( player != NULL )
	{
		//NSNode * playernode = (NSNode*)player->PostMessage(MSGID_GET_NODE, NULL);
		//this->m_pCamera->PostMessage(MSGID_SET_TARGET, playernode);
		this->m_pCamera->PostMessage( MSGID_SET_TARGET, player );
	}
	else
	{
		CShipController::Get()->PostMessage(MSGID_CYCLE_CAMERA, NULL);
	}



	// create a sky box
	sprintf(buff, "skybox");
	this->m_pSkyBox = new NSNode( buff, 0.0f, 0.0f/*-2.0f*/, 0.0f );

	// get the skybox's mesh
	CMeshManager::MESHES meshtype;
	meshtype = CMeshManager::SKY_BOX;
	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pSkyBox->AttachMesh( mesh );

	NSMatrix4 skymat;
	skymat.SetIdentity();
	this->m_pSkyBox->SetMatrix( skymat );
	this->m_pSkyBox->Update();


	// create our water plane
	this->m_pWaterPlane = new CWaterPlane();
	this->m_pWaterPlane->PostMessage(MSGID_INIT, NULL);




	// assign our ships targets
//	CShipController::Get()->PostMessage( MSGID_ASSIGN_TARGETS, NULL );

	return S_OK;
}


HRESULT CGameController::DeleteDeviceObjects()
{
///////////////////////////
// Destroy our Scene
///////////////////////////


	// Destroy our city manager
	CCityManager::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our planet
	CWorldManager::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our Direct Input Device since were changing window contexts
	CInputController::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our ship manager
	CShipController::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our particle manager
	CParticleManager::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our HUD
	CHUD::Get()->PostMessage(MSGID_DESTROY, NULL);

	// kill our water plane
	this->m_pWaterPlane->PostMessage(MSGID_DESTROY, NULL);
	delete this->m_pWaterPlane;

	// Destroy our mesh manager
	CMeshManager::Get()->PostMessage(MSGID_DESTROY, NULL);

	// Destroy our scene camera
	this->m_pCamera->PostMessage(MSGID_DESTROY, NULL);
	delete this->m_pCamera;


	// Delete our scene after other objects, incase dynamic objects need to kill their scene nodes
	// if we dont, when they get destroyed, they will crash when they try to delete their nodes
	// Destroy our scene
	delete this->m_pScene;			// parent nodes will kill any child nodes for us
	delete this->m_pSkyBox;

	this->m_pShipBranch = NULL;
//	this->m_pPlanetBranch = NULL;
	this->m_pCityBranch = NULL;
	this->m_pScene = NULL;
	this->m_pBulletBranch = NULL;
	this->m_pParticleBranch = NULL;
	this->m_pSkyBox = NULL;
	this->m_pZeppelinBranch = NULL;
	this->m_pZeppelinBulletBranch = NULL;

	// Sound Controller
	CSoundController::Get()->PostMessage(MSGID_DESTROY, NULL);


	return S_OK;
}






HRESULT CGameController::RestoreDeviceObjects()
{
	return S_OK;
}


HRESULT CGameController::InvalidateDeviceObjects()
{
	return S_OK;
}

//              END UGLY DX FUNCTIONS
/////////////////////////////////////////////////////

