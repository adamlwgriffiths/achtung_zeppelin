#include "ShipController.h"

CShipController * CShipController::pShipController;		// singleton pointer


const float			CShipController::s_fFriendlySpawnTime	= 15.0f;//15.0f;
const float			CShipController::s_fEnemySpawnTime		= 15.0f;//15.0f;

const float			CShipController::s_fShipVerticalSpawnOffset = 10.5f;
const float			CShipController::s_fShipHorizontalSpawnOffset = 6.0f;

const float			CShipController::s_fZeppelinHeight = 30.0f;

const float			CShipController::s_fBulletAllowanceRadius = 0.5f;

void * CShipController::PostMessage(int ID, void * Data)
{
	void * result = 0;

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

		case MSGID_CREATE_SHIP:
			result = this->CreateShip(Data);
			break;

		case MSGID_CHECK_COLLISIONSSPHEREVSSHIP:
			this->CheckCollisionsShipVsShip();
			break;

		case MSGID_CHECK_COLLISIONSSPHEREVSGROUND:
			this->CheckCollisionsShipVsGround();
			break;

		case MSGID_CHECK_COLLISIONSSPHEREVSBULLET:
			this->CheckCollisionShipVsBullet( (CTask*) Data );
			break;

		case MSGID_CHECK_COLLISIONSHIPVSCITY:
			this->CheckCollisionsShipVsCity( (CTask*) Data );
			break;

		case MSGID_REQUEST_DELETION:
			this->DestroyShip( (CTask*)Data );
			break;

		case MSGID_NOTIFY_OF_DEATH:
			this->InvalidateShip( (CTask*)Data );
			break;

		case MSGID_NOTIFY_OF_ZEPPELINDEATH:
			this->InvalidateZeppelin( (CTask*)Data );
			break;


		case MSGID_GET_NUMFRIENDLYSHIPS:
			result = &this->m_iNumFriendlyShips;
			break;
		case MSGID_GET_FRIENDLYSHIPSARRAY:
			result = this->m_pFriendlyShips;
			break;

		case MSGID_GET_NUMENEMYSHIPS:
			result = &this->m_iNumEnemyShips;
			break;
		case MSGID_GET_ENEMYSHIPSARRAY:
			result = this->m_pEnemyShips;
			break;

		case MSGID_GET_PLAYERSHIP:
			result = this->m_pPlayerShip;
			break;

		case MSGID_GET_NUMFRIENDLYZEPPELINS:
			result = &this->m_iNumFriendlyZeppelins;
			break;
		case MSGID_GET_FRIENDLYZEPPELINSARRAY:
			result = this->m_pFriendlyZeppelins;
			break;

		case MSGID_GET_NUMENEMYZEPPELINS:
			result = &this->m_iNumEnemyZeppelins;
			break;
		case MSGID_GET_ENEMYZEPPELINSARRAY:
			result = this->m_pEnemyZeppelins;
			break;

		case MSGID_SPAWN_SHIPS:
			this->SpawnShips();
			break;

		case MSGID_SPAWN_ZEPPELINS:
			this->SpawnZeppelins();
			break;


		case MSGID_CREATE_ZEPPELIN:
			this->CreateZeppelin( Data );
			break;


		case MSGID_CHECK_COLLISIONSSPHEREVSZEPPELIN:
			this->CheckCollisionsVsZeppelin();
			break;

		case MSGID_CHECK_COLLISIONSZEPPELINVSBULLET:
			this->CheckCollisionZeppelinVsBullet( (CTask*)Data );
			break;


		case MSGID_CYCLE_CAMERA:
			this->CameraCycleNext();
			break;

		default:
			assert(0);		// should never get here!
			break;
	}

	return result;
}

CShipController::CShipController()
{

}

CShipController::~CShipController()
{

}



/////////////////////////////////////////////
// Opens an instance of the singleton.<P>
// If its already open, throw an assertion so we can track down
// any bad logic in our program.
void CShipController::Open()
{
	assert(!pShipController);			// shouldnt get here either!

	pShipController = new CShipController();
}

/////////////////////////////////////////////
// Kill our singleton
void CShipController::Close()
{
	assert(pShipController);

	delete pShipController;
	pShipController = NULL;
}

/////////////////////////////////////////////
// Get the singleton instance of the class
CTask * CShipController::Get()
{
	if (!pShipController)
		CShipController::Open();

	return (CTask*)pShipController;
}



void CShipController::Init(void * Data)
{
	// cast and save the ship branch
	//m_pShipBranch = (NSNode*)Data;
	CShipController::ShipControllerInitData data = *(CShipController::ShipControllerInitData*)Data;

	// ship branches
	this->m_pShipBranch = data.shipBranch;
	this->m_pBulletBranch = data.bulletBranch;

	// zeppelin branches
	this->m_pZeppelinBranch = data.zeppelinBranch;
	this->m_pZeppelinBulletBranch = data.zeppelinBulletBranch;


	this->m_bPlayer = false;
	this->m_pPlayerShip = NULL;


	// Ships
	for ( int i = 0; i < SHIPS_MAX_NUM; i++)
	{
		this->m_pFriendlyShips[i] = NULL;
		this->m_pEnemyShips[i] = NULL;
		this->m_pShipsPendingDeletion[i] = NULL;
	}

	this->m_iNumFriendlyShips = 0;
	this->m_iNumEnemyShips = 0;
	this->m_iNumShipsPendingDeletion = 0;

	// Zeppelins
	for ( int i = 0; i < ZEPPELINS_MAX_NUM; i++)
	{
		this->m_pFriendlyZeppelins[i] = NULL;
		this->m_pEnemyZeppelins[i] = NULL;
		this->m_pZeppelinsPendingDeletion[i] = NULL;
	}

	this->m_iNumFriendlyZeppelins = 0;
	this->m_iNumEnemyZeppelins = 0;
	this->m_iNumZeppelinsPendingDeletion = 0;


	// Camera
	this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
	this->m_iCameraTargetPos = 0;


	// start off spawning a ship
	this->m_fFriendlySpawnTimer = this->s_fFriendlySpawnTime;
	this->m_fEnemySpawnTimer = this->s_fEnemySpawnTime;
}

