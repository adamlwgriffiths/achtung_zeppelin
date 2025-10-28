#include "Zeppelin.h"


int				CZeppelin::s_iNumZeppelins					= 0;

const float		CZeppelin::s_fTurnRate						= 0.1f;
const float		CZeppelin::s_fGravityPullStrength			= 1.0f;
const float		CZeppelin::s_fDiveRate						= 0.1f;

const int		CZeppelin::s_iMaxHealth						= 1000;
const int		CZeppelin::s_iMinHealth						= 0;

float			CZeppelin::s_fMass							= 2000.0f;
const float		CZeppelin::s_fSpeed							= 10.0f;



float			CZeppelin::s_fBBRadius						=  3.0f;
NSMatrix4		CZeppelin::s_kBBMatrix						= NSMatrix4();


const float		CZeppelin::s_fTargetSearchTime				= 3.0f;




void * CZeppelin::PostMessage(int ID, void * Data)
{
	void * result = 0;
	bool bResult;
	float fResult;

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



		case MSGID_GET_NODE:
			result = this->m_pNode;
			break;

		case MSGID_GET_BULLETNODE:
			result = this->m_pBulletNode;
			break;



		case MSGID_GET_ALLIANCE:
			result = &this->m_eAlliance;
			break;

		case MSGID_SET_ARRAYPOSITION:
			this->m_iArrayPosition = *(int*)Data;
			break;

		case MSGID_GET_ARRAYPOSITION:
			result = &this->m_iArrayPosition;
			break;


		case MSGID_IS_ALIVE:
			bResult = this->m_bAlive;
			result = &bResult;
			break;



		case MSGID_GET_VELOCITY:
			result = &this->m_kVelocity;
			break;

		case MSGID_APPLY_DAMAGE:
#ifdef ZEPPELIN_DAMAGEABLE
			this->ApplyDamage( *(int*)Data );
#endif // #ifdef ZEPPELIN_DAMAGEABLE
			break;

		case MSGID_GET_MASS:
			result = &this->s_fMass;
			break;

		case MSGID_GET_SHIPTYPE:
			result = &this->m_eShipType;
			break;


		case MSGID_NOTIFY_OF_DEATH:
			this->NotifyOfShipDeath( (CTask*)Data );
			break;

		case MSGID_NOTIFY_CITYDEATH:
			this->NotifyOfCityDeath( *(int*)Data );
			break;

		case MSGID_GET_BOUNDINGRADIUS:
			result = &this->s_fBBRadius;
			break;

		case MSGID_GET_BOUNDINGMATRIX:
			result = &this->s_kBBMatrix;
			break;

		case MSGID_IS_PLAYERSHIP:
			bResult = false;
			result = &bResult;
			break;

		case MSGID_GET_THROTTLE:
			if ( this->m_bAlive )
				fResult = 100.0f;
			else
				fResult = 10.0f;
			result = &fResult;
			break;

		case MSGID_IS_FIRING:
			result = &this->m_bIsFiring;
			break;

		case MSGID_NOTIFY_FIRING:
			this->m_bIsFiring = true;
			break;

		case MSGID_GET_STATE_DESCRIPTION:
			result = this->m_pStateDescription;
			break;

		case MSGID_GET_HEALTH:
			result = &this->m_iHealth;
			break;


		default:
			assert(0);
	}

	return result;
}



CZeppelin::CZeppelin()
{
	// set our BB scale
	this->s_kBBMatrix.SetIdentity();
	this->s_kBBMatrix.SetScale( 1.0f, 1.0f, 6.0f );
}

CZeppelin::~CZeppelin()
{
}




void CZeppelin::Init( void * Data )
{
	CZeppelin::InitData data = *(CZeppelin::InitData*)Data;

	// get our info from the init data
	NSVector3df trans = data.trans;

	// create our nodes
	char buff[20];
	sprintf(buff, "zeppelin%i", this->s_iNumZeppelins);
	this->m_pNode = new NSNode( buff, trans );

	sprintf(buff, "zeppbullbranch%i", this->s_iNumZeppelins);
	this->m_pBulletNode = new NSNode(buff);

	this->s_iNumZeppelins++;



	this->m_pNode->SetMatrix( data.mat );

	this->m_pShipController = data.shipcontroller;

	this->m_eState = CZeppelin::FLYING;

	this->m_iHealth = CZeppelin::s_iMaxHealth;
	this->m_eAlliance = data.alliance;

	this->m_bColliding = false;
	this->m_bAlive = true;
	this->m_bIsFiring = false;

	this->m_eShipType = ZEPPELIN;

	this->m_pTarget = NULL;

	this->m_fTargetSearchTimer = 0.0f;


	// attach our mesh
	CMeshManager::MESHES meshtype;

	// create and attach a mesh
	if (this->m_eAlliance == FRIENDLY)
		meshtype = CMeshManager::ZEPPELIN_FRIEND;
	else
		meshtype = CMeshManager::ZEPPELIN_ENEMY;

	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pNode->AttachMesh( mesh );

	sprintf( this->m_pStateDescription, "Flying");


	// create our turrets
	this->CreateTurrets();
}



