#include "Turret.h"

int				CTurret::s_iNumTurrets			= 0;

const float		CTurret::s_fTimeBetweenShots	= 0.2f;
const float		CTurret::s_fAvgTargetSearchTime	= 0.7f;
float			CTurret::s_fMaxTracerDistanceShip	= 500.0f;	// tracer speed * lifetime ~ 1000.0f
float			CTurret::s_fMaxTracerDistanceCity	= 700.0f;

float			CTurret::s_fTracerSpeed			= 400.0f;	// is updated during init anyway



void * CTurret::PostMessage(int ID, void * Data)
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


		case MSGID_DISABLE:
			this->m_bActive = false;
			break;

		case MSGID_ENABLE:
			this->m_bActive = true;
			break;


		case MSGID_GET_NODE:
			result = this->m_pNode;
			break;


		case MSGID_GET_BULLETNODE:
			result = this->m_pBulletNode;
			break;

		case MSGID_NOTIFY_OF_DEATH:
			this->NotifyOfShipDeath( (CTask*)Data );
			break;

		case MSGID_NOTIFY_CITYDEATH:
			this->NotifyOfCityDeath( *(int*)Data );
			break;

		default:
			assert(0);
	}

	return result;
}



CTurret::CTurret()
{
}

CTurret::~CTurret()
{
}




void CTurret::Init( void * Data )
{
	CTurret::InitData data = *(CTurret::InitData*)Data;

	// create our nodes
	char buff[20];
	sprintf(buff, "turret%i", this->s_iNumTurrets);
	this->m_pNode = new NSNode( buff, data.trans );

	// copy the data
	this->m_kFacing = data.facing;
	this->m_pParentNode = data.parentnode;
	this->m_pBulletParent = data.bulletbranch;
	this->m_fMaxDotFiringAngle = data.maxdotfiringangle;
	this->m_pTarget = NULL;
	this->m_fTargetSearchTimer = 0.0f;
	this->m_eAlliance = data.alliance;
	this->m_bTargetIsCity = false;
	this->m_fGunTimer = 0.0f;
	this->m_bActive = true;

	this->m_pParent = data.parent;

	// add a random time to this so all turrets on a vehicle dont search at once and lag us
	this->m_fMinTargetSearchTime = ( (float)rand() / (float)RAND_MAX ) * 0.5f;
	this->m_fMinTargetSearchTime += this->s_fAvgTargetSearchTime;

	this->m_pParentNode->AttachChild( this->m_pNode );

	// create our bullet branch
	sprintf(buff, "turret%i", this->s_iNumTurrets);
	this->m_pBulletNode = new NSNode( buff );

	this->m_pBulletParent->AttachChild( this->m_pBulletNode );

	this->m_iNumTracers = 0;


	this->s_iNumTurrets++;
}

