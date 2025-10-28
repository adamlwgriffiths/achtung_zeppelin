#include "CityManager.h"

CCityManager * CCityManager::pCityManager = NULL;


void * CCityManager::PostMessage(int ID, void * Data)
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

		case MSGID_CHECK_COLLISIONSBOXVSBULLET:
			this->CheckCollisionVsBullet( (CTask*) Data );
			break;

		case MSGID_CHECK_COLLISIONSHIPVSCITY:
			this->CheckCollisionVsShip( (CTask*) Data );
			break;

		case MSGID_GET_NUMFRIENDLYCITIES:
			result = &this->m_iNumFriendlyCities;
			break;
		case MSGID_GET_FRIENDLYCITIESARRAY:
			result = this->m_pFriendlyCities;
			break;

		case MSGID_GET_NUMENEMYCITIES:
			result = &this->m_iNumEnemyCities;
			break;
		case MSGID_GET_ENEMYCITIESARRAY:
			result = this->m_pEnemyCities;
			break;



		default:
			assert(0);									// should never get here
	}

	return result;
}


CCityManager::CCityManager()
{
}

CCityManager::~CCityManager()
{
}


void CCityManager::Open()
{
	if ( pCityManager == NULL )
		pCityManager = new CCityManager();
	else
		assert(0);
}

void CCityManager::Close()
{
	if ( pCityManager != NULL )
	{
		delete pCityManager;
		pCityManager = NULL;
	}
	else
		assert(0);
}

CTask * CCityManager::Get()
{
	if ( pCityManager == NULL)
		assert(0);

	return pCityManager;
}