void CZeppelin::Destroy()
{
	this->s_iNumZeppelins--;

	// delete our turrets
	for ( int i = 0; i < this->m_iNumTurrets; i++ )
	{
		this->m_pTurrets[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pTurrets[i];
	}
	//this->m_pTurrets[0]->PostMessage(MSGID_DESTROY, NULL);
	//delete this->m_pTurrets[0];

	// delete ourself
	delete this->m_pNode;
	this->m_pNode = NULL;
}


void CZeppelin::CreateTurrets()
{
	CTurret::InitData data;

	NSVector3df facingvec;
	NSVector3df trans;
	float maxdot;

	data.parentnode = this->m_pNode;
	data.parent = this;
	data.bulletbranch = this->m_pBulletNode;
	data.alliance = this->m_eAlliance;

	// Turret 1
	// TOP FRONT
	// Facing upward
	// Max dot of 0.0f (180 degree firing arc)
	this->m_pTurrets[ 0 ] = new CTurret();
	facingvec = NSVector3df( 0.0f, 1.0f, 0.0f );
	trans = this->m_pNode->GetFacingVector();
	trans.SetLength( 10.0f );
	trans.v.y += 10.0f;
	maxdot = 0.0f;

	data.facing = this->m_pNode->GetUpVector();//facingvec;
	data.trans = trans;
	data.maxdotfiringangle = maxdot;

	this->m_pTurrets[ 0 ]->PostMessage( MSGID_INIT, &data );


	// Turret 2
	// TOP CONNING-TOWER
	// Facing upward
	// Max dot of -0.1f (~160 degree firing arc)
	this->m_pTurrets[ 1 ] = new CTurret();
	facingvec = NSVector3df( 0.0f, 1.0f, 0.0f );
	trans = this->m_pNode->GetFacingVector();
	trans.SetLength( 5.0f );
	trans.v.y += 10.0f;
	// place to the right (on the tower)
	NSVector3df temptrans = this->m_pNode->GetCrossVector();
	temptrans.Invert();
	temptrans.SetLength( 3.0f );
	trans += temptrans;
	maxdot = 0.2f;

	data.facing = facingvec;
	data.trans = trans;
	data.maxdotfiringangle = maxdot;

	this->m_pTurrets[ 1 ]->PostMessage( MSGID_INIT, &data );



	// Turret 3
	// BOTTOM FRONT TOWER
	// Facing forward and down
	// Max dot of 0.2f (~140 degree firing arc)
	this->m_pTurrets[ 2 ] = new CTurret();
	facingvec = this->m_pNode->GetFacingVector();
	facingvec.Normalise();
	facingvec.v.y -= 2.0f;
	facingvec.Normalise();
	trans = this->m_pNode->GetFacingVector();
	trans.SetLength( 10.0f );
	trans.v.y -= 10.0f;
	maxdot = 0.5f;

	data.facing = facingvec;
	data.facing.Invert();
	data.trans = trans;
	data.maxdotfiringangle = maxdot;

	this->m_pTurrets[ 2 ]->PostMessage( MSGID_INIT, &data );


	// Turret 4
	// BOTTOM FRONT TOWER
	// Facing backward and down
	// Max dot of 0.2f (~80 degree firing arc)
	this->m_pTurrets[ 3 ] = new CTurret();
	facingvec = this->m_pNode->GetFacingVector();
	facingvec.Invert();
	facingvec.Normalise();
	facingvec.v.y -= 2.0f;
	facingvec.Normalise();
	trans = this->m_pNode->GetFacingVector();
	trans.SetLength( 5.0f );
	trans.v.y -= 10.0f;
	maxdot = 0.5f;

	data.facing = facingvec;
	data.facing.Invert();
	data.trans = trans;
	data.maxdotfiringangle = maxdot;

	this->m_pTurrets[ 3 ]->PostMessage( MSGID_INIT, &data );

}




void CZeppelin::Update( void * Data )
{
	this->m_fTimeDelta = *(float*)Data;

	this->m_bIsFiring = false;

	this->m_kVelocity = this->m_pNode->GetFacingVector();
	this->m_kVelocity.SetLength( this->s_fSpeed );

	if ( this->m_eState == CZeppelin::FLYING )
	{
		if ( this->m_pTarget == NULL )
		{
			// get a target
			// if weve hit our timer
			if ( this->m_fTargetSearchTimer >= this->s_fTargetSearchTime )
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
			this->AIGoToTarget();
		}

		// update our turrets
		for ( int i = 0; i < this->m_iNumTurrets; i++ )
		{
			this->m_pTurrets[i]->PostMessage( MSGID_UPDATE, &this->m_fTimeDelta );
		}
	}
	else if ( this->m_eState == CZeppelin::FALLING )
	{
		sprintf( this->m_pStateDescription, "Dead");

		this->AIFalling();

		// if we dont do this, the tracers will stop moving..
		// well keep ruining **** until were really dead!
		for ( int i = 0; i < this->m_iNumTurrets; i++ )
		{
			this->m_pTurrets[i]->PostMessage( MSGID_UPDATE, &this->m_fTimeDelta );
		}
	}
	else if ( this->m_eState == CZeppelin::DEAD )
	{
	}
}


void CZeppelin::AIFalling()
{
	// get our facing vector
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
	facingvec.Normalise();

	// if our Y component is above ~45 degrees.. then roll downward
	if ( facingvec.v.y > -0.6 )
	{
		float pitchamount = 0.02f * this->m_fTimeDelta;
		this->m_pNode->RotateX( -pitchamount );
	}

	float fvelo = this->s_fSpeed * this->m_fTimeDelta;
	this->m_pNode->TranslateForward( fvelo );
}


void CZeppelin::NotifyOfShipDeath( CTask * ship )
{
	// update our turrets
	for ( int i = 0; i < this->m_iNumTurrets; i++ )
	{
		this->m_pTurrets[i]->PostMessage( MSGID_NOTIFY_OF_DEATH, ship );
	}
}



void CZeppelin::NotifyOfCityDeath( int arraypos )
{
	if ( this->m_pTarget != NULL )
	{
		// zeppelins only head toward cities,
		// so we can safely assume that if our array pos matches, its the one
		int targarraypos = *(int*)this->m_pTarget->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );

		if ( targarraypos == arraypos )
		{
			this->m_pTarget = NULL;
		}
	}

	// update our turrets
	for ( int i = 0; i < this->m_iNumTurrets; i++ )
	{
		this->m_pTurrets[i]->PostMessage( MSGID_NOTIFY_CITYDEATH, &arraypos );
	}

}