void CShipController::Destroy()
{
	// kill the player ship
	if ( this->m_pPlayerShip != NULL )
	{
		this->m_pPlayerShip->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pPlayerShip;
		this->m_pPlayerShip = false;
	}

	// kill our ships
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
	{
		this->m_pFriendlyShips[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pFriendlyShips[i];
		this->m_pFriendlyShips[i] = NULL;
	}

	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
	{
		this->m_pEnemyShips[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pEnemyShips[i];
		this->m_pEnemyShips[i] = NULL;
	}

	for ( int i = 0; i < this->m_iNumShipsPendingDeletion; i++ )
	{
		this->m_pShipsPendingDeletion[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pShipsPendingDeletion[i];
		this->m_pShipsPendingDeletion[i] = NULL;
	}

	// kill our zeppelins
	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
	{
		this->m_pFriendlyZeppelins[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pFriendlyZeppelins[i];
		this->m_pFriendlyZeppelins[i] = NULL;
	}

	for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
	{
		this->m_pEnemyZeppelins[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pEnemyZeppelins[i];
		this->m_pEnemyZeppelins[i] = NULL;
	}

	for ( int i = 0; i < this->m_iNumZeppelinsPendingDeletion; i++ )
	{
		this->m_pZeppelinsPendingDeletion[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pShipsPendingDeletion[i];
		this->m_pShipsPendingDeletion[i] = NULL;
	}
}



void CShipController::CameraCycleNext()
{
	//NSNode * targetnode;
	CTask * target;

	/////////////////////
	// Player
	if ( this->m_eCameraTarget == CShipController::CAMTARG_PLAYER )
	{
		// if there are any friendly planes
		if ( this->m_iNumFriendlyShips > 0 )
		{
			this->m_eCameraTarget = CShipController::CAMTARG_FRIENDLY;
			this->m_iCameraTargetPos = 0;
			//targetnode = (NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pFriendlyShips[ this->m_iCameraTargetPos ];
		}
		else if ( this->m_iNumEnemyShips > 0 )
		{
			// pick an enemy ship
			this->m_eCameraTarget = CShipController::CAMTARG_ENEMY;
			this->m_iCameraTargetPos = 0;
			//targetnode = (NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pEnemyShips[ this->m_iCameraTargetPos ];
		}
		else if ( this->m_iNumFriendlyZeppelins > 0 )
		{
			// pick a friendly zeppelin
			this->m_eCameraTarget = CShipController::CAMTARG_ZEPFRIENDLY;
			this->m_iCameraTargetPos = 0;
			//targetnode = (NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ];
		}
		else if ( this->m_iNumEnemyZeppelins > 0 )
		{
			// pick an enemy zeppelin
			this->m_eCameraTarget = CShipController::CAMTARG_ZEPENEMY;
			this->m_iCameraTargetPos = 0;
			//targetnode = (NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ];
		}
		else
		{
			target = NULL;
//			return;
		}
	}
	/////////////////////
	// Friendlies
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_FRIENDLY )
	{
		// if were not past the array bounds
		if ( this->m_iCameraTargetPos < this->m_iNumFriendlyShips - 1 )
		{
			this->m_iCameraTargetPos++;
			//targetnode = (NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pFriendlyShips[ this->m_iCameraTargetPos ];
		}
		else
		{
			// if there are any enemies
			if ( this->m_iNumEnemyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_ENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyShips[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumFriendlyZeppelins > 0 )
			{
				// pick a friendly zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPFRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumEnemyZeppelins > 0 )
			{
				// pick an enemy zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_pPlayerShip != NULL )
			{
				// if the player is alive, pick it
				this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pPlayerShip->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pPlayerShip;
			}
			else
			{
				target = NULL;
//				return;
			}
		}
	}
	/////////////////////
	// Enemy
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ENEMY )
	{
		// if were not past the array bounds
		if ( this->m_iCameraTargetPos < this->m_iNumEnemyShips - 1 )
		{
			this->m_iCameraTargetPos++;
			//targetnode = (NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pEnemyShips[ this->m_iCameraTargetPos ];
		}
		else
		{
			// if there is a player
			// if the player is alive, pick it
			if ( this->m_iNumFriendlyZeppelins > 0 )
			{
				// pick a friendly zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPFRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumEnemyZeppelins > 0 )
			{
				// pick an enemy zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_pPlayerShip != NULL )
			{
				this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pPlayerShip->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pPlayerShip;
			}
			else if ( this->m_iNumFriendlyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_FRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyShips[ this->m_iCameraTargetPos ];
			}
			else
			{
				target = NULL;
//				return;
			}
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ZEPFRIENDLY )
	{
		// if were not past the array bounds
		if ( this->m_iCameraTargetPos < this->m_iNumFriendlyZeppelins - 1 )
		{
			this->m_iCameraTargetPos++;
			//targetnode = (NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ];
		}
		else
		{
			if ( this->m_iNumEnemyZeppelins > 0 )
			{
				// pick an enemy zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_pPlayerShip != NULL )
			{
				this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pPlayerShip->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pPlayerShip;
			}
			else if ( this->m_iNumFriendlyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_FRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyShips[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumEnemyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_ENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyShips[ this->m_iCameraTargetPos ];
			}
			else
			{
				target = NULL;
//				return;
			}
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ZEPENEMY )
	{
		// if were not past the array bounds
		if ( this->m_iCameraTargetPos < this->m_iNumEnemyZeppelins - 1 )
		{
			this->m_iCameraTargetPos++;
			//targetnode = (NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
			target = this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ];
		}
		else
		{
			if ( this->m_pPlayerShip != NULL )
			{
				this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pPlayerShip->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pPlayerShip;
			}
			else if ( this->m_iNumFriendlyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_FRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyShips[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumEnemyShips > 0 )
			{
				// pick an enemy ship
				this->m_eCameraTarget = CShipController::CAMTARG_ENEMY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pEnemyShips[ this->m_iCameraTargetPos ];
			}
			else if ( this->m_iNumFriendlyZeppelins > 0 )
			{
				// pick a friendly zeppelin
				this->m_eCameraTarget = CShipController::CAMTARG_ZEPFRIENDLY;
				this->m_iCameraTargetPos = 0;
				//targetnode = (NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage( MSGID_GET_NODE, NULL );
				target = this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ];
			}
			else
			{
				target = NULL;
//				return;
			}
		}
	}
	else
	{
		target = NULL;
//		return;
	}

	CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);
	camera->PostMessage(MSGID_SET_TARGET, target );//targetnode);

	float delta = 0.0f;
	camera->PostMessage( MSGID_UPDATE, &delta );
}

void CShipController::CameraCyclePrevious()
{

}


void CShipController::CheckCameraCycle()
{
	CInputController::Keys keys = *(CInputController::Keys*)CInputController::Get()->PostMessage(MSGID_GET_KEYS, NULL);

	if ( keys.m_bCamera_CycleNext )
	{
		this->CameraCycleNext();

	}
	else if ( keys.m_bCamera_CyclePrevious )
	{

	}
	else
		return;

	//CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);
	//camera->PostMessage(MSGID_SET_TARGET, targetnode);
}


void CShipController::Update(void * Data)
{
	///////////////////////////////////
	// Update our ships

	// check if we need to switch cameras
	this->CheckCameraCycle();


	///////////////
	// Ships

	// update the players ship
//	if (this->m_bPlayer)
	if ( this->m_pPlayerShip != NULL )
		m_pPlayerShip->PostMessage(MSGID_UPDATE, Data);

	// update any friendly ships
	for (int i = 0; i < this->m_iNumFriendlyShips; i++)
		m_pFriendlyShips[i]->PostMessage(MSGID_UPDATE, Data);

	// update any enemy ships
	for (int i = 0; i < this->m_iNumEnemyShips; i++)
		m_pEnemyShips[i]->PostMessage(MSGID_UPDATE, Data);


	///////////////
	// Zeppelins

	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
		this->m_pFriendlyZeppelins[i]->PostMessage(MSGID_UPDATE, Data);

	for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
		this->m_pEnemyZeppelins[i]->PostMessage(MSGID_UPDATE, Data);



	////////////////////////////////
	// Destroy any pending ships

	// check our pending deletion array
	for ( int i = 0; i < this->m_iNumShipsPendingDeletion; )
	{
		// i is always 0
		this->m_pShipsPendingDeletion[i]->PostMessage( MSGID_DESTROY, NULL );
		delete this->m_pShipsPendingDeletion[i];

		// shuffle down the array
		this->m_iNumShipsPendingDeletion--;
		this->m_pShipsPendingDeletion[i] = this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ];
		this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ] = NULL;
	}


	// zeppelins
	// check our pending deletion array
	for ( int i = 0; i < this->m_iNumZeppelinsPendingDeletion; )
	{
		// i is always 0
		delete this->m_pZeppelinsPendingDeletion[i];

		// shuffle down the array
		this->m_iNumZeppelinsPendingDeletion--;
		this->m_pZeppelinsPendingDeletion[i] = this->m_pZeppelinsPendingDeletion[ this->m_iNumZeppelinsPendingDeletion ];
		this->m_pZeppelinsPendingDeletion[ this->m_iNumZeppelinsPendingDeletion ] = NULL;
	}



	///////////////////////////
	// update the HUD
	int iNumFriendlyZeppelins = 0;
	int iNumEnemyZeppelins = 0;

	CHUD::ShipData huddata;
	huddata.numenemyships = this->m_iNumEnemyShips;
	huddata.numfriendlyships = this->m_iNumFriendlyShips;
	huddata.numenemyzeppelins = 0;
	huddata.numfriendlyzeppelins = 0;
	for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
	{
		if ( *(bool*)this->m_pEnemyZeppelins[i]->PostMessage(MSGID_IS_ALIVE, NULL) )
			iNumEnemyZeppelins++;
	}
	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
	{
		if ( *(bool*)this->m_pFriendlyZeppelins[i]->PostMessage(MSGID_IS_ALIVE, NULL) )
			iNumFriendlyZeppelins++;
	}

	huddata.numenemyzeppelins = iNumEnemyZeppelins;
	huddata.numfriendlyzeppelins = iNumFriendlyZeppelins;

	// account for the player
	if ( this->m_pPlayerShip != NULL )
		huddata.numfriendlyships++;

	huddata.respawnTime = s_fFriendlySpawnTime - this->m_fFriendlySpawnTimer;

	CHUD::Get()->PostMessage( MSGID_UPDATE_SHIPSTATS, &huddata );



	///////////////////////////
	// update our sound controller
	bool shooting = false;
	CSoundController::SoundData sounddata;
	sounddata.sound = CSoundController::TRACER;
	sounddata.distance = 0.0f;
	sounddata.frequency = -1;
	sounddata.volume = 2;


	CSoundController::ShipData enginesounddata;
	float throttle = 0.0f;
	SHIP_TYPE type;

	CHUD::PlayerData playerdata;

	if ( this->m_eCameraTarget == CShipController::CAMTARG_PLAYER )
	{
		type = PLANE;

		if ( this->m_pPlayerShip != NULL )
		{
			throttle = *(float*)this->m_pPlayerShip->PostMessage(MSGID_GET_THROTTLE, NULL);

			// work out if its shooting
			if ( *(bool*)this->m_pPlayerShip->PostMessage(MSGID_IS_FIRING, NULL) )
			{
				sounddata.sound = CSoundController::TRACER;
				shooting = true;
			}

			// update our hud
			playerdata.health = *(int*)this->m_pPlayerShip->PostMessage(MSGID_GET_HEALTH, NULL);

			playerdata.throttle = throttle;

			NSVector3df trans = *((NSNode*)this->m_pPlayerShip->PostMessage(MSGID_GET_NODE, NULL))->GetWorldTranslation();
			playerdata.posx = trans.v.x;
			playerdata.posy = trans.v.y;
			playerdata.posz = trans.v.z;

			playerdata.speed = ((NSVector3df*)this->m_pPlayerShip->PostMessage(MSGID_GET_VELOCITY, NULL) )->GetLength();

			CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &playerdata );
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_FRIENDLY )
	{
		type = PLANE;

		if ( this->m_pFriendlyShips[ this->m_iCameraTargetPos ] != NULL )
		{
			throttle = *(float*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_THROTTLE, NULL);


			// work out if its shooting
			if ( *(bool*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_IS_FIRING, NULL) )
			{
				sounddata.sound = CSoundController::TRACER;
				shooting = true;
			}

			// update our hud
			playerdata.health = *(int*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_HEALTH, NULL);

			playerdata.throttle = throttle;

			NSVector3df trans = *((NSNode*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_NODE, NULL))->GetWorldTranslation();
			playerdata.posx = trans.v.x;
			playerdata.posy = trans.v.y;
			playerdata.posz = trans.v.z;

			playerdata.speed = ((NSVector3df*)this->m_pFriendlyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_VELOCITY, NULL) )->GetLength();

			CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &playerdata );
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ENEMY )
	{
		type = PLANE;

		if ( this->m_pEnemyShips[ this->m_iCameraTargetPos ] != NULL )
		{
			throttle = *(float*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_THROTTLE, NULL);
	

			// work out if its shooting
			if ( *(bool*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_IS_FIRING, NULL) )
			{
				sounddata.sound = CSoundController::TRACER;
				shooting = true;
			}

			// update our hud
			playerdata.health = *(int*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_HEALTH, NULL);

			playerdata.throttle = throttle;

			NSVector3df trans = *((NSNode*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_NODE, NULL))->GetWorldTranslation();
			playerdata.posx = trans.v.x;
			playerdata.posy = trans.v.y;
			playerdata.posz = trans.v.z;

			playerdata.speed = ((NSVector3df*)this->m_pEnemyShips[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_VELOCITY, NULL) )->GetLength();

			CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &playerdata );
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ZEPFRIENDLY )
	{
		type = ZEPPELIN;

		if ( this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ] != NULL )
		{
			throttle = *(float*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_THROTTLE, NULL);

			// work out if its shooting
			if ( *(bool*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_IS_FIRING, NULL) )
			{
				sounddata.sound = CSoundController::ZEPPELIN_GUN;
				shooting = true;
			}

			// update our hud
			playerdata.health = *(int*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_HEALTH, NULL);

			playerdata.throttle = throttle;

			NSVector3df trans = *((NSNode*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_NODE, NULL))->GetWorldTranslation();
			playerdata.posx = trans.v.x;
			playerdata.posy = trans.v.y;
			playerdata.posz = trans.v.z;

			playerdata.speed = ((NSVector3df*)this->m_pFriendlyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_VELOCITY, NULL) )->GetLength();

			CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &playerdata );
		}
	}
	else if ( this->m_eCameraTarget == CShipController::CAMTARG_ZEPENEMY )
	{
		type = ZEPPELIN;

		if ( this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ] != NULL )
		{
			throttle = *(float*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_THROTTLE, NULL);

			// work out if its shooting
			if ( *(bool*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_IS_FIRING, NULL) )
			{
				sounddata.sound = CSoundController::ZEPPELIN_GUN;
				shooting = true;
			}

			// update our hud
			playerdata.health = *(int*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_HEALTH, NULL);

			playerdata.throttle = throttle;

			NSVector3df trans = *((NSNode*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_NODE, NULL))->GetWorldTranslation();
			playerdata.posx = trans.v.x;
			playerdata.posy = trans.v.y;
			playerdata.posz = trans.v.z;

			playerdata.speed = ((NSVector3df*)this->m_pEnemyZeppelins[ this->m_iCameraTargetPos ]->PostMessage(MSGID_GET_VELOCITY, NULL) )->GetLength();

			CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &playerdata );
		}
	}


	// get the target ship
	// update our engine noise
	enginesounddata.throttle = throttle;
	enginesounddata.type = type;

	CSoundController::Get()->PostMessage( MSGID_UPDATE_PLAYERENGINE, &enginesounddata );

//	if ( shooting )
//		CSoundController::Get()->PostMessage( MSGID_PLAY_SOUND, &sounddata );



	// count the number of alive ships
	int iNumFriendlyShips = 0;
	int iNumEnemyShips = 0;

	if ( this->m_pPlayerShip != NULL )
	{
		if ( *(bool*)this->m_pPlayerShip->PostMessage(MSGID_IS_ALIVE, NULL ) )
			iNumFriendlyShips++;
	}

	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
	{
		if ( *(bool*)this->m_pFriendlyShips[i]->PostMessage(MSGID_IS_ALIVE, NULL ) )
			iNumFriendlyShips++;
	}
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
	{
		if ( *(bool*)this->m_pEnemyShips[i]->PostMessage(MSGID_IS_ALIVE, NULL ) )
			iNumEnemyShips++;
	}



	// check our game state
	if ( iNumFriendlyZeppelins == 0  /*&&  iNumFriendlyShips == 0*/ )
	{
		// game over for friendlies!
		CHUD::Get()->PostMessage(MSGID_GAMEOVER_FRIENDLYNOSHIPS, NULL);

		// hide the respawn timer
		CHUD::Get()->PostMessage( MSGID_HIDE_SPAWNTIMER, NULL );
	}
	else if ( iNumEnemyZeppelins == 0  /*&&  iNumEnemyShips == 0*/ )
	{
		// game over for enemies!
		CHUD::Get()->PostMessage(MSGID_GAMEOVER_ENEMYNOSHIPS, NULL);
	}






	//////////////////////////////
	// lastly, spawn any new ships for this turn
	this->SpawnShips();

	this->m_fEnemySpawnTimer += *(float*)Data;
	this->m_fFriendlySpawnTimer += *(float*)Data;
}



void CShipController::SpawnShips()
{
	CShip::InitData data;

	// friendly ships
	int numFriendlyShips = SHIPS_MAX_NUM;
	if ( this->m_pPlayerShip )
		numFriendlyShips--;

	if ( this->m_iNumFriendlyZeppelins > 0 )
	{

		if ( this->m_fFriendlySpawnTimer >= this->s_fFriendlySpawnTime )
		{
			// spawn a ship at each zeppelin
			for ( int zeppnum = 0; zeppnum < this->m_iNumFriendlyZeppelins; zeppnum++ )
			{


				// if we dont have a player, spawn one immediately
				if ( this->m_pPlayerShip == NULL  &&  !CUFOMain::m_bDemoMode )
				{
					data.isPlayer = true;

					// find a random zeppelin carrier
//					int zeppnum = (int)(   ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iNumFriendlyZeppelins  );

					// make sure the zeppelin is alive
					if ( *(bool*)this->m_pFriendlyZeppelins[ zeppnum ]->PostMessage(MSGID_IS_ALIVE, NULL) )
					{
						NSNode * zeppnode = (NSNode*)this->m_pFriendlyZeppelins[ zeppnum ]->PostMessage(MSGID_GET_NODE, NULL);
						NSVector3df zepptrans = *zeppnode->GetWorldTranslation();
						NSVector3df zeppfacing = zeppnode->GetFacingVector();
						zeppfacing.SetLength( s_fShipHorizontalSpawnOffset );
						zepptrans -= zeppfacing;
						NSMatrix4 zeppmat = *zeppnode->GetWorldMatrix();

						zepptrans.v.y += s_fShipVerticalSpawnOffset;

						data.alliance = FRIENDLY;
						data.mat = zeppmat;
						data.shipcontroller = this;
						data.trans = zepptrans;
						data.up = NSVector3df ( 0.0f, 0.0f, 0.0f );

						this->CreateShip( &data );

						// change the camera to this ship
						NSNode * targetnode;
						this->m_eCameraTarget = CShipController::CAMTARG_PLAYER;
						this->m_iCameraTargetPos = 0;
						targetnode = (NSNode*)this->m_pPlayerShip->PostMessage( MSGID_GET_NODE, NULL );

						CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);
						//camera->PostMessage(MSGID_SET_TARGET, targetnode);
						camera->PostMessage(MSGID_SET_TARGET, this->m_pPlayerShip );

						float delta = 0.0f;
						camera->PostMessage( MSGID_UPDATE, &delta );

						this->m_fFriendlySpawnTimer = 0.0f;
					}
				}

//		if ( this->m_fFriendlySpawnTimer >= this->s_fFriendlySpawnTime )
//		{
				else if ( this->m_iNumFriendlyShips < CUFOMain::m_iMaxNumShips
					&&  this->m_iNumFriendlyShips < numFriendlyShips )//SHIPS_MAX_NUM) // account for player
				{
					// create a new ship
					data.isPlayer = false;

					// find a random zeppelin carrier
//					int zeppnum = (int)(   ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iNumFriendlyZeppelins  );

					if ( this->m_pFriendlyZeppelins[ zeppnum ] != NULL )
					{
						if ( *(bool*)this->m_pFriendlyZeppelins[ zeppnum ]->PostMessage(MSGID_IS_ALIVE, NULL) )
						{
							NSNode * zeppnode = (NSNode*)this->m_pFriendlyZeppelins[ zeppnum ]->PostMessage(MSGID_GET_NODE, NULL);
							NSVector3df zepptrans = *zeppnode->GetWorldTranslation();
							NSVector3df zeppfacing = zeppnode->GetFacingVector();
							zeppfacing.SetLength( s_fShipHorizontalSpawnOffset );
							zepptrans -= zeppfacing;
							NSMatrix4 zeppmat = *zeppnode->GetWorldMatrix();

							zepptrans.v.y += s_fShipVerticalSpawnOffset;

							data.alliance = FRIENDLY;
							data.mat = zeppmat;
							data.shipcontroller = this;
							data.trans = zepptrans;
							data.up = NSVector3df ( 0.0f, 0.0f, 0.0f );

							this->CreateShip( &data );

							this->m_fFriendlySpawnTimer = 0.0f;
						}
					}
				}


			}
		}

	}


	// enemy ships

	if ( this->m_iNumEnemyZeppelins > 0 )
	{
		if ( this->m_fEnemySpawnTimer >= this->s_fEnemySpawnTime )
		{
			// spawn a ship at each zeppelin
			for ( int zeppnum = 0; zeppnum < this->m_iNumEnemyZeppelins; zeppnum++ )
			{
				if ( this->m_iNumEnemyShips < CUFOMain::m_iMaxNumShips
					&&  this->m_iNumEnemyShips < SHIPS_MAX_NUM )
				{
					// create a new ship
					data.isPlayer = false;

					// find a random zeppelin carrier
//					int zeppnum = (int)(   ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iNumEnemyZeppelins  );

					if ( this->m_pEnemyZeppelins[ zeppnum ] != NULL )
					{
						if ( *(bool*)this->m_pEnemyZeppelins[ zeppnum ]->PostMessage(MSGID_IS_ALIVE, NULL) )
						{
							NSNode * zeppnode = (NSNode*)this->m_pEnemyZeppelins[ zeppnum ]->PostMessage(MSGID_GET_NODE, NULL);
							NSVector3df zepptrans = *zeppnode->GetWorldTranslation();
							NSVector3df zeppfacing = zeppnode->GetFacingVector();
							zeppfacing.SetLength( s_fShipHorizontalSpawnOffset );
							zepptrans -= zeppfacing;
							NSMatrix4 zeppmat = *zeppnode->GetWorldMatrix();

							zepptrans.v.y += s_fShipVerticalSpawnOffset;

							data.alliance = ENEMY;
							data.mat = zeppmat;
							data.shipcontroller = this;
							data.trans = zepptrans;
							data.up = NSVector3df ( 0.0f, 0.0f, 0.0f );

							this->CreateShip( &data );

							this->m_fEnemySpawnTimer = 0.0f;
						}
					}
				}
			}


		}
	}
}



void CShipController::SpawnZeppelins()
{
	int numzeppelins = CUFOMain::m_iNumZeppelins;

	float fZeppelinHeight = CUFOMain::m_fMeshHeightScale * CUFOMain::m_iAmplitude + this->s_fZeppelinHeight;

	float maxrandomxz = (float)CUFOMain::m_iMeshWidth;

	if ( (float)CUFOMain::m_iMeshLength < maxrandomxz )
		maxrandomxz = (float)CUFOMain::m_iMeshLength;

	// our origin is in the middle of the mesh
	// quarter it again to make things a bit more tense
	maxrandomxz *= (float)CUFOMain::m_iMeshSpacing / 6.0f;

	float randx, randz;

	CZeppelin::InitData zeppdata;
	NSVector3df zepptrans;
	NSMatrix4 zeppmat;


	// Friendly
	zeppdata.alliance = FRIENDLY;
	for ( int i = 0; i < numzeppelins; i++ )
	{
		zeppmat.SetIdentity();
		zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
		zeppdata.mat = zeppmat;


		randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
		if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
			randx *= -1.0f;
		randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
		if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
			randz *= -1.0f;

		zepptrans = NSVector3df( randx, fZeppelinHeight, randz);
		zeppdata.trans = zepptrans;

		this->CreateZeppelin( &zeppdata );
	}


	zeppdata.alliance = ENEMY;
	for ( int i = 0; i < numzeppelins; i++ )
	{
		zeppmat.SetIdentity();
		zeppmat.SetRotationY(   ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI   );
		zeppdata.mat = zeppmat;


		randx = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
		if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
			randx *= -1.0f;
		randz = ( (float)rand() / (float)RAND_MAX ) * maxrandomxz;
		if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
			randz *= -1.0f;

		zepptrans = NSVector3df( randx, fZeppelinHeight, randz);
		zeppdata.trans = zepptrans;

		this->CreateZeppelin( &zeppdata );
	}
}






/////////////////////////////
//      Zeppelins
/////////////////////////////

CTask * CShipController::CreateZeppelin( void * Data )
{
	CZeppelin::InitData * data = (CZeppelin::InitData*)Data;
	data->shipcontroller = this;

	int arraypos;
	CZeppelin * zeppelin;



	// create a new AI ship
	zeppelin = new CZeppelin();

	// get the proper array
	if ( data->alliance == FRIENDLY)
	{
		// create an AI ship if we havent hit our limit
		if ( this->m_iNumFriendlyZeppelins < ZEPPELINS_MAX_NUM )
		{
			this->m_pFriendlyZeppelins[ this->m_iNumFriendlyZeppelins ] = zeppelin;
			arraypos = this->m_iNumFriendlyZeppelins;
			this->m_iNumFriendlyZeppelins++;
		}
		else
		{
			OutputDebugString( _T("CShipController:: Cannot create new zeppelin - Max num already reached") );
			return NULL;
		}
	}
	else
	{
		// create an AI ship if we havent hit our limit
		if ( this->m_iNumEnemyZeppelins < ZEPPELINS_MAX_NUM )
		{
			this->m_pEnemyZeppelins[ this->m_iNumEnemyZeppelins ] = zeppelin;
			arraypos = this->m_iNumEnemyZeppelins;
			this->m_iNumEnemyZeppelins++;
		}
		else
		{
			OutputDebugString( _T("CShipController:: Cannot create new zeppelin - Max num already reached") );
			return NULL;
		}
	}


	// initialise the ship
	zeppelin->PostMessage( MSGID_INIT, Data );

	zeppelin->PostMessage( MSGID_SET_ARRAYPOSITION, &arraypos );

	// add the ship to the branch
	this->m_pZeppelinBranch->AttachChild( (NSNode*)zeppelin->PostMessage(MSGID_GET_NODE, NULL) );
	this->m_pZeppelinBulletBranch->AttachChild( (NSNode*)zeppelin->PostMessage(MSGID_GET_BULLETNODE, NULL) );

	return zeppelin;
}



void CShipController::DestroyZeppelin( CTask * zeppelin )
{
	// make sure no ships are looking at it
	this->InvalidateShip( zeppelin );

	// get the ships alliance
	ALLIANCE side = *(ALLIANCE*)zeppelin->PostMessage( MSGID_GET_ALLIANCE, NULL);
	int arraypos;

	CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);

	if (side == FRIENDLY)
	{
		// ally ship

		// get the ships array position
		arraypos = *(int*)zeppelin->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// delete the ship
		//delete this->m_pFriendlyShips[i];
		this->m_pZeppelinsPendingDeletion[ this->m_iNumZeppelinsPendingDeletion ] = this->m_pFriendlyZeppelins[ arraypos ];
		this->m_iNumZeppelinsPendingDeletion++;

		// shuffle the array
		this->m_iNumFriendlyZeppelins--;
		this->m_pFriendlyZeppelins[ arraypos ] = this->m_pFriendlyZeppelins[ this->m_iNumFriendlyZeppelins ];
		this->m_pFriendlyZeppelins[ this->m_iNumFriendlyZeppelins ] = NULL;

		// notify the ships of the change of array positions
		for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
		{
			this->m_pFriendlyZeppelins[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}

		// if the camera is point at a node further down the list,
		// decrement our counter so we dont skip a ship
		if ( this->m_iCameraTargetPos >= arraypos  && this->m_eCameraTarget == CAMTARG_ZEPFRIENDLY )
		{
			this->m_iCameraTargetPos--;
			// move the camera if its pointing at this
//			if ( this->m_iCameraTargetPos < 0  ||  this->m_iCameraTargetPos == arraypos )
//			{
				// clear it to make sure
				camera->PostMessage(MSGID_SET_TARGET, NULL );//targetnode);
				this->CameraCycleNext();
//			}
		}

		return;
	}
	else
	{
		// enemy ship

		// get the ships array position
		arraypos = *(int*)zeppelin->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// delete the ship
		//delete this->m_pEnemyShips[i];
		this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ] = this->m_pEnemyShips[ arraypos ];
		this->m_iNumShipsPendingDeletion++;

		// shuffle the array
		this->m_iNumEnemyShips--;
		this->m_pEnemyShips[ arraypos ] = this->m_pEnemyShips[ this->m_iNumEnemyShips ];
		this->m_pEnemyShips[ this->m_iNumEnemyShips ] = NULL;

		// notify the ships of the change of array positions
		for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		{
			this->m_pEnemyShips[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}

		if ( this->m_iCameraTargetPos >= arraypos  && this->m_eCameraTarget == CAMTARG_ZEPENEMY )
		{
			this->m_iCameraTargetPos--;
//			if ( this->m_iCameraTargetPos < 0  ||  this->m_iCameraTargetPos == arraypos )
//			{
				// clear it to make sure
				camera->PostMessage(MSGID_SET_TARGET, NULL );//targetnode);
				this->CameraCycleNext();
//			}
		}

		return;
	}

	// should never get here
	assert(0);
}


void CShipController::InvalidateZeppelin( CTask * zeppelin )
{
	// get the ships alliance
	ALLIANCE side = *(ALLIANCE*)zeppelin->PostMessage( MSGID_GET_ALLIANCE, NULL);
	int arraypos;

	if (side == FRIENDLY)
	{
		// ally ship

		// get the ships array position
		arraypos = *(int*)zeppelin->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// notify others of the deletion
		// notify the enemies
		for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		{
			this->m_pEnemyShips[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, zeppelin );
		}

		return;
	}
	else
	{
		// enemy ship

		// get the ships array position
		arraypos = *(int*)zeppelin->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// notify others of the deletion
		// notify the friendlies
		for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		{
			this->m_pFriendlyShips[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, zeppelin );
		}

		return;
	}

	// should never get here
	assert(0);
}









/////////////////////////////
//        Planes
/////////////////////////////

CTask * CShipController::CreateShip( void * Data )
{
	CShip * ship;
	CShip::InitData * data = (CShip::InitData*)Data;

	data->shipcontroller = this;

	int arraypos;

	// if its the player, create the player ship
	if ( data->isPlayer )
	{
		// if theres already one, then something bad has happened
		if ( this->m_pPlayerShip != NULL )
			assert(0);

		// create a new Player ship
		ship = new CShipPlayer();

		arraypos = 0;

		this->m_pPlayerShip = ship;
		this->m_bPlayer = true;

		CHUD::Get()->PostMessage( MSGID_HIDE_SPAWNTIMER, NULL );
	}
	else
	{
		// create a new AI ship
		ship = new CShipAI();

		// get the proper array
		if ( data->alliance == FRIENDLY)
		{
			// create an AI ship if we havent hit our limit
			if ( this->m_iNumFriendlyShips < SHIPS_MAX_NUM )
			{
				this->m_pFriendlyShips[ this->m_iNumFriendlyShips ] = ship;
				arraypos = this->m_iNumFriendlyShips;
				this->m_iNumFriendlyShips++;
			}
			else
			{
				OutputDebugString( _T("CShipController:: Cannot create new ship - Max num already reached") );
				delete ship;
				return NULL;
			}
		}
		else
		{
			// create an AI ship if we havent hit our limit
			if ( this->m_iNumEnemyShips < SHIPS_MAX_NUM )
			{
				this->m_pEnemyShips[ this->m_iNumEnemyShips ] = ship;
				arraypos = this->m_iNumEnemyShips;
				this->m_iNumEnemyShips++;
			}
			else
			{
				OutputDebugString( _T("CShipController:: Cannot create new ship - Max num already reached") );
				delete ship;
				return NULL;
			}
		}
	}

	// initialise the ship
	ship->PostMessage( MSGID_INIT, Data );

	ship->PostMessage( MSGID_SET_ARRAYPOSITION, &arraypos );

	// add the ship to the branch
	this->m_pShipBranch->AttachChild( (NSNode*)ship->PostMessage(MSGID_GET_NODE, NULL) );
	this->m_pBulletBranch->AttachChild( (NSNode*)ship->PostMessage(MSGID_GET_BULLETNODE, NULL) );

	return ship;
}


void CShipController::InvalidateShip( CTask * ship )
{
	// get the ships alliance
	ALLIANCE side = *(ALLIANCE*)ship->PostMessage( MSGID_GET_ALLIANCE, NULL);
//	int arraypos;

	if ( side == FRIENDLY )
	{
		// check if its the player
//		bool playership;

//		playership = *(bool*)ship->PostMessage(MSGID_IS_PLAYERSHIP, NULL);

//		if ( playership )
//		{
			// notify others of the deletion
			// notify our enemies
//			for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
//			{
//				this->m_pEnemyShips[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
//			}

			// notify our zeppelins incase their turrets are tracking us
//			for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
//			{
//				this->m_pEnemyZeppelins[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
//			}

//			this->m_bPlayer = false; // stop doing collisions!

//			return;
//		}

		// otherwise its an ally ship

		// get the ships array position
//		arraypos = *(int*)ship->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// notify others of the deletion
		// notify the enemies
		for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		{
			this->m_pEnemyShips[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
		}

		// notify our zeppelins incase their turrets are tracking us
		for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
		{
			this->m_pEnemyZeppelins[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
		}

		return;
	}
	else
	{
		// enemy ship

		// get the ships array position
//		arraypos = *(int*)ship->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// notify others of the deletion
		// notify the friendlies
		for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		{
			this->m_pFriendlyShips[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
		}

		// notify our zeppelins incase their turrets are tracking us
		for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
		{
			this->m_pFriendlyZeppelins[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
		}

		return;
	}

	// should never get here
	assert(0);
}



void CShipController::DestroyShip( CTask * ship )
{
	// make sure no ships are looking at it
	this->InvalidateShip( ship );

	// get the ships alliance
	ALLIANCE side = *(ALLIANCE*)ship->PostMessage( MSGID_GET_ALLIANCE, NULL);
	int arraypos;

	CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);

	if (side == FRIENDLY)
	{
		// check if its the player
		bool playership;

		playership = *(bool*)ship->PostMessage(MSGID_IS_PLAYERSHIP, NULL);

		if ( playership )
		{
			if ( this->m_eCameraTarget == CAMTARG_PLAYER )
			{
				// clear it to make sure
				camera->PostMessage(MSGID_SET_TARGET, NULL );//targetnode);
				this->CameraCycleNext();

				if ( this->m_iNumFriendlyZeppelins > 0 )
					CHUD::Get()->PostMessage( MSGID_SHOW_SPAWNTIMER, NULL );
			}

			// delete the ship
			// the ship has been Destroyed (since thats how we got called)
			// so we just delete it
			//delete this->m_pPlayerShip;
			this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ] = this->m_pPlayerShip;
			this->m_iNumShipsPendingDeletion++;

			this->m_pPlayerShip = NULL;

			this->m_bPlayer = false;

			return;
		}

		// otherwise its an ally ship

		// get the ships array position
		arraypos = *(int*)ship->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// delete the ship
		//delete this->m_pFriendlyShips[i];
		this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ] = this->m_pFriendlyShips[ arraypos ];
		this->m_iNumShipsPendingDeletion++;

		// shuffle the array
		this->m_iNumFriendlyShips--;
		this->m_pFriendlyShips[ arraypos ] = this->m_pFriendlyShips[ this->m_iNumFriendlyShips ];
		this->m_pFriendlyShips[ this->m_iNumFriendlyShips ] = NULL;

		// notify the ships of the change of array positions
		for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		{
			this->m_pFriendlyShips[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}

		if ( this->m_iCameraTargetPos >= arraypos  &&  this->m_eCameraTarget == CAMTARG_FRIENDLY )
		{
			this->m_iCameraTargetPos--;
//			if ( this->m_iCameraTargetPos < 0  ||  this->m_iCameraTargetPos == arraypos )
//			{
				// clear it to make sure
				camera->PostMessage(MSGID_SET_TARGET, NULL );//targetnode);
				this->CameraCycleNext();
//			}
		}

		return;
	}
	else
	{
		// enemy ship

		// get the ships array position
		arraypos = *(int*)ship->PostMessage(MSGID_GET_ARRAYPOSITION, NULL);

		// delete the ship
		//delete this->m_pEnemyShips[i];
		this->m_pShipsPendingDeletion[ this->m_iNumShipsPendingDeletion ] = this->m_pEnemyShips[ arraypos ];
		this->m_iNumShipsPendingDeletion++;

		// shuffle the array
		this->m_iNumEnemyShips--;
		this->m_pEnemyShips[ arraypos ] = this->m_pEnemyShips[ this->m_iNumEnemyShips ];
		this->m_pEnemyShips[ this->m_iNumEnemyShips ] = NULL;


		// notify the ships of the change of array positions
		for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		{
			this->m_pEnemyShips[i]->PostMessage( MSGID_SET_ARRAYPOSITION, &i );
		}

		if ( this->m_iCameraTargetPos >= arraypos  && this->m_eCameraTarget == CAMTARG_ENEMY )
		{
			this->m_iCameraTargetPos--;
//			if ( this->m_iCameraTargetPos < 0  ||  this->m_iCameraTargetPos == arraypos )
//			{
				// clear it to make sure
				camera->PostMessage(MSGID_SET_TARGET, NULL );//targetnode);
				this->CameraCycleNext();
//			}
		}

		return;
	}

	// should never get here
	assert(0);
}





void CShipController::CheckCollisionShip( CTask * s1, CTask * s2 )
{
	float r1, r2;
	r1 = *(float*) s1->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	r2 = *(float*) s2->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df v1, v2;
	v1 = *( (NSNode*) s1->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	v2 = *( (NSNode*) s2->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	NSSphere sp1 = NSSphere( v1, r1 );
	NSSphere sp2 = NSSphere( v2, r2 );

	float dist =  IntersectSphereSphere( sp1, sp2 );

	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("Sphere Collision\n") );
		
		// get the two ships velocity vectors
		NSVector3df *vel1, *vel2;
		vel1 = (NSVector3df*)s1->PostMessage(MSGID_GET_VELOCITY, NULL);
		vel2 = (NSVector3df*)s2->PostMessage(MSGID_GET_VELOCITY, NULL);

		float m1, m2;
		m1 = *(float*)s1->PostMessage(MSGID_GET_MASS, NULL);
		m2 = *(float*)s2->PostMessage(MSGID_GET_MASS, NULL);

		// get the difference between the velocities
		NSVector3df diff = *vel2 - *vel1;

		// apply a scale and apply the damage based on mass of the opposing ship
		float diffLength = diff.GetLength();
		int d1 = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * m2  );
		int d2 = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * m1  );
		s1->PostMessage(MSGID_APPLY_DAMAGE, &d1 );
		s2->PostMessage(MSGID_APPLY_DAMAGE, &d2 );

		// push the ships apart
//		float s1vel, s2vel;
//		s1vel = vel1->GetLength();
//		s2vel = vel2->GetLength();

		// switch directions
		NSVector3df * tempvel = new NSVector3df( 0.0f, 0.0f, 0.0f);
		*tempvel = *vel2;
		*vel2 = *vel1;
		*vel1 = *tempvel;
		delete tempvel;

		// Lose some velocity
		float newvelocity;
		newvelocity = vel1->GetLength();
		newvelocity -= (newvelocity * SHIP_COLLISION_RETURNSPEEDSCALE);
		vel1->SetLength( newvelocity );

		newvelocity = vel2->GetLength();
		newvelocity -= (newvelocity * SHIP_COLLISION_RETURNSPEEDSCALE);
		vel2->SetLength( newvelocity );
	}
}


void CShipController::CheckCollisionsShipVsShip()
{
	// check the player vs friendly ships
	if ( this->m_pPlayerShip != NULL )
		for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
			this->CheckCollisionShip( this->m_pPlayerShip, this->m_pFriendlyShips[i] );

	// check the player vs enemy ships
	if ( this->m_pPlayerShip != NULL )
		for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
			this->CheckCollisionShip( this->m_pPlayerShip, this->m_pEnemyShips[i] );


	// check friendly vs friendly ships
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		for ( int j = i + 1; j < this->m_iNumFriendlyShips; j++)
			this->CheckCollisionShip( this->m_pFriendlyShips[i], this->m_pFriendlyShips[j] );

	// check friendly vs enemy ships
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		for ( int j = 0; j < this->m_iNumEnemyShips; j++)
			this->CheckCollisionShip( this->m_pFriendlyShips[i], this->m_pEnemyShips[j] );

	// check enemy vs enemy ships
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		for ( int j = i + 1; j < this->m_iNumEnemyShips; j++)
			this->CheckCollisionShip( this->m_pEnemyShips[i], this->m_pEnemyShips[j] );
}


void CShipController::CheckCollisionsShipVsGround()
{
	CTask * world = (CTask*)CWorldManager::Get()->PostMessage(MSGID_GET_WORLD, NULL);

	// check player ship vs ground
	if ( this->m_pPlayerShip != NULL )
		world->PostMessage(MSGID_CHECK_COLLISIONSSPHEREVSGROUND, this->m_pPlayerShip);

	// check friendly ships vs ground
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		world->PostMessage( MSGID_CHECK_COLLISIONSSPHEREVSGROUND, this->m_pFriendlyShips[i] );

	// check enemy ships vs ground
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		world->PostMessage( MSGID_CHECK_COLLISIONSSPHEREVSGROUND, this->m_pEnemyShips[i] );
}


void CShipController::CheckCollisionBullet( CTask * bullet, CTask * s )
{
	// Dont continue if friendly fire is disabled and were on the same team
	if ( ! CUFOMain::m_bFriendlyFire )
	{
		ALLIANCE balliance = *(ALLIANCE*)bullet->PostMessage(MSGID_GET_ALLIANCE, NULL);
		ALLIANCE salliance = *(ALLIANCE*)s->PostMessage(MSGID_GET_ALLIANCE, NULL);

		if ( balliance == salliance )
			return;
	}

	// do collision detection
	float sr1, br2;
	// add an extra bullet hit radius to make gun fighting easier
	sr1 = *(float*) s->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL) + CShipController::s_fBulletAllowanceRadius;
	br2 = *(float*) bullet->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df sv1, bv2;
	sv1 = *( (NSNode*) s->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	bv2 = *( (NSNode*) bullet->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	NSSphere ssp1 = NSSphere( sv1, sr1 );
	NSSphere bsp2 = NSSphere( bv2, br2 );

	float dist =  IntersectSphereSphere( ssp1, bsp2 );

	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("Ship vs Bullet Collision\n") );
		
		int damage = *(int*)bullet->PostMessage(MSGID_GET_DAMAGEAMOUNT, NULL);
		s->PostMessage(MSGID_APPLY_DAMAGE, &damage );

		// kill the bullet
		bullet->PostMessage( MSGID_TRACER_HIT, NULL );
	}
}


void CShipController::CheckCollisionShipVsBullet( CTask * bullet )
{
	CTask * owner;
	owner = (CTask*)bullet->PostMessage(MSGID_GET_OWNER, NULL);

	// check player ship vs ground
	if ( this->m_pPlayerShip != NULL )
	{
		// dont shoot ourselves!
		if ( owner != this->m_pPlayerShip )
			this->CheckCollisionBullet( bullet, this->m_pPlayerShip );
	}

	// check friendly ships vs ground
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
	{
		if ( owner != this->m_pFriendlyShips[i] )
			this->CheckCollisionBullet( bullet, this->m_pFriendlyShips[i] );
	}

	// check enemy ships vs ground
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
	{
		if ( owner != this->m_pEnemyShips[i] )
			this->CheckCollisionBullet( bullet, this->m_pEnemyShips[i] );
	}
}




void CShipController::CheckCollisionsShipVsCity( CTask * Data )
{
	// check player ship vs ground
	if ( this->m_pPlayerShip != NULL )
		CCityManager::Get()->PostMessage(MSGID_CHECK_COLLISIONSHIPVSCITY, this->m_pPlayerShip);

	// check friendly ships vs ground
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		CCityManager::Get()->PostMessage(MSGID_CHECK_COLLISIONSHIPVSCITY, this->m_pFriendlyShips[i] );

	// check enemy ships vs ground
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		CCityManager::Get()->PostMessage(MSGID_CHECK_COLLISIONSHIPVSCITY, this->m_pEnemyShips[i] );
}






void CShipController::CheckCollisionShipZeppelin( CTask * ship, CTask * zeppelin )
{
//	if (  ! *(bool*)ship->PostMessage( MSGID_IS_ALIVE, NULL )  )
//		return;

	float shipr, zeppr;
	shipr = *(float*) ship->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	zeppr = *(float*) zeppelin->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df shipv, zeppv;
	shipv = *( (NSNode*) ship->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	NSNode * zeppnode = ( (NSNode*) zeppelin->PostMessage(MSGID_GET_NODE, NULL) );
	zeppv = *zeppnode->GetWorldTranslation();

	NSSphere shipsp = NSSphere( shipv, shipr );
	NSSphere zeppsp = NSSphere( zeppv, zeppr );

	// by supplying a matrix, we can create a 3d eliptical shape
	// this allows us to get a rough zeppelin look without the annoyance of using multiple boxes!
	// even though eliptical space is different (the vector gets translated and is squed slightly)
	// converting between space wouldnt make much different in the result.. so its fine for the time being
	// ie, 45 degrees on a sphere converted to an elipse via a matrix is not 45 degrees on
	// a normal elipse..
	NSMatrix4 zeppmat = *(NSMatrix4*) zeppelin->PostMessage( MSGID_GET_BOUNDINGMATRIX, NULL );
	NSMatrix4 zepporient = *zeppnode->GetMatrix();
	zeppmat *= zepporient;

	float dist =  IntersectSphereMatrixSphere( shipsp, zeppsp, zeppmat );

	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("Sphere Collision\n") );
		
		// get the two ships velocity vectors
		NSVector3df *shipvel, zeppvel;
		shipvel =  (NSVector3df*) ship->PostMessage(MSGID_GET_VELOCITY, NULL);
		zeppvel = *(NSVector3df*) zeppelin->PostMessage(MSGID_GET_VELOCITY, NULL);

		float shipm, zeppm;
		shipm = *(float*)ship->PostMessage(MSGID_GET_MASS, NULL);
		zeppm = *(float*)zeppelin->PostMessage(MSGID_GET_MASS, NULL);

		// get the difference between the velocities
		NSVector3df diff = zeppvel - *shipvel;

		// apply a scale and apply the damage based on mass of the opposing ship
		float diffLength = diff.GetLength();
		int shipd = (int)(  (diffLength * SHIP_COLLISION_VSZEPPELINDAMAGESCALE) * zeppm  );
		int zeppd = (int)(  (diffLength * SHIP_COLLISION_VSZEPPELINDAMAGESCALE) * shipm  );
		ship->PostMessage(MSGID_APPLY_DAMAGE, &shipd );
		zeppelin->PostMessage(MSGID_APPLY_DAMAGE, &zeppd );

		// push the ships apart
//		float s1vel, s2vel;
//		s1vel = vel1->GetLength();
//		s2vel = vel2->GetLength();

		//// switch directions
		//NSVector3df * tempvel = new NSVector3df( 0.0f, 0.0f, 0.0f);
		//*tempvel = *vel2;
		//*vel2 = *vel1;
		//*vel1 = *tempvel;
		//delete tempvel;

		//// Lose some velocity
		//float newvelocity;
		//newvelocity = vel1->GetLength();
		//newvelocity -= (newvelocity * SHIP_COLLISION_RETURNSPEEDSCALE);
		//vel1->SetLength( newvelocity );
	}
}

