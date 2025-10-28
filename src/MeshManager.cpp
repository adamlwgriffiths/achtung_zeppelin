#include "MeshManager.h"


CMeshManager * CMeshManager::meshmanager = NULL;


void * CMeshManager::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_GET_MESH:
			result = this->GetMesh( *(CMeshManager::MESHES*)Data );
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}



CMeshManager::CMeshManager()
{

}

CMeshManager::~CMeshManager()
{
}

void CMeshManager::Open()
{
	if ( meshmanager == NULL )
		meshmanager = new CMeshManager();
	else
		assert(0);
}

void CMeshManager::Close()
{
	assert(meshmanager);

	delete meshmanager;
	meshmanager = NULL;
}

CTask * CMeshManager::Get()
{
	assert(meshmanager);

	return meshmanager;
}





void CMeshManager::Init(void * Data)
{
	// get the device
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9) CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	// load the meshes
	NSColourf friendlyemissive = NSColourf( 0.0f, 0.0f, 0.2f );
	NSColourf enemyemissive = NSColourf( 0.2f, 0.0f, 0.0f );

	NSColourf friendlytraceremissive = NSColourf( 1.0f, 0.4f, 0.0f );	// yellow-ish
	NSColourf enemytraceremissive = NSColourf( 0.7f, 0.0f, 0.7f );		// purple-ish

	//Ships
	this->m_pShipFriend = new NSMesh();
	this->m_pShipFriend->Load( device, "models/plane-fri.x" );
	// they look even darker
//	( (NSMesh*) this->m_pShipFriend )->SetEmissive( friendlyemissive );

	this->m_pShipEnemy = new NSMesh();
	this->m_pShipEnemy->Load( device, "models/plane-ene.x" );
//	( (NSMesh*) this->m_pShipEnemy )->SetEmissive( enemyemissive );

	this->m_pPropeller = new NSMesh();
	this->m_pPropeller->Load( device, "models/propeller.x" );


	// Zeppelins
	this->m_pZeppelinFriend = new NSMesh();
	this->m_pZeppelinFriend->Load( device, "models/blimpfri.x" );
	// make our zeppelins look brighter, the shadows obscure the textures and make them look the same!
	( (NSMesh*) this->m_pZeppelinFriend )->SetEmissive( NSColourf( 0.3f, 0.3f, 0.3f ) );//friendlyemissive );

	this->m_pZeppelinEnemy = new NSMesh();
	this->m_pZeppelinEnemy->Load( device, "models/blimpene.x" );
	( (NSMesh*) this->m_pZeppelinEnemy )->SetEmissive( NSColourf( 0.3f, 0.3f, 0.3f ) );//enemyemissive );


	// Cities
	this->m_pCityFriend1 = new NSMesh();
	this->m_pCityFriend1->Load( device, "models/city1fri.x" );
	( (NSMesh*) this->m_pCityFriend1 )->SetEmissive( friendlyemissive );
	this->m_pCityFriend2 = new NSMesh();
	this->m_pCityFriend2->Load( device, "models/city2fri.x" );
	( (NSMesh*) this->m_pCityFriend2 )->SetEmissive( friendlyemissive );
	this->m_pCityFriend3 = new NSMesh();
	this->m_pCityFriend3->Load( device, "models/city3fri.x" );
	( (NSMesh*) this->m_pCityFriend3 )->SetEmissive( friendlyemissive );


	this->m_pCityEnemy1 = new NSMesh();
	this->m_pCityEnemy1->Load( device, "models/city1ene.x" );
	( (NSMesh*) this->m_pCityEnemy1 )->SetEmissive( enemyemissive );
	this->m_pCityEnemy2 = new NSMesh();
	this->m_pCityEnemy2->Load( device, "models/city2ene.x" );
	( (NSMesh*) this->m_pCityEnemy2 )->SetEmissive( enemyemissive );
	this->m_pCityEnemy3 = new NSMesh();
	this->m_pCityEnemy3->Load( device, "models/city3ene.x" );
	( (NSMesh*) this->m_pCityEnemy3 )->SetEmissive( enemyemissive );


	// Shrapnel
	this->m_pPlaneShrapnel1 = new NSMesh();
	this->m_pPlaneShrapnel1->Load( device, "models/planeshrapnel.x" );
	this->m_pCityShrapnel1 = new NSMesh();
	this->m_pCityShrapnel1->Load( device, "models/cityshrapnel.x" );

	// Tracers
	this->m_pTracerFriend = new NSMesh();
	this->m_pTracerFriend->Load( device, "models/tracer.x" );
	( (NSMesh*) this->m_pTracerFriend )->SetEmissive( friendlytraceremissive );

	this->m_pTracerEnemy = new NSMesh();
	this->m_pTracerEnemy->Load( device, "models/tracer.x" );
	( (NSMesh*) this->m_pTracerEnemy )->SetEmissive( enemytraceremissive );

	// Particles
	this->m_pSmokeBlack = new NSBillboard();
	this->m_pSmokeBlack->Load( device, "models/Smoke.dds" );

	NSColourf smokecolour( 0.0f, 1.0f, 0.0f );
	( (NSBillboard*) this->m_pSmokeBlack )->SetColour( smokecolour );
	( (NSBillboard*) this->m_pSmokeBlack )->SetSize( 20.0f );


	// Skybox
	this->m_pSkyBox = new NSMesh();
	this->m_pSkyBox->Load( device, "models/skybox.x" );
}