void CZeppelin::AIGoToTarget()
{
	float fvelo = this->s_fSpeed * this->m_fTimeDelta;

	int targpos = 0;

//	NSVector3df velo = this->m_kVelocity;
//	velo.SetLength( fvelo );

	// work out if the target is to our left or right

	// get our translations
	NSVector3df trans = *this->m_pNode->GetWorldTranslation();
	NSVector3df targtrans = *( (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	// create a plane along our vertical axis facing left

	NSVector3df leftvec = this->m_pNode->GetCrossVector();
	NSPlane horzplane = NSPlane( trans, leftvec );

	float horzvalue = IntersectPointPlane( targtrans, horzplane );

	if ( horzvalue < -0.0f )
		targpos = targpos | CZeppelin::RIGHT;
	else if ( horzvalue > +0.0f )
		targpos = targpos | CZeppelin::LEFT;


	// turn to face the target
	float turnamount = 0.1f * this->m_fTimeDelta;

	if ( targpos & CZeppelin::LEFT )
	{
		this->m_pNode->RotateY( -turnamount );
	}
	else if ( targpos & CZeppelin::RIGHT )
	{
		this->m_pNode->RotateY( +turnamount );
	}


	this->m_pNode->TranslateForward( fvelo );
}


void CZeppelin::ApplyDamage( int amount )
{
	// if we havent died previously
	if ( this->m_bAlive )
	{
		// damage our ship
		this->m_iHealth -= amount;

		// create a shrapnel particle system
		// create a random spot to spawn the shrapnel
		NSVector3df shraptrans = this->m_kVelocity;
		shraptrans.Scale( ( (float)rand() / (float)RAND_MAX ) * 2.0f );
		if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f )
			shraptrans.Invert();

		shraptrans += *this->m_pNode->GetWorldTranslation();

		CParticleManager::NewSystemData data;
		data.trans = shraptrans;
		data.velocity = this->m_kVelocity;	// not used
		data.type = CParticleManager::PLANE_SHRAPNEL;
		data.radius = 1.0f;					// not used
//		data.smoke = false;
//		data.meshtype = CMeshManager::PLANE_SHRAPNEL_1;

		CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );



		// if weve lost all of our health
		if ( this->m_iHealth <= this->s_iMinHealth )
		{
			// were not alive anymore
			this->m_bAlive = false;

			// stop our turrets from shooting
			for ( int i = 0; i < this->m_iNumTurrets; i++ )
			{
				this->m_pTurrets[i]->PostMessage( MSGID_DISABLE, NULL );
			}

			// invalidate ourselves
			CShipController::Get()->PostMessage(MSGID_NOTIFY_OF_ZEPPELINDEATH, this);

			this->m_eState = CZeppelin::FALLING;

			// create an explosion
			CParticleManager::NewSystemData data;
			data.trans = *this->m_pNode->GetWorldTranslation();
			data.velocity = this->m_kVelocity;
			data.type = CParticleManager::CITY_EXPLOSION;
			data.radius = 1.0f;
		}
	}
}




void CZeppelin::SearchForTarget()
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

	this->m_pTarget = targets[ bestarraypos ];
}