void CShipController::CheckCollisionZeppelinZeppelin( CTask * z1, CTask * z2 )
{
	float zepp1r, zepp2r;
	zepp1r = *(float*) z1->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	zepp2r = *(float*) z2->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df zepp1v, zepp2v;
	NSNode * zepp1node = ( (NSNode*) z1->PostMessage(MSGID_GET_NODE, NULL) );
	zepp1v = *zepp1node->GetWorldTranslation();
	NSNode * zepp2node = ( (NSNode*) z2->PostMessage(MSGID_GET_NODE, NULL) );
	zepp2v = *zepp2node->GetWorldTranslation();

	NSSphere zepp1sp = NSSphere( zepp1v, zepp1r );
	NSSphere zepp2sp = NSSphere( zepp2v, zepp2r );

	// by supplying a matrix, we can create a 3d eliptical shape
	// this allows us to get a rough zeppelin look without the annoyance of using multiple boxes!
	// even though eliptical space is different (the vector gets translated and is squed slightly)
	// converting between space wouldnt make much different in the result.. so its fine for the time being
	// ie, 45 degrees on a sphere converted to an elipse via a matrix is not 45 degrees on
	// a normal elipse..
	NSMatrix4 zepp2mat = *(NSMatrix4*) z2->PostMessage( MSGID_GET_BOUNDINGMATRIX, NULL );
	NSMatrix4 zepp2orient = *zepp2node->GetMatrix();
	zepp2mat *= zepp2orient;

	NSMatrix4 zepp1mat = *(NSMatrix4*) z1->PostMessage( MSGID_GET_BOUNDINGMATRIX, NULL );
	NSMatrix4 zepp1orient = *zepp1node->GetMatrix();
	zepp1mat *= zepp1orient;

	float dist =  IntersectMatrixSphereMatrixSphere( zepp1sp, zepp1mat, zepp2sp, zepp2mat );

	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("Sphere Collision\n") );
		
		// get the two ships velocity vectors
		NSVector3df zepp1vel, zepp2vel;
		zepp1vel = *(NSVector3df*) z1->PostMessage(MSGID_GET_VELOCITY, NULL);
		zepp2vel = *(NSVector3df*) z2->PostMessage(MSGID_GET_VELOCITY, NULL);

		float zepp1m, zepp2m;
		zepp1m = *(float*)z1->PostMessage(MSGID_GET_MASS, NULL);
		zepp2m = *(float*)z2->PostMessage(MSGID_GET_MASS, NULL);

		// get the difference between the velocities
		NSVector3df diff = zepp2vel - zepp1vel;

		// apply a scale and apply the damage based on mass of the opposing ship
		float diffLength = diff.GetLength();
		int zepp1d = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * zepp2m  );
		int zepp2d = (int)(  (diffLength * SHIP_COLLISION_VSSHIPDAMAGESCALE) * zepp1m  );
		z1->PostMessage(MSGID_APPLY_DAMAGE, &zepp1d );
		z2->PostMessage(MSGID_APPLY_DAMAGE, &zepp2d );
	}
}