void CTurret::Destroy()
{
	for ( int i = 0; i < this->m_iNumTracers; i++ )
	{
		this->m_pTracers[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pTracers[i];
	}

	this->m_iNumTracers = 0;

	delete this->m_pBulletNode;
	this->m_pBulletNode = NULL;

	delete this->m_pNode;
	this->m_pNode = NULL;

	this->m_pParentNode = NULL;
	this->m_pBulletParent = NULL;
	this->m_pTarget = NULL;

	this->s_iNumTurrets--;
}

void CTurret::Update( void * Data )
{
	this->m_fTimeDelta = *(float*)Data;

	if ( this->m_bActive )
	{
		// reduce our gun timer
		if ( this->m_fGunTimer > 0.0f )
			this->m_fGunTimer -= this->m_fTimeDelta;

		if ( this->m_pTarget == NULL )
		{
			// if weve hit our timer
			if ( this->m_fTargetSearchTimer >= this->m_fMinTargetSearchTime )
			{
				// try and get a target
				this->SearchForTarget();

				// reset our timer
				this->m_fTargetSearchTimer = 0.0f;
			}

			// increment our search timer
			this->m_fTargetSearchTimer += this->m_fTimeDelta;
		}
		else
		{
			NSNode * targnode = (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL );
			NSVector3df trans = *this->m_pNode->GetWorldTranslation();
			NSVector3df targvec = *targnode->GetWorldTranslation();
			targvec = targvec - trans;

			// make sure our target is within range
			if ( targvec.GetLength() > this->s_fMaxTracerDistanceShip  &&  !this->m_bTargetIsCity )
			{
				// ships
				// if its not, set our target to NULL
				this->m_pTarget = NULL;
			}
			else if ( targvec.GetLength() > this->s_fMaxTracerDistanceCity  &&  this->m_bTargetIsCity )
			{
				// cities
				// if its not, set our target to NULL
				this->m_pTarget = NULL;
			}
			else
			{
				// check if its within our dot
				if ( this->m_kFacing.Dot( targvec ) > this->m_fMaxDotFiringAngle )
				{
					// do a plane check to see if the target is infront of the turret and not behind

					// create a plane along our horizontal axis facing up
					NSPlane frontplane = NSPlane( *this->m_pNode->GetTranslation(), this->m_kFacing );
					float frontvalue = IntersectPointPlane( targvec, frontplane );

					// fire on our target
					if ( frontvalue > 0.0f )
						this->FireGuns();
				}
				else
				{
					// if its not, set our target to NULL
					this->m_pTarget = NULL;
				}
			}
		}
	}

	// Update our tracers
	this->UpdateTracers();
}


void CTurret::UpdateTracers()
{
	// check for dead tracers
	for ( int i = 0; i < this->m_iNumTracers; i++ )
	{
		// check if the tracer is dead
		if ( *(bool*)this->m_pTracers[i]->PostMessage(MSGID_IS_LIFETIMEEXPIRED, NULL) )
		{
			// if it is, hide it
			this->m_pTracers[i]->PostMessage( MSGID_DESTROY, NULL );

			// shuffle down the array
			this->m_iNumTracers--;
			this->m_pTracers[i] = this->m_pTracers[ this->m_iNumTracers ];
			this->m_pTracers[ this->m_iNumTracers ] = NULL;
		}
	}

	// update our alive ones
	// we do this seperately since shuffling the array can cause some to be missed
	for ( int i = 0; i < this->m_iNumTracers; i++ )
	{
		this->m_pTracers[i]->PostMessage( MSGID_UPDATE, &this->m_fTimeDelta );
	}
}



void CTurret::NotifyOfShipDeath( CTask * ship )
{
	if ( this->m_pTarget == NULL )
		return;

	if ( this->m_bTargetIsCity )
		return;

	bool targisplayer = *(bool*)this->m_pTarget->PostMessage( MSGID_IS_PLAYERSHIP, NULL );;
	bool shipisplayer = *(bool*)this->m_pTarget->PostMessage( MSGID_IS_PLAYERSHIP, NULL );

	SHIP_TYPE shiptype = *(SHIP_TYPE*)ship->PostMessage( MSGID_GET_SHIPTYPE, NULL );
	SHIP_TYPE targtype = *(SHIP_TYPE*)this->m_pTarget->PostMessage( MSGID_GET_SHIPTYPE, NULL );

	int targarraypos = *(int*)this->m_pTarget->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );
	int shiparraypos = *(int*)ship->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );

	if ( targarraypos == shiparraypos  &&  targisplayer == shipisplayer  &&  shiptype == targtype )
	{
		this->m_pTarget = NULL;
		this->m_bTargetIsCity = false;
	}
}


void CTurret::NotifyOfCityDeath( int arraypos )
{
	if ( this->m_pTarget == NULL )
		return;

	if ( !this->m_bTargetIsCity )
		return;

	int targarraypos = *(int*)this->m_pTarget->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );

	if ( targarraypos == arraypos )
	{
		this->m_pTarget = NULL;
		this->m_bTargetIsCity = false;
	}
}