void CCityManager::Init(void * Data)
{
	this->m_pCityBranch = (NSNode*)Data;

	// Find out how many friendly cities we need
	this->m_iNumFriendlyCities = CUFOMain::m_iNumCities;
	this->m_iNumEnemyCities = CUFOMain::m_iNumCities;

	this->m_pFriendlyCities = new CCity * [ this->m_iNumFriendlyCities ];
	this->m_pEnemyCities = new CCity * [ this->m_iNumEnemyCities ];

	// get the width and length of the mesh
	CTask * world = (CTask*)CWorldManager::Get()->PostMessage(MSGID_GET_WORLD, NULL);

	int numCols = *(int*) world->PostMessage(MSGID_GET_NUMCOLS, NULL);
	int numRows = *(int*) world->PostMessage(MSGID_GET_NUMROWS, NULL);

	float mountainlevel = *(float*) world->PostMessage(MSGID_GET_MOUNTAINLEVEL, NULL);

	int tx, tz;
	CWorldFlat::WorldGetHeightAt getheightatData;

	CCity::CitySetupData cityData;

	bool valid = false;
	NSVector3df vec;

	bool isFlatWorld = *(bool*)CWorldManager::Get()->PostMessage(MSGID_IS_FLATWORLD, NULL);

	float radius = 0.0f;

	float veclength = 0.0f;

	if (!isFlatWorld)
		radius = *(float*)( (CTask*)CWorldManager::Get()->PostMessage(MSGID_GET_WORLD, NULL) )->PostMessage(MSGID_GET_RADIUS, NULL);

	// for each friendly city
	for ( int i = 0; i < this->m_iNumFriendlyCities; i++ )
	{
		while ( !valid )
		{
			// Get a random X, Y position
			tx = (int)( ( (float)rand() / (float)RAND_MAX ) * numCols);
			tz = (int)( ( (float)rand() / (float)RAND_MAX ) * numRows);

			// Flatten the area around the vertex

			// Get the height
			getheightatData.x = tx;
			getheightatData.y = tz;
			vec = *(NSVector3df*)world->PostMessage(MSGID_GET_HEIGHTAT, &getheightatData);

			// make sure they dont spawn on water or mountains
			if (isFlatWorld)
			{
				if (vec.v.y > 0.0f  &&  vec.v.y < mountainlevel)					// flat world
					valid = true;
			}
			else
			{
				veclength = vec.GetLength();

				// hack to see what the value is
				veclength = veclength;

				if (veclength > radius  &&  veclength < radius + mountainlevel)		// spherical world
					valid = true;
			}
		}

		valid = false;

		// Create a city
		cityData.cityBranch = this->m_pCityBranch;
		cityData.x = vec.v.x;
		cityData.y = vec.v.y;
		cityData.z = vec.v.z;
		cityData.alliance = FRIENDLY;
		this->m_pFriendlyCities[i] = new CCity();
		this->m_pFriendlyCities[i]->PostMessage(MSGID_INIT, &cityData);
		
		// set the cities array position
		this->m_pFriendlyCities[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
	}


	// for each enemy city
	for ( int i = 0; i < this->m_iNumEnemyCities; i++ )
	{
		while ( !valid )
		{
			// Get a random X, Y position
			tx = (int)( ( (float)rand() / (float)RAND_MAX ) * numCols);
			tz = (int)( ( (float)rand() / (float)RAND_MAX ) * numRows);

			// Flatten the area around the vertex

			// Get the height
			getheightatData.x = tx;
			getheightatData.y = tz;
			vec = *(NSVector3df*)world->PostMessage(MSGID_GET_HEIGHTAT, &getheightatData);

			// make sure they dont spawn on water or mountains
			if (isFlatWorld)
			{
				if (vec.v.y > 0.0f  &&  vec.v.y < mountainlevel)					// flat world
					valid = true;
			}
			else
			{
				veclength = vec.GetLength();
				if (veclength > radius  &&  veclength < radius + mountainlevel)		// spherical world
					valid = true;
			}
		}

		valid = false;

		// Create a city
		cityData.cityBranch = this->m_pCityBranch;
		cityData.x = vec.v.x;
		cityData.y = vec.v.y;
		cityData.z = vec.v.z;
		cityData.alliance = ENEMY;
		this->m_pEnemyCities[i] = new CCity();
		this->m_pEnemyCities[i]->PostMessage(MSGID_INIT, &cityData);

		// set the cities array position
		this->m_pEnemyCities[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
	}
}

void CCityManager::Destroy()
{
	for ( int i = 0; i < this->m_iNumFriendlyCities; i++ )
	{
		this->m_pFriendlyCities[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pFriendlyCities[i];
	}

	for ( int i = 0; i < this->m_iNumEnemyCities; i++ )
	{
		this->m_pEnemyCities[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pEnemyCities[i];
	}

	delete [] this->m_pFriendlyCities;
	delete [] this->m_pEnemyCities;

	this->m_pFriendlyCities = NULL;
	this->m_pEnemyCities = NULL;

	this->m_pCityBranch = NULL;
}


void CCityManager::Update(void * Data)
{
	bool finished;
	for ( int i = 0; i < this->m_iNumFriendlyCities; i++ )
	{
		finished = *(bool*)this->m_pFriendlyCities[i]->PostMessage(MSGID_UPDATE, Data);
		if (finished)
		{
			// notify the ships of a removed city
			this->NotifyShipsOfRemovedCity( i, FRIENDLY );

			// delete the node
			this->m_pFriendlyCities[i]->PostMessage(MSGID_DESTROY, NULL);
			delete this->m_pFriendlyCities[i];

			// shuffle the array
			this->m_iNumFriendlyCities--;
			this->m_pFriendlyCities[i] = this->m_pFriendlyCities[ this->m_iNumFriendlyCities ];
			this->m_pFriendlyCities[ this->m_iNumFriendlyCities ] = NULL;

			// update the moved city's array position
			if ( this->m_iNumFriendlyCities != i )
				this->m_pFriendlyCities[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}
	}

	for ( int i = 0; i < this->m_iNumEnemyCities; i++ )
	{
		finished = *(bool*)this->m_pEnemyCities[i]->PostMessage(MSGID_UPDATE, Data);
		if (finished)
		{
			// notify the ships of a removed city
			this->NotifyShipsOfRemovedCity( i, ENEMY );

			// delete the node
			this->m_pEnemyCities[i]->PostMessage(MSGID_DESTROY, NULL);
			delete this->m_pEnemyCities[i];

			// shuffle the array
			this->m_iNumEnemyCities--;
			this->m_pEnemyCities[i] = this->m_pEnemyCities[ this->m_iNumEnemyCities ];
			this->m_pEnemyCities[ this->m_iNumEnemyCities ] = NULL;

			// update the moved city's array position
			if ( this->m_iNumEnemyCities != i )
				this->m_pEnemyCities[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}
	}


	// update our hud
	CHUD::CityData citydata;
	citydata.numenemycities = this->m_iNumEnemyCities;
	citydata.numfriendlycities = this->m_iNumFriendlyCities;

	CHUD::Get()->PostMessage( MSGID_UPDATE_CITYSTATS, &citydata );


		// check our game state
	if ( this->m_iNumFriendlyCities == 0 )
	{
		// game over for friendlies!
		CHUD::Get()->PostMessage(MSGID_GAMEOVER_FRIENDLYNOCITIES, NULL);

	}
	else if ( this->m_iNumEnemyCities == 0 )
	{
		// game over for enemies!
		CHUD::Get()->PostMessage(MSGID_GAMEOVER_ENEMYNOCITIES, NULL);
	}
}



void CCityManager::NotifyShipsOfRemovedCity( int arraypos, ALLIANCE alliance )
{
	int num;
	CTask ** ships;

	// ships dont currently target cities

	//if ( alliance == ENEMY )
	//{
	//	// notify the friendly ships
	//	num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYSHIPS, NULL );
	//	ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYSHIPSARRAY, NULL );
	//}
	//else
	//{
	//	// notify the friendly ships
	//	num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYSHIPS, NULL );
	//	ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYSHIPSARRAY, NULL );
	//}

	//for ( int i = 0; i < num; i++ )
	//{
	//	// notify each ship of a deleted city
	//	ships[i]->PostMessage( MSGID_NOTIFY_CITYDEATH, &arraypos );
	//}

	// notify our zeppelins
	if ( alliance == ENEMY )
	{
		// notify the friendly ships
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYZEPPELINS, NULL );
		ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYZEPPELINSARRAY, NULL );
	}
	else
	{
		// notify the friendly ships
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYZEPPELINS, NULL );
		ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYZEPPELINSARRAY, NULL );
	}

	for ( int i = 0; i < num; i++ )
	{
		// notify each ship of a deleted city
		ships[i]->PostMessage( MSGID_NOTIFY_CITYDEATH, &arraypos );
	}


}








void CCityManager::CheckCollisionBullet( CTask * c, CTask * bullet )
{
	// Dont continue if friendly fire is disabled and were on the same team
	if ( ! CUFOMain::m_bFriendlyFire )
	{
		ALLIANCE balliance = *(ALLIANCE*)bullet->PostMessage(MSGID_GET_ALLIANCE, NULL);
		ALLIANCE calliance = *(ALLIANCE*)c->PostMessage(MSGID_GET_ALLIANCE, NULL);

		if ( balliance == calliance )
			return;
	}

	// do collision detection
	float cr, br;
	cr = *(float*) c->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	br = *(float*) bullet->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df cv, bv;
	cv = *(  ( (NSNode*) c->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation()  );
	bv = *(  ( (NSNode*) bullet->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation()  );

	NSSphere csp = NSSphere( cv, cr );
	NSSphere bsp = NSSphere( bv, br );

	float dist =  IntersectSphereSphere( csp, bsp );

	// if dist is <= 0.0f, we have an intersection
	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("City vs Bullet Collision\n") );
		
		int damage = *(int*)bullet->PostMessage(MSGID_GET_DAMAGEAMOUNT, NULL);
		c->PostMessage(MSGID_APPLY_DAMAGE, &damage );

		// spawn some shrapnel
// FIXME

		// kill the bullet
		bullet->PostMessage( MSGID_TRACER_HIT, NULL );
	}
}

void CCityManager::CheckCollisionVsBullet( CTask * bullet )
{
	// check friendly cities vs bullet
	for ( int i = 0; i < this->m_iNumFriendlyCities; i++ )
	{
		this->CheckCollisionBullet( this->m_pFriendlyCities[i], bullet );
	}

	// check enemy cities vs bullet
	for ( int i = 0; i < this->m_iNumEnemyCities; i++ )
	{
		this->CheckCollisionBullet( this->m_pEnemyCities[i], bullet );
	}
}




void CCityManager::CheckCollisionShip( CTask * c, CTask * s )	// called by VsShip
{
	float sr, cr;
	sr = *(float*) s->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	cr = *(float*) c->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df sv, cv;
	sv = *( (NSNode*) s->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	cv = *( (NSNode*) c->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	NSSphere ssp = NSSphere( sv, sr );
	NSSphere csp = NSSphere( cv, cr );

	float dist =  IntersectSphereSphere( ssp, csp );

	// if dist is <= 0.0f, we have an intersection
	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("City vs Ship Collision\n") );
		
		// get the two ships velocity vectors
		NSVector3df * svel;
		svel = (NSVector3df*)s->PostMessage(MSGID_GET_VELOCITY, NULL);

		float sm, cm;
		sm = *(float*)s->PostMessage(MSGID_GET_MASS, NULL);
		cm = *(float*)c->PostMessage(MSGID_GET_MASS, NULL);

		// get the difference between the vectors
		NSVector3df diff = cv - sv;

		// apply a scale and apply the damage based on mass of the opposing ship
		float diffLength = svel->Dot( diff );
		if ( diffLength < 0.0f )
			return;

		int sd = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * cm  );
		int cd = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * sm  );
		s->PostMessage(MSGID_APPLY_DAMAGE, &sd );
		c->PostMessage(MSGID_APPLY_DAMAGE, &cd );

		// reflect the direction
		ReflectVector( diff, *svel );


		// Lose some velocity
		float newvelocity;
		newvelocity = svel->GetLength();
		newvelocity -= (newvelocity * SHIP_COLLISION_RETURNSPEEDSCALE);
		svel->SetLength( newvelocity );
	}
}

void CCityManager::CheckCollisionVsShip( CTask * ship )
{
	// check friendly cities vs bullet
	for ( int i = 0; i < this->m_iNumFriendlyCities; i++ )
	{
		this->CheckCollisionShip( this->m_pFriendlyCities[i], ship );
	}

	// check enemy cities vs bullet
	for ( int i = 0; i < this->m_iNumEnemyCities; i++ )
	{
		this->CheckCollisionShip( this->m_pEnemyCities[i], ship );
	}
}