void CShipController::CheckCollisionsVsZeppelin()
{
	//////////////////////
	// Ship Vs Zeppelin

	// check the player ship vs friendly zeppelins
	if ( this->m_pPlayerShip != NULL )
		for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
			this->CheckCollisionShipZeppelin( this->m_pPlayerShip, this->m_pFriendlyZeppelins[i] );

	// check the player ship vs enemy zeppelins
	if ( this->m_pPlayerShip != NULL )
		for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
			this->CheckCollisionShipZeppelin( this->m_pPlayerShip, this->m_pEnemyZeppelins[i] );


	// check friendly ship vs friendly zeppelins
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		for ( int j = 0; j < this->m_iNumFriendlyZeppelins; j++)
			this->CheckCollisionShipZeppelin( this->m_pFriendlyShips[i], this->m_pFriendlyZeppelins[j] );

	// check friendly ship vs enemy zeppelins
	for ( int i = 0; i < this->m_iNumFriendlyShips; i++ )
		for ( int j = 0; j < this->m_iNumEnemyZeppelins; j++)
			this->CheckCollisionShipZeppelin( this->m_pFriendlyShips[i], this->m_pEnemyZeppelins[j] );

	// check enemy ship vs enemy zeppelins
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		for ( int j = 0; j < this->m_iNumEnemyZeppelins; j++)
			this->CheckCollisionShipZeppelin( this->m_pEnemyShips[i], this->m_pEnemyZeppelins[j] );

	// check enemy ships vs friendly zeppelins
	for ( int i = 0; i < this->m_iNumEnemyShips; i++ )
		for ( int j = 0; j < this->m_iNumFriendlyZeppelins; j++)
			this->CheckCollisionShipZeppelin( this->m_pEnemyShips[i], this->m_pFriendlyZeppelins[j] );


	//////////////////////
	// Zeppelin Vs Zeppelin

	// check friendly zeppelins vs friendly zeppelins
	// check friendly vs friendly zeppelins
	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
		for ( int j = i + 1; j < this->m_iNumFriendlyZeppelins; j++)
			this->CheckCollisionZeppelinZeppelin( this->m_pFriendlyZeppelins[i], this->m_pFriendlyZeppelins[j] );

	// check enemy zeppelins vs enemy zeppelins
	// check enemy vs enemy zeppelins
	for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
		for ( int j = i + 1; j < this->m_iNumEnemyZeppelins; j++)
			this->CheckCollisionZeppelinZeppelin( this->m_pEnemyZeppelins[i], this->m_pEnemyZeppelins[j] );

	// check friendly zeppelins vs enemy zeppelins
	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
		for ( int j = 0; j < this->m_iNumEnemyZeppelins; j++)
			this->CheckCollisionZeppelinZeppelin( this->m_pFriendlyZeppelins[i], this->m_pEnemyZeppelins[j] );
}



