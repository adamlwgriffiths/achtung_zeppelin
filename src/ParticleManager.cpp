#include "ParticleManager.h"

CParticleManager * CParticleManager::pParticleManager;	

const int	CParticleManager::s_iMaxNumPSystems			= 50;


void * CParticleManager::PostMessage(int ID, void * Data)
{
	void * result = 0;

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

		case MSGID_CREATE_PSYSTEM:
			result = this->CreatePSystem( (CParticleManager::NewSystemData*) Data );
			break;

		case MSGID_DELETE_PSYSTEM:
			this->DeletePSystem( (PSystem*)Data );
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CParticleManager::CParticleManager()
{}

CParticleManager::~CParticleManager()
{}


/////////////////////////////////////////////
// Opens an instance of the singleton.<P>
// If its already open, throw an assertion so we can track down
// any bad logic in our program.
void CParticleManager::Open()
{
	assert(!pParticleManager);			// shouldnt get here either!

	pParticleManager = new CParticleManager();
}

/////////////////////////////////////////////
// Kill our singleton
void CParticleManager::Close()
{
	assert(pParticleManager);

	delete pParticleManager;
	pParticleManager = NULL;
}

/////////////////////////////////////////////
// Get the singleton instance of the class
CTask * CParticleManager::Get()
{
	if (!pParticleManager)
		CParticleManager::Open();

	return (CTask*)pParticleManager;
}




void CParticleManager::Init( void * Data )
{
	this->m_iNumPSystems = 0;

	this->m_pPSystemsBranch = (NSNode*)Data;
	this->m_pPSystems = new PSystem * [ this->s_iMaxNumPSystems ];
}

void CParticleManager::Update( void * Data )
{
	float fTimeDelta = *(float*)Data;

	// kill any dead particle systems
	for ( int i = 0; i < this->m_iNumPSystems; i++ )
	{
		if ( this->m_pPSystems[i]->IsFinished() == true )
		{
			delete this->m_pPSystems[i];

			// shuffle the array
			this->m_iNumPSystems--;
			this->m_pPSystems[i] = this->m_pPSystems[ this->m_iNumPSystems ];
			this->m_pPSystems[ this->m_iNumPSystems ] = NULL;
		}
	}

	// update any alive nodes
	for ( int i = 0; i < this->m_iNumPSystems; i++ )
	{
		this->m_pPSystems[i]->Update( fTimeDelta );
	}
}

void CParticleManager::Destroy()
{
	for ( int i = 0; i < this->m_iNumPSystems; i++ )
	{
		delete this->m_pPSystems[i];
	}
	delete [] this->m_pPSystems;
}

PSystem * CParticleManager::CreatePSystem( CParticleManager::NewSystemData * data )
{
	PSystem * system;

	// dont create too many systems
	if ( this->m_iNumPSystems >= CParticleManager::s_iMaxNumPSystems )
		return NULL;

	switch ( data->type )
	{
		case CITY_EXPLOSION:
			system = new CityExplosionPSystem( this->m_pPSystemsBranch, data->trans, data->radius, data->velocity );
			break;
		case PLANE_SHRAPNEL:
			system = new PlaneShrapnel( this->m_pPSystemsBranch, data->trans );
			break;
		case PLANE_SMOKE:
			system = new PlaneSmoke( this->m_pPSystemsBranch, data->parent );
			break;

		default:
			assert(0);
	}

	// add the system
	this->m_pPSystems[ this->m_iNumPSystems ] = system;
	this->m_iNumPSystems++;

	return system;
}

void CParticleManager::DeletePSystem( PSystem * system )
{
	// find our system
	for ( int i = 0; i < this->m_iNumPSystems; i++ )
	{
		// if the address is the same
		if ( this->m_pPSystems[i] == system )
		{
			// blat it
			delete this->m_pPSystems[i];

			// shuffle the array
			this->m_iNumPSystems--;
			this->m_pPSystems[i] = this->m_pPSystems[ this->m_iNumPSystems ];
			this->m_pPSystems[ this->m_iNumPSystems ] = NULL;
		}
	}
}