void CTurret::SearchForTarget()
{
	int num;
	CTask ** targets;

	NSVector3df targvec;
	NSVector3df trans = *this->m_pNode->GetWorldTranslation();
	NSNode * node;
	float currdist;
	float bestdist = -1.0f;
	int bestarraypos = -1;

	///////////////////
	// Cities

	// we prefer to target cities since theyre our main objective.. and they dont move!

	// get the number of cities
	// get the cities array
	if ( this->m_eAlliance == FRIENDLY )
	{
		num = *(int*)CCityManager::Get()->PostMessage( MSGID_GET_NUMENEMYCITIES, NULL );
		targets = (CTask**)CCityManager::Get()->PostMessage( MSGID_GET_ENEMYCITIESARRAY, NULL );
	}
	else
	{
		num = *(int*)CCityManager::Get()->PostMessage( MSGID_GET_NUMFRIENDLYCITIES, NULL );
		targets = (CTask**)CCityManager::Get()->PostMessage( MSGID_GET_FRIENDLYCITIESARRAY, NULL );
	}

	// find a city within our range
	for ( int i = 0; i < num; i++ )
	{
		node = (NSNode*)targets[i]->PostMessage(MSGID_GET_NODE, NULL);
		targvec = *node->GetWorldTranslation();
		targvec = targvec - trans;
		currdist = targvec.GetLength();

		// if its the closest target or its our first target
		if ( currdist < bestdist  ||  bestdist < 0.0f )
		{
			bestdist = currdist;
			bestarraypos = i;
		}
	}

	// if weve found a city target
	if ( bestarraypos > -1 )
	{
		if ( bestdist < this->s_fMaxTracerDistanceCity )
		{
			this->m_pTarget = targets[ bestarraypos ];
			this->m_bTargetIsCity = true;
			return;
		}
	}



	///////////////////
	// Ships

	// otherwise, get the number ships
	// get the ships array
	bestdist = -1.0f;
	bestarraypos = -1;
	if ( this->m_eAlliance == FRIENDLY )
	{
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYSHIPS, NULL );
		targets = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYSHIPSARRAY, NULL );
	}
	else
	{
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYSHIPS, NULL );
		targets = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYSHIPSARRAY, NULL );
	}


	// find a ship within our range
	for ( int i = 0; i < num; i++ )
	{
		node = (NSNode*)targets[i]->PostMessage(MSGID_GET_NODE, NULL);
		targvec = *node->GetWorldTranslation();
		targvec = targvec - trans;
		currdist = targvec.GetLength();

		// if its the closest target or its our first target
		if ( currdist < bestdist  ||  bestdist < 0.0f )
		{
			bestdist = currdist;
			bestarraypos = i;
		}
	}

	// if were checking for the enemy check for the player's ship
	if ( this->m_eAlliance == ENEMY )
	{
		CTask * player = (CTask*)CShipController::Get()->PostMessage( MSGID_GET_PLAYERSHIP, NULL );
		if ( player != NULL )
		{
			node = (NSNode*)player->PostMessage(MSGID_GET_NODE, NULL);
			targvec = *node->GetWorldTranslation();
			targvec = targvec - trans;
			currdist = targvec.GetLength();

			// if its the closest target or its our first target
			if ( currdist < bestdist  ||  bestdist < 0.0f )
			{
				if ( currdist < this->s_fMaxTracerDistanceShip )
				{
					if ( player != NULL )
					{
						if ( *(bool*)player->PostMessage(MSGID_IS_ALIVE, NULL) )
						{
							this->m_pTarget = player;
							this->m_bTargetIsCity = false;
							return;
						}
					}
				}
			}
		}
	}



	// if weve found a city target
	if ( bestarraypos > -1 )
	{
		if ( bestdist < this->s_fMaxTracerDistanceShip )
		{
			if ( targets[ bestarraypos ] != NULL )
			{
				if ( *(bool*)targets[ bestarraypos ]->PostMessage(MSGID_IS_ALIVE, NULL) )
				{
					this->m_pTarget = targets[ bestarraypos ];
					this->m_bTargetIsCity = false;
					return;
				}
			}
		}
	}

	// we havent found a target
	this->m_pTarget = NULL;
	this->m_bTargetIsCity = false;






	///////////////////
	// Zeppelins

	// otherwise, get the number ships
	// get the ships array
	bestdist = -1.0f;
	bestarraypos = -1;
	if ( this->m_eAlliance == FRIENDLY )
	{
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYZEPPELINS, NULL );
		targets = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYZEPPELINSARRAY, NULL );
	}
	else
	{
		num = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYZEPPELINS, NULL );
		targets = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYZEPPELINSARRAY, NULL );
	}


	// find a ship within our range
	for ( int i = 0; i < num; i++ )
	{
		node = (NSNode*)targets[i]->PostMessage(MSGID_GET_NODE, NULL);
		targvec = *node->GetWorldTranslation();
		targvec = targvec - trans;
		currdist = targvec.GetLength();

		// if its the closest target or its our first target
		if ( currdist < bestdist  ||  bestdist < 0.0f )
		{
			bestdist = currdist;
			bestarraypos = i;
		}
	}


	// if weve found a city target
	if ( bestarraypos > -1 )
	{
		if ( bestdist < this->s_fMaxTracerDistanceShip )
		{
			if ( targets[ bestarraypos ] != NULL )
			{
				if ( *(bool*)targets[ bestarraypos ]->PostMessage(MSGID_IS_ALIVE, NULL) )
				{
					this->m_pTarget = targets[ bestarraypos ];
					this->m_bTargetIsCity = false;
					return;
				}
			}
		}
	}

	// we havent found a target
	this->m_pTarget = NULL;
	this->m_bTargetIsCity = false;
}