void CShipController::CheckCollisionZeppBullet( CTask * bullet, CTask * z )
{
	// Dont continue if friendly fire is disabled and were on the same team
	if ( ! CUFOMain::m_bFriendlyFire )
	{
		ALLIANCE balliance = *(ALLIANCE*)bullet->PostMessage(MSGID_GET_ALLIANCE, NULL);
		ALLIANCE zalliance = *(ALLIANCE*)z->PostMessage(MSGID_GET_ALLIANCE, NULL);

		if ( balliance == zalliance )
			return;
	}

	// do collision detection
	float zr1, br2;
	zr1 = *(float*) z->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	br2 = *(float*) bullet->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	NSVector3df zv1, bv2;
	zv1 = *( (NSNode*) z->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	bv2 = *( (NSNode*) bullet->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	NSSphere zsp1 = NSSphere( zv1, zr1 );
	NSSphere bsp2 = NSSphere( bv2, br2 );

	// by supplying a matrix, we can create a 3d eliptical shape
	// this allows us to get a rough zeppelin look without the annoyance of using multiple boxes!
	// even though eliptical space is different (the vector gets translated and is squed slightly)
	// converting between space wouldnt make much different in the result.. so its fine for the time being
	// ie, 45 degrees on a sphere converted to an elipse via a matrix is not 45 degrees on
	// a normal elipse..
	NSMatrix4 zeppmat = *(NSMatrix4*) z->PostMessage( MSGID_GET_BOUNDINGMATRIX, NULL );

	float dist =  IntersectSphereMatrixSphere( bsp2, zsp1, zeppmat );

	if ( dist <= 0.0f )
	{
//		OutputDebugString( _T("Ship vs Bullet Collision\n") );
		
		int damage = *(int*)bullet->PostMessage(MSGID_GET_DAMAGEAMOUNT, NULL);
		z->PostMessage(MSGID_APPLY_DAMAGE, &damage );

		// kill the bullet
		bullet->PostMessage( MSGID_TRACER_HIT, NULL );
	}
}


void CShipController::CheckCollisionZeppelinVsBullet( CTask * bullet )
{
	CTask * owner;
	owner = (CTask*)bullet->PostMessage(MSGID_GET_OWNER, NULL);

	// check friendly ships vs ground
	for ( int i = 0; i < this->m_iNumFriendlyZeppelins; i++ )
	{
		if ( owner != this->m_pFriendlyZeppelins[i] )
			this->CheckCollisionZeppBullet( bullet, this->m_pFriendlyZeppelins[i] );
	}

	// check enemy ships vs ground
	for ( int i = 0; i < this->m_iNumEnemyZeppelins; i++ )
	{
		if ( owner != this->m_pEnemyZeppelins[i] )
			this->CheckCollisionZeppBullet( bullet, this->m_pEnemyZeppelins[i] );
	}
}



