#include "WorldManager.h"

CWorldManager * CWorldManager::pWorldManager = NULL;


void * CWorldManager::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init( Data );
			break;
		case MSGID_RENDER:
			this->Render();
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_GET_WORLD:
			result = this->m_pWorld;
			break;

		case MSGID_IS_FLATWORLD:
			result = &this->m_bFlatWorld;
			break;

		default:
			assert(0);
	}

	return result;
}

CWorldManager::CWorldManager()
{
}

CWorldManager::~CWorldManager()
{
}

void CWorldManager::Open()
{
	if ( pWorldManager == NULL )
		pWorldManager = new CWorldManager();
	else
		assert(0);
}

void CWorldManager::Close()
{
	assert(pWorldManager);

	delete pWorldManager;
	pWorldManager = NULL;
}

CTask * CWorldManager::Get()
{
	assert(pWorldManager);

	return pWorldManager;
}


void CWorldManager::Init( void * Data )
{
	//this->m_pWorld = (CTask*)Data;
	CWorldManager::WorldSetup data = *(CWorldManager::WorldSetup*)Data;
	this->m_pWorld = data.pWorld;
//	data.pPlanetBranch->AttachChild( ( NSNode *)this->m_pWorld );

	this->m_bFlatWorld = data.bIsFlatWorld;
}

void CWorldManager::Render()
{
	this->m_pWorld->PostMessage(MSGID_RENDER, NULL);
}

void CWorldManager::Destroy()
{
	// destroy our world
	this->m_pWorld->PostMessage(MSGID_DESTROY, NULL);

	// // dont delete the world since it is part of the scene
	// it isn't now
	delete this->m_pWorld;
	this->m_pWorld = NULL;
}