void CTurret::FireGuns()
{
	// if its time to shoot
	if ( this->m_fGunTimer <= 0.0f )
	{

		// if were not at our max number of tracers
		if ( this->m_iNumTracers < SHIP_GUNS_MAXNUMTRACERS )
		{
			// set our timer
			this->m_fGunTimer = this->s_fTimeBetweenShots;

			// create a bullet
			this->m_pTracers[ this->m_iNumTracers ] = new CTracer();

			NSVector3df trans = *this->m_pNode->GetWorldTranslation();
			NSVector3df targvec;

			if ( !this->m_bTargetIsCity )
			{
				targvec = this->GetFiringVector();
			}
			else
			{
				NSNode * targnode = (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL );
				targvec = *targnode->GetWorldTranslation();
			}

			targvec = targvec - trans;

//			// move the guns forward
//			NSVector3df gunforwarddistance = facing;
//			gunforwarddistance.Scale( SHIP_GUNS_FORWARDDISTANCE );
//			pos += gunforwarddistance;

			CTracer::TracerSetupData tdata;
			tdata.pos = trans;
			tdata.dir = targvec;
			tdata.shipvelo = NSVector3df(0.0f, 0.0f, 0.0f);

			tdata.alliance = this->m_eAlliance;
			tdata.owner = this;
			tdata.bulletBranch = this->m_pBulletNode;
			tdata.flatworld = false;

			// initialise the bullet
			this->m_pTracers[ this->m_iNumTracers ]->PostMessage( MSGID_INIT, &tdata );

			this->m_iNumTracers++;

			this->m_pParent->PostMessage( MSGID_NOTIFY_FIRING, NULL );

			// play a sound
			CTask *camera = (CTask*)CGameController::Get()->PostMessage( MSGID_GET_CAMERA, NULL );
			trans = *( (NSNode*)camera->PostMessage( MSGID_GET_NODE, NULL ) )->GetWorldTranslation();
			trans -= *this->m_pNode->GetWorldTranslation();

			CSoundController::SoundData sounddata;
			sounddata.sound = CSoundController::ZEPPELIN_GUN;
			sounddata.distance = trans.GetLength();
			sounddata.frequency = -1;
			sounddata.volume = 2;

			CSoundController::Get()->PostMessage( MSGID_PLAY_SOUND, &sounddata );
		}

	}
}



NSVector3df CTurret::GetFiringVector()
{
	NSVector3df targtrans;
	NSVector3df targvec;
	NSVector3df targvelo;
	NSVector3df trans;

	// Get the components we need
	trans = *this->m_pNode->GetWorldTranslation();

	targtrans = *( (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	targvelo = *(NSVector3df*)this->m_pTarget->PostMessage( MSGID_GET_VELOCITY, NULL);

	targvec = targtrans - trans;

	// work out the targets distance and velocity
	float ftargdist = targvec.GetLength();
	float ftargvelo = targvelo.GetLength();

	// work out the time to the target
	float timetotarget = ftargdist / this->s_fTracerSpeed;

	// multiply his velocity by this
	targvelo *= timetotarget;

	// add the velocity to the world translation
	targtrans += targvelo;

	// this is our new target vector

	return targtrans;
}