void CMeshManager::Destroy()
{
	// destroy our meshes

	// ships
	this->m_pShipFriend->Unload();
	this->m_pShipEnemy->Unload();

	this->m_pPropeller->Unload();

	// zeppelins
	this->m_pZeppelinFriend->Unload();
	this->m_pZeppelinEnemy->Unload();

	// cities
	this->m_pCityFriend1->Unload();
	this->m_pCityFriend2->Unload();
	this->m_pCityFriend3->Unload();

	this->m_pCityEnemy1->Unload();
	this->m_pCityEnemy2->Unload();
	this->m_pCityEnemy3->Unload();

	// shrapnel
	this->m_pCityShrapnel1->Unload();
	this->m_pPlaneShrapnel1->Unload();

	// tracers
	this->m_pTracerFriend->Unload();
	this->m_pTracerEnemy->Unload();

	// particles
	this->m_pSmokeBlack->Unload();

	// skybox
	this->m_pSkyBox->Unload();



	this->m_pShipFriend = NULL;
	this->m_pShipEnemy = NULL;

	this->m_pPropeller = NULL;

	this->m_pZeppelinFriend = NULL;
	this->m_pZeppelinEnemy = NULL;

	this->m_pCityFriend1 = NULL;
	this->m_pCityFriend2 = NULL;
	this->m_pCityFriend3 = NULL;

	this->m_pCityEnemy1 = NULL;
	this->m_pCityEnemy2 = NULL;
	this->m_pCityEnemy3 = NULL;

	this->m_pCityShrapnel1 = NULL;
	this->m_pPlaneShrapnel1 = NULL;

	this->m_pTracerFriend = NULL;
	this->m_pTracerEnemy = NULL;

	this->m_pSmokeBlack = NULL;

	this->m_pSkyBox = NULL;
}




NSElement * CMeshManager::GetMesh( MESHES meshid )
{
	NSElement * result;

	switch (meshid)
	{
		case TRACER_FRIEND:
			result = this->m_pTracerFriend;
			break;
		case TRACER_ENEMY:
			result = this->m_pTracerEnemy;
			break;

		case SHIP_FRIEND:
			result = this->m_pShipFriend;
			break;
		case SHIP_ENEMY:
			result = this->m_pShipEnemy;
			break;

		case SHIP_PROPELLER:
			result = this->m_pPropeller;
			break;

		case ZEPPELIN_FRIEND:
			result = this->m_pZeppelinFriend;
			break;

		case ZEPPELIN_ENEMY:
			result = this->m_pZeppelinEnemy;
			break;

		case CITY_FRIEND_1:
			result = this->m_pCityFriend1;
			break;
		case CITY_FRIEND_2:
			result = this->m_pCityFriend2;
			break;
		case CITY_FRIEND_3:
			result = this->m_pCityFriend3;
			break;

		case CITY_ENEMY_1:
			result = this->m_pCityEnemy1;
			break;
		case CITY_ENEMY_2:
			result = this->m_pCityEnemy2;
			break;
		case CITY_ENEMY_3:
			result = this->m_pCityEnemy3;
			break;

		case PLANE_SHRAPNEL_1:
			result = this->m_pPlaneShrapnel1;
			break;
		case CITY_SHRAPNEL_1:
			result = this->m_pCityShrapnel1;
			break;

		case SMOKE_BLACK:
			result = this->m_pSmokeBlack;
			break;

		case SKY_BOX:
			result = this->m_pSkyBox;
			break;

		default:
			assert(0);			// shouldnt get here
			break;
	}

	return result;
}

