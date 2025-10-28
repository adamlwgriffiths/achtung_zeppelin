#include "Ship.h"

int		CShip::m_iNumShips					= 0;

// put here because of the absolutely stupid way ive laid out my #include's
// 3 min compiles for a #define change isnt fun!

// dont uncomment this as i have changed the values for the simplified version without
// tweaking the more complex and _less_fun_ version
//#define SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT

float	CShip::s_fRollScale					=  0.000080f;//0.000060f;//0.000045f;
float	CShip::s_fYawScale					=  0.000030f;//0.000020f;//0.000008f;
float	CShip::s_fPitchScale				=  0.000050f;//0.000030f;//0.000019f;//16f;

float	CShip::s_fEngineIncreaseScale		= 30.0f;
float	CShip::s_fEngineDecreaseScale		= 25.0f;

float	CShip::s_fThrustScale				=  0.006f;//0.016f//0.0050f;//0.006f;
float	CShip::s_fDragScale					=  0.002f;//0.003f//0.001f//0.2f;
float	CShip::s_fDragAOAScale				=  1.6f;

float	CShip::s_fGravPull					=  0.6f;//0.2f;
float	CShip::s_fGravRotation				=  0.12f;//0.1f;
float	CShip::s_fGravMinLift				=  1.0f;
//float	CShip::s_fLiftPush					=  0.00716f;//0.4f;
float	CShip::s_fLiftPush					=  0.1625f;//0.4f;
float	CShip::s_fLiftRotation				=  0.2f;//0.1f;

float	CShip::s_fVelocityScale				=  3.0f;

const float	CShip::s_fTakeOffTime			=  2.0f;

const float	CShip::s_fMaxDeadTime			=  3.0f;

//const float	CShip::s_fPropellerScale		=  50.0f;


//////////////////////////
//         AI

const float	CShipAI::s_fAIGroundCheckSpeedMultiplier		= 3.0f;	// number of seconds
float		CShipAI::s_fAIGroundCheckMinimumSafeHeight		= 100.0f;	// estimate, will be set later
const float CShipAI::s_fAIFireGunsMinDot					= 0.87f;
const float CShipAI::s_fAIFireGunsMinDistance				= 300.0f;

const float CShipAI::s_fAIFireGunsMinTrackingDistance		= 400.0f;
float		CShipAI::s_fAITracerSpeed						= 400.0f;	// is updated during init anyway

const float	CShipAI::s_fAITargetHeightAllowance				= 150.0f;

const float	CShipAI::s_fAITargetSearchTime					= 3.0f;

const float	CShipAI::s_fAIEvadeDistance						= 80.0f;


const float CShipAI::s_fMaxBehindTime		= 10.0f;
const float	CShipAI::s_fAIGroundCheckBuffer	= 15.0f;

const float	CShipAI::s_fAIDotDistanceAllowance				= 300.0f;
const float	CShipAI::s_fAILargeDotDeviationAllowance		= 0.999f;
const float	CShipAI::s_fAISmallDotDeviationAllowance		= 0.9999f;



void * CShip::PostMessage(int ID, void * Data)
{
	void * result = 0;
	bool bResult;

	switch(ID)
	{
		case MSGID_INIT:
			result = (void*)this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			break;


		case MSGID_IS_ALIVE:
			bResult = this->m_bAlive;
			result = &bResult;
			break;


		case MSGID_GET_NODE:
			result = this->m_pNode;
			break;

		case MSGID_GET_BOUNDINGRADIUS:
			result = &this->m_fRadius;
			break;

		case MSGID_GET_VELOCITY:
			result = &this->m_kVelocity;
			break;

		case MSGID_APPLY_DAMAGE:
#ifdef SHIP_DAMAGEABLE
			this->ApplyDamage( *(int*)Data );
#endif // #ifdef SHIP_DAMAGEABLE
			break;

		case MSGID_GET_MASS:
			result = &this->m_fMass;
			break;

		case MSGID_SET_COLLIDING:
			this->m_bColliding = true;
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

		case MSGID_GET_SHIPTYPE:
			result = &this->m_eShipType;
			break;

		case MSGID_GET_THROTTLE:
			result = &this->m_fThrottle;
			break;

		case MSGID_IS_FIRING:
			result = &this->m_bIsFiring;
			break;

		case MSGID_GET_STATE_DESCRIPTION:
			result = this->m_pStateDescription;
			break;

		case MSGID_GET_HEALTH:
			result = &this->m_iHealth;
			break;


		default:
			assert(0);		// should never get here!
			break;
	}

	return result;
}


CShip::CShip()
{
}

CShip::~CShip()
{
}




NSNode * CShip::Init( void * Data )
{
	// start in takeoff mode
	this->m_eState = CShip::TAKEOFF;

	///////////////////////
	// hack
//	this->m_eState = CShip::FLYING;
	///////////////////////

	CShip::InitData * data = (CShip::InitData*)Data;

	// get our info from the init data
	NSVector3df trans = data->trans;
	NSVector3df up = data->up;
	this->m_pShipController = data->shipcontroller;

	// create a new node
	char buff[20];
	sprintf(buff, "ship%i", this->m_iNumShips);

	this->m_pNode = new NSNode(buff);
	this->m_pNode->SetTranslate( trans );
//	this->m_pNode->SetUpAxis( up );
	this->m_pNode->SetMatrix( data->mat );

	sprintf(buff, "bulletbranch%i", this->m_iNumShips);
	this->m_pBulletNode = new NSNode(buff);

	
	// setup our default variables
	this->m_eAlliance = data->alliance;

	this->m_bAlive = true;
	this->m_bColliding = false;
	this->m_fPitchInput = 0.0f;
	this->m_fYawInput = 0.0f;
	this->m_fRollInput = 0.0f;

	this->m_fDeathTimer = 0.0f;
	this->m_pSmokeSystem = NULL;

//	this->m_pParticleSystem = NULL;

	this->m_fThrottle = 100.0f;//0.0f;
	this->m_fEngineSpeed = 50.0f;//0.0f;

	this->m_fGunTimer = 0.0f;
	this->m_iGunAmmo = SHIP_GUNS_AMMOMAX;
	this->m_iNumTracers = 0;
	this->m_bGunLeftBarrel = true;
	this->m_fTakeOffTimer = 0.0f;
	this->m_bIsFiring = false;

	this->m_eShipType = PLANE;

	this->m_bFlatWorld = *(bool*)CWorldManager::Get()->PostMessage(MSGID_IS_FLATWORLD, NULL);

	this->m_kVelocity = this->m_pNode->GetFacingVector();//NSVector3df( 0.0f, 0.0f, 0.0f );
	// start at the same speed as the zeppelin, so we dont slide back
	this->m_kVelocity.SetLength( 10.0f );//50.0f );


	// create our propeller node
	sprintf(buff, "propeller%i", this->m_iNumShips);
	NSVector3df propvec = this->m_pNode->GetFacingVector();
	propvec.SetLength( 1.9f );
	propvec.v.y -= 0.5f;
	this->m_pPropellerNode = new NSNode(buff, propvec );

	CMeshManager::MESHES meshtype;
	meshtype = CMeshManager::SHIP_PROPELLER;
	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);
	this->m_pPropellerNode->AttachMesh( mesh );

	this->m_pNode->AttachChild( this->m_pPropellerNode );


	this->m_iNumShips++;

	//this->m_pStateDescription = new char[10];
	sprintf( this->m_pStateDescription, "Idle");


	// return our ship node
	return this->m_pNode;
}


void CShip::Destroy()
{
	this->m_iNumShips--;

	for ( int i = 0; i < this->m_iNumTracers; i++ )
	{
		this->m_pTracers[i]->PostMessage(MSGID_DESTROY, NULL);
		delete this->m_pTracers[i];
	}
//	delete [] this->m_pTracers;

	this->m_iNumTracers = 0;

	delete this->m_pBulletNode;
	this->m_pBulletNode = NULL;

	delete this->m_pNode;
	this->m_pNode = NULL;
	this->m_pPropellerNode = NULL;

	if ( this->m_pSmokeSystem )
	{
		((PlaneSmoke*)this->m_pSmokeSystem)->Stop();
		this->m_pSmokeSystem = NULL;
	}

//	if ( this->m_pStateDescription )
//		delete [] this->m_pStateDescription;

//	this->m_pShipController->PostMessage( MSGID_NOTIFY_OF_DEATH, this );
}


void CShip::Update( void * Data )
{
	this->m_fTimeDelta = *(float*)Data;

	this->m_bIsFiring = false;

	// update our propeller
	this->m_pPropellerNode->RotateZ( (this->m_fThrottle + 1.0f) / 100.0f  );

	// Our VEEERRRYYY basic state machine
	switch ( this->m_eState )
	{
		case CShip::TAKEOFF:
			this->UpdateTakeOff();
			this->UpdateEngine();
			this->m_pNode->Update();
			// allow our sub classes to do misc stuff
			this->PostUpdate();
			return;

			break;

		case CShip::FLYING:
			this->UpdateFlying();
			break;

		case CShip::EXPLODING:
			this->UpdateExploding();
			break;

		case CShip::FALLINGAPART:
			this->UpdateFallingApart();
			break;

		case CShip::DEAD:
			// doing this will destroy our camera node if we have one!!
			//this->PostMessage( MSGID_DESTROY, NULL );
			this->m_pShipController->PostMessage( MSGID_REQUEST_DELETION, this );
			return;	// dont continue
			break;

		default:
			assert(0);

	};

	///////////////////////////
	// Upkeep

	// Apply our physics
	this->ApplyPhysics();

	// update our engine speed
	this->UpdateEngine();

	// Update our tracers
	this->UpdateTracers();

	this->m_pNode->Update();

	// allow our sub classes to do misc stuff
	this->PostUpdate();

	// we dont check for collisions in our own update loop since we will repeat many checks
	// instead we do it outside in the ship controller, that way we dont check
	// n^n times, instead, we only check n! times

	// unset our colliding variable
	this->m_bColliding = false;
}


////////////////////////////////////
// Our VERY basic State specific updates

void CShip::UpdateTakeOff()
{
	if ( this->m_fTakeOffTimer > this->s_fTakeOffTime )
	{
		this->m_eState = CShip::FLYING;
		return;
	}

	this->m_fTakeOffTimer += this->m_fTimeDelta;

	// Apply some basic physics, minus the gravity, until we get airborne

	float velo = this->m_kVelocity.GetLength();
	float velotd = velo * this->m_fTimeDelta;

	NSVector3df facingvec = this->m_pNode->GetFacingVector();

	this->m_kVelocity = facingvec;
	this->m_kVelocity.SetLength( velo );



	///////////////////////////////////
	// Thrust
	NSVector3df thrustvec = facingvec;
	float fthrust = this->m_fEngineSpeed * this->s_fThrustScale;// * this->m_fTimeDelta;
	float fthrusttd = fthrust * this->m_fTimeDelta;

	thrustvec.SetLength( fthrusttd );


	///////////////////////////////////
	// Drag
	// air friction = v / v^2 / v^3 * scale (increases with speed)
	// invert the velocity
	NSVector3df dragvec = this->m_kVelocity;
	dragvec.Invert();
	float fdrag = pow( velo, 1.2f ) * this->s_fDragScale;//pow( velo, 2.0f ) * this->s_fDragScale;

	float fdragtd = fdrag * this->m_fTimeDelta;
	dragvec.SetLength( fdragtd );


	///////////////////////////////////
	// Add our components

	NSVector3df velovectd = this->m_kVelocity * this->m_fTimeDelta;

	velovectd += thrustvec;
	velovectd += dragvec;

	this->m_kVelocity = velovectd / this->m_fTimeDelta;


	// velocity is in world co-ordinates so this is ok
	this->m_pNode->SetTranslate(  (*this->m_pNode->GetTranslation() + velovectd )  );
}

void CShip::UpdateFlying()
{
	// reduce our gun timer
	if ( this->m_fGunTimer > 0.0f )
		this->m_fGunTimer -= this->m_fTimeDelta;

	// this check isnt really necessary since its handled by our state machine
//	if (this->m_bAlive)
	{
		// update our Player Input / AI
		this->UpdateInput();
	}

	// update the ships translation
	this->ApplyInput();
}

void CShip::UpdateExploding()
{
	this->m_fDeathTimer += this->m_fTimeDelta;

	if ( this->m_fDeathTimer >= s_fMaxDeadTime )
	{
		this->m_eState = CShip::DEAD;
		this->m_fDeathTimer = 0.0f;
	}

	// check if our particle system has run out

	// our particle system may not have been created
//	if ( !this->m_pParticleSystem )
//		this->m_eState = CShip::DEAD;

//	if ( this->m_pParticleSystem->IsFinished() == true )
//	{
		// it will be deleted by the particle manager
//		this->m_pParticleSystem = NULL;

		// if it has, set the state to DEAD
//		this->m_eState = CShip::DEAD;
//	}
}

void CShip::UpdateFallingApart()
{
	// roll to the left
	this->RollLeft( 75.0f );

	// update the ships translation
	// we call this to apply the roll
	this->ApplyInput();

	// update our particle system

	// check if our ship has hit the ground
	if ( this->m_bColliding )
	{
		// if it has, set the state to DEAD
		this->Explode();
		this->m_eState = CShip::EXPLODING;
	}
}



////////////////////////////////////





void CShip::UpdateTracers()
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



void CShip::FireGuns()
{
#ifdef SHIP_LIMITEDAMMO
	// if were not out of ammo
	if ( this->m_iGunAmmo > 0 )
	{
#endif // #ifndef SHIP_LIMITEDAMMO

		// if its time to shoot
		if ( this->m_fGunTimer <= 0.0f )
		{

			// if were not at our max number of tracers
			if ( this->m_iNumTracers < SHIP_GUNS_MAXNUMTRACERS )
			{
				// set our timer
				this->m_fGunTimer = SHIP_GUNS_TIMEBETWEENSHOTS;

#ifdef SHIP_LIMITEDAMMO
				// lower our ammo count
				this->m_iGunAmmo--;
#endif // #ifndef SHIP_LIMITEDAMMO

				// create a bullet
				this->m_pTracers[ this->m_iNumTracers ] = new CTracer();

				// shoot out of one of the barrels
				NSVector3df pos = *this->m_pNode->GetTranslation();
				NSVector3df cross = this->m_pNode->GetCrossVector();
				cross.Normalise();
				cross.Scale( SHIP_GUNS_BARRELDISTANCE );

				if ( this->m_bGunLeftBarrel )
					cross.Invert();

				pos += cross;

				NSVector3df facing = this->m_pNode->GetFacingVector();

				// move the guns forward
				NSVector3df gunforwarddistance = facing;
				gunforwarddistance.Scale( SHIP_GUNS_FORWARDDISTANCE );
				pos += gunforwarddistance;


				CTracer::TracerSetupData tdata;
				tdata.pos = pos;
				tdata.dir = facing;
				//tdata.shipvelo = this->m_kVelocity;
				NSVector3df velo = this->m_kVelocity;
				// undo the time delta
				velo.v.x /= this->m_fTimeDelta;
				velo.v.y /= this->m_fTimeDelta;
				velo.v.z /= this->m_fTimeDelta;
				tdata.shipvelo = velo;

				tdata.alliance = this->m_eAlliance;
				tdata.owner = this;
				tdata.bulletBranch = this->m_pBulletNode;
				tdata.flatworld = this->m_bFlatWorld;

				// initialise the bullet
				this->m_pTracers[ this->m_iNumTracers ]->PostMessage( MSGID_INIT, &tdata );

				this->m_iNumTracers++;

				// switch barrels
				this->m_bGunLeftBarrel = !this->m_bGunLeftBarrel;
				

				//// play a sound
				//if ( *(bool*)this->PostMessage(MSGID_IS_PLAYERSHIP, NULL) )
				//{
					CTask *camera = (CTask*)CGameController::Get()->PostMessage( MSGID_GET_CAMERA, NULL );
					NSVector3df trans = *( (NSNode*)camera->PostMessage( MSGID_GET_NODE, NULL ) )->GetWorldTranslation();
					trans -= *this->m_pNode->GetWorldTranslation();

					CSoundController::SoundData sounddata;
					sounddata.sound = CSoundController::TRACER;
					sounddata.distance = trans.GetLength();
					sounddata.frequency = -1;
					sounddata.volume = 2;

					CSoundController::Get()->PostMessage( MSGID_PLAY_SOUND, &sounddata );
				//}
				this->m_bIsFiring = true;
			}

		}

#ifdef SHIP_LIMITEDAMMO
	}
#endif // #ifndef SHIP_LIMITEDAMMO
}


void CShip::UpdateEngine()
{
	if ( this->m_fEngineSpeed < this->m_fThrottle )
		this->m_fEngineSpeed += this->s_fEngineIncreaseScale * this->m_fTimeDelta;
	else if ( this->m_fEngineSpeed > this->m_fThrottle )
		this->m_fEngineSpeed -= this->s_fEngineDecreaseScale * this->m_fTimeDelta;
}


void CShip::ApplyInput()
{
	float velo = this->m_kVelocity.GetLength();
	float velotd = velo * this->m_fTimeDelta;
	// our input values have not had a time delta applied to them

	// apply roll
	float roll = this->m_fRollInput * velotd * this->s_fRollScale;
	this->m_pNode->RotateZ( roll );

	// apply pitch
	float pitch = this->m_fPitchInput * velotd * this->s_fPitchScale;
	this->m_pNode->RotateX( pitch );

	// apply yaw
	float yaw = this->m_fYawInput * velotd * this->s_fYawScale;
	this->m_pNode->RotateY( yaw );

	// reset our input
	this->m_fPitchInput = 0.0f;
	this->m_fRollInput = 0.0f;
	this->m_fYawInput = 0.0f;
}


void CShip::ApplyPhysics()
{
	float velo = this->m_kVelocity.GetLength();
	float velotd = velo * this->m_fTimeDelta;

	NSVector3df facingvec = this->m_pNode->GetFacingVector();
#ifndef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT
	// hack the velocity to be in the same direction as the plane
	this->m_kVelocity = facingvec;
	this->m_kVelocity.SetLength( velo );
#endif // #ifndef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT
	NSVector3df velonorm = this->m_kVelocity;
	velonorm.Normalise();

	float aoa = facingvec.Dot( velonorm );
	// 0.0f aoa is straight ahead, 2.0f is behind
	if ( aoa >= 0.0f )
		aoa = 1.0f - aoa;
	else if ( aoa < 0.0f )
		aoa = 1.0f + abs( aoa );

	///////////////////////////////////
	// Thrust
	NSVector3df thrustvec = facingvec;
	float fthrust = this->m_fEngineSpeed * this->s_fThrustScale;// * this->m_fTimeDelta;
	float fthrusttd = fthrust * this->m_fTimeDelta;

	thrustvec.SetLength( fthrusttd );


	///////////////////////////////////
	// Drag
	// air friction = v / v^2 / v^3 * scale (increases with speed)
	// invert the velocity
	NSVector3df dragvec = this->m_kVelocity;
	dragvec.Invert();
	float fdrag = pow( velo, 1.2f ) * this->s_fDragScale;//pow( velo, 2.0f ) * this->s_fDragScale;
	// drag increases with angle of attack
	fdrag *= pow( aoa + 1.0f, this->s_fDragAOAScale );

	float fdragtd = fdrag * this->m_fTimeDelta;
	dragvec.SetLength( fdragtd );

	///////////////////////////////////
	// Lift
	//float flift = pow( velo, 1.2f ) * this->s_fLiftPush;//pow( velo, 2.0f ) * this->s_fLiftPush;
	//float flift = log( velo * this->s_fLiftPush );// * this->s_fLiftPush;
	// from graphing function, dont expect to understand it
	float flift = (-0.00106596f + 0.226913f * velo) / ( 1.0f + 0.0507326f * velo + 0.0000005117f * pow(velo, 2.0f) );
	flift *= this->s_fLiftPush;

	// scale lift by the +ve dot of the facing vector and the velocity (AOA)
	float fliftdot = facingvec.Dot( velonorm );
	fliftdot += 0.2f;	// lift increases at a slight AOA, this is a rough allowance for that

	if ( fliftdot > 1.0f )
		fliftdot = 1.0f;
	else if ( fliftdot < 0.0f )
		fliftdot = 0.0f;

	flift *= fliftdot;

	float flifttd = flift * this->m_fTimeDelta;

	NSVector3df liftvec = *this->m_pNode->GetUpAxis();
	liftvec.SetLength( flifttd );



	///////////////////////////////////
	// Gravity
	NSVector3df gravityvec;
	float gravpull = this->s_fGravPull;
	float gravpulltd = gravpull * this->m_fTimeDelta;
	// help prevent the object from sinking into the ground
	if ( !this->m_bColliding )
	{
		if (this->m_bFlatWorld)
		{
			// Flat
			gravityvec = NSVector3df( 0.0f, -gravpulltd, 0.0f );
		}
		else
		{
			// Spherical
			gravityvec = *this->m_pNode->GetTranslation();
			gravityvec.SetLength( -gravpulltd );
		}
	}

#ifndef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT
	// screw with our gravity vector
	
	if ( flift < this->s_fGravMinLift )
	{
		// min = 0.0f;
		// max = this->s_fGravMinLift
		float scale = this->s_fGravMinLift / 100.0f;
		float hackgravpull = gravpull * scale;
		// make it so that the strength is inverted (stronger at lower lift values)
		hackgravpull = this->s_fGravPull - hackgravpull;
		float hackgravpulltd = hackgravpull * this->m_fTimeDelta;
		gravityvec.SetLength( hackgravpulltd );
	}

#endif // #ifndef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT

	///////////////////////////////////
	// Add our components

	NSVector3df velovectd = this->m_kVelocity * this->m_fTimeDelta;

	if ( !this->m_bColliding )
	{
		velovectd += thrustvec;
		velovectd += dragvec;
		velovectd += gravityvec;
#ifdef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT
		velovectd += liftvec;
#endif // #ifdef SHIP_PHYSICS_ENABLE_GRAVITYANDLIFT
	}

	this->m_kVelocity = velovectd / this->m_fTimeDelta;


	// velocity is in world co-ordinates so this is ok
	this->m_pNode->SetTranslate(  (*this->m_pNode->GetTranslation() + velovectd )  );



	///////////////////////////////////
	// Dip the aircraft's nose with gravity

	// get the cross of the ships facing and Y = 1
	NSVector3df upvec;

	if (this->m_bFlatWorld)
	{
		// Flat
		upvec = NSVector3df( 0.0f, 1.0f, 0.0f );
	}
	else
	{
		// Spherical
		upvec = *this->m_pNode->GetTranslation();
	}

	NSVector3df facing = this->m_pNode->GetFacingVector();
	NSVector3df gravrotationvec = facing.CrossProduct( upvec );

	if ( ! this->m_bColliding )
	{
		float gravrot = this->s_fGravRotation;

		// if were below 100kph, start increasing the nose dipping
		float fspeed = this->m_kVelocity.GetLength();
//		if ( fspeed < 100.0f )
		{
			float gravrotscale = 100.0f - fspeed;
			gravrot *= (float)pow(2.0f, (0.04f * gravrotscale) );
		}

		float gravrottd = gravrot * this->m_fTimeDelta;
		NSMatrix4 gravmat;
		gravmat.SetIdentity();
		gravmat.SetRotationAboutAxis( gravrotationvec, -gravrottd );

		NSMatrix4 * mat = this->m_pNode->GetMatrix();

		// tilt to orientation
		*mat = *mat * gravmat;
	}


	///////////////////////////////////
	// Lift the aircraft's nose with lift
	// flift already has a timedelta applied to it
	this->m_pNode->RotateX( flifttd * this->s_fLiftRotation );
}


void CShip::ApplyDamage( int amount )
{
	CTask *camera;
	NSVector3df trans;
	CSoundController::SoundData sounddata;

	// if we havent died previously
//	if ( this->m_bAlive )
	{
		// damage our ship
		this->m_iHealth -= amount;

		if ( this->m_bAlive )
		{
			// create a shrapnel particle system
			CParticleManager::NewSystemData data;
			data.trans = *this->m_pNode->GetTranslation();
			data.type = CParticleManager::PLANE_SHRAPNEL;
			data.velocity = this->m_kVelocity;	// not used
			data.radius = 1.0f;					// not used

			CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );
		}


		// play a sound
//		if ( *(bool*)this->PostMessage(MSGID_IS_PLAYERSHIP, NULL) )
		{
			camera = (CTask*)CGameController::Get()->PostMessage( MSGID_GET_CAMERA, NULL );
			trans = *( (NSNode*)camera->PostMessage( MSGID_GET_NODE, NULL ) )->GetWorldTranslation();
			trans -= *this->m_pNode->GetWorldTranslation();

			sounddata.sound = CSoundController::SHIP_HIT;
			sounddata.distance = trans.GetLength();//0.0f;
			sounddata.frequency = -1;
			sounddata.volume = 5;

			CSoundController::Get()->PostMessage( MSGID_PLAY_SOUND, &sounddata );
		}


		// if weve lost all of our health
		if ( this->m_iHealth <= SHIP_HEALTH_MIN )
		{
			// were not alive anymore
			// set this later so we can explode if we hit zeppelins
//			this->m_bAlive = false;
			this->m_fThrottle = 0.0f;		// kill our throttle

			// tell people to stop targetting us
			this->m_pShipController->PostMessage( MSGID_NOTIFY_OF_DEATH, this );

			if ( this->m_iHealth <= SHIP_CRITICAL_DEATH)
			{
				if ( this->m_eState != CShip::EXPLODING  &&  this->m_eState != CShip::DEAD )
				{
					// big ship go Boom!
					this->Explode();
					this->m_eState = CShip::EXPLODING;
				}
			}
			else
			{
				// only `fall apart' once
				if ( this->m_bAlive )
				{
					this->FallApart();
					this->m_eState = CShip::FALLINGAPART;
				}
			}

			this->m_bAlive = false;

			// play a sound
//			if ( *(bool*)this->PostMessage(MSGID_IS_PLAYERSHIP, NULL) )
			{
				camera = (CTask*)CGameController::Get()->PostMessage( MSGID_GET_CAMERA, NULL );
				trans = *( (NSNode*)camera->PostMessage( MSGID_GET_NODE, NULL ) )->GetWorldTranslation();
				trans -= *this->m_pNode->GetWorldTranslation();

				sounddata.sound = CSoundController::SHIP_EXPLOSION;
				sounddata.distance = trans.GetLength();//0.0f;
				sounddata.frequency = -1;
				sounddata.volume = 5;

				CSoundController::Get()->PostMessage( MSGID_PLAY_SOUND, &sounddata );

//				CSoundController::Get()->PostMessage( MSGID_DISABLE_PLAYERENGINE, NULL );
			}

		}
	}
}

void CShip::Explode()
{
	// create an explosion particle system
	NSNode * parent = this->m_pNode->GetParent();

	CParticleManager::NewSystemData data;
	data.trans = *this->m_pNode->GetTranslation();
	data.velocity = this->m_kVelocity;
	data.type = CParticleManager::CITY_EXPLOSION;
	data.radius = 1.0f;

	this->m_eState = CShip::EXPLODING;

//	if ( this->m_pParticleSystem != NULL )
//		delete this->m_pParticleSystem;

//	this->m_pParticleSystem = (CityExplosionPSystem*)CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );
	(CityExplosionPSystem*)CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );

	// remove this node
	parent->DetachChild( this->m_pNode );

}

void CShip::FallApart()
{
	// create a smoke particle system
	CParticleManager::NewSystemData data;
	data.trans = *this->m_pNode->GetWorldTranslation();	// not used
	data.velocity = this->m_kVelocity;	// not used
	data.type = CParticleManager::PLANE_SMOKE;
	data.radius = 1.0f;					// not used
//	data.smoke = true;
	data.parent = this->m_pNode;

	this->m_pSmokeSystem = (PSystem*)CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );
}



void CShip::Accelerate( float amount )
{
	//NSVector3df accel = this->m_pNode->GetFacingVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity += accel;

	this->m_fThrottle += amount * SHIP_INPUT_ACCELSCALE;
	if ( this->m_fThrottle > SHIP_THROTTLE_MAXIMUM )
		this->m_fThrottle = SHIP_THROTTLE_MAXIMUM;
}

void CShip::Decelerate( float amount )
{
	//NSVector3df accel = this->m_pNode->GetFacingVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity -= accel;

	this->m_fThrottle -= amount * SHIP_INPUT_DECELSCALE;
	if ( this->m_fThrottle < SHIP_THROTTLE_MINIMUM )
		this->m_fThrottle = SHIP_THROTTLE_MINIMUM;
}


void CShip::StrafeLeft( float amount )
{
	//NSVector3df accel = this->m_pNode->GetCrossVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity += accel;		// cross is facing the left.. oops
}

void CShip::StrafeRight( float amount )
{
	//NSVector3df accel = this->m_pNode->GetCrossVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity -= accel;
}

void CShip::StrafeUp( float amount )
{
	//NSVector3df accel = this->m_pNode->GetUpVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity += accel;
}

void CShip::StrafeDown( float amount )
{
	//NSVector3df accel = this->m_pNode->GetUpVector();
	//accel.SetLength( amount * this->m_fTimeDelta );

	//this->m_kVelocity -= accel;
}


void CShip::TurnLeft( float amount )
{
//	this->m_pNode->RotateY( -amount * this->m_fTimeDelta );
	this->m_fYawInput = -amount;
}

void CShip::TurnRight( float amount )
{
//	this->m_pNode->RotateY( amount * this->m_fTimeDelta );
	this->m_fYawInput = amount;
}

void CShip::TurnUp( float amount )
{
//	this->m_pNode->RotateX( -amount * this->m_fTimeDelta );
	this->m_fPitchInput = amount;
}

void CShip::TurnDown( float amount )
{
//	this->m_pNode->RotateX( amount * this->m_fTimeDelta );
	this->m_fPitchInput = -amount;
}

void CShip::RollLeft( float amount )
{
//	this->m_pNode->RotateZ( -amount * this->m_fTimeDelta );
	this->m_fRollInput = -amount;
}

void CShip::RollRight( float amount )
{
//	this->m_pNode->RotateZ( amount * this->m_fTimeDelta );
	this->m_fRollInput = amount;
}










///////////////////////////////////
//
//         PLAYER SHIP
//
///////////////////////////////////

// this is here to stop circular dependency hell from occuring and causing my brain to explode
// i hate single pass compilers!


void * CShipPlayer::PostMessage(int ID, void * Data)
{
	void * result = 0;
	bool bResult;

	switch(ID)
	{
		case MSGID_INIT:
			result = (void*)this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_IS_PLAYERSHIP:
			bResult = true;
			result = &bResult;
			break;

		default:
			result = CShip::PostMessage(ID, Data);		// if we dont understand it, try our parent
			break;
	}

	return result;
}


CShipPlayer::CShipPlayer()
{
}

CShipPlayer::~CShipPlayer()
{
}



NSNode * CShipPlayer::Init( void * Data )
{
	// call our parent Init
	CShip::Init(Data);

	CShip::InitData data = *(CShip::InitData*)Data;

	this->m_iHealth = SHIP_HEALTH_MAX;

	this->m_fRadius = SHIP_BOUNDINGRADIUS_FRIENDLY;
	this->m_fMass = SHIP_MASS_FRIENDLY;

	// create and attach a mesh
	CMeshManager::MESHES meshtype = CMeshManager::SHIP_FRIEND;
	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pNode->AttachMesh( mesh );

	// show our hud
//	CHUD::Get()->PostMessage( MSGID_SHOW_PLAYERSTATS, NULL );

	// enable our engine noise
//	CSoundController::Get()->PostMessage( MSGID_ENABLE_PLAYERENGINE, NULL );

	// Set our status description
//	if ( this->m_pStateDescription )
//		delete [] this->m_pStateDescription;
//	this->m_pStateDescription = new char[15];
	sprintf( this->m_pStateDescription, "Player Control");

	return this->m_pNode;
}


void CShipPlayer::Explode()
{
	// hide our hud
//	CHUD::Get()->PostMessage( MSGID_HIDE_PLAYERSTATS, NULL );

	CShip::Explode();
}

void CShipPlayer::FallApart()
{
	// hide our hud
//	CHUD::Get()->PostMessage( MSGID_HIDE_PLAYERSTATS, NULL );

	CShip::FallApart();
}


void CShipPlayer::UpdateInput()
{
	// update our shields orbit
//	m_pShieldNode->RotateZ( SHIP_SHIELD_ORBITSPEED * this->m_fTimeDelta );

	// Handle player input
	CInputController::Keys keys = *(CInputController::Keys*)CInputController::Get()->PostMessage(MSGID_GET_KEYS, NULL);

	// acceleration
	if (keys.m_bShip_Accel != 0.0f)									// accelerate
		this->Accelerate( keys.m_bShip_Accel );
	if (keys.m_bShip_Decel != 0.0f)									// decelerate
		this->Decelerate( keys.m_bShip_Decel );

	//// strafing
	//if (keys.m_bShip_StrafeLeft)							// strafe left
	//	this->StrafeLeft( SHIP_INPUT_STRAFESPEED );
	//if (keys.m_bShip_StrafeRight)							// strafe right
	//	this->StrafeRight( SHIP_INPUT_STRAFESPEED );
	//if (keys.m_bShip_StrafeUp)								// strafe up
	//	this->StrafeUp( SHIP_INPUT_STRAFESPEED );
	//if (keys.m_bShip_StrafeDown)							// strafe down
	//	this->StrafeDown( SHIP_INPUT_STRAFESPEED );

	// turning
	if (keys.m_bShip_TurnLeft != 0.0f)								// turn left
		this->TurnLeft( keys.m_bShip_TurnLeft );
	if (keys.m_bShip_TurnRight != 0.0f)								// turn right
		this->TurnRight( keys.m_bShip_TurnRight );

	if (keys.m_bShip_TurnUp != 0.0f)								// turn up
		this->TurnUp( keys.m_bShip_TurnUp );
	if (keys.m_bShip_TurnDown != 0.0f)								// turn down
		this->TurnDown( keys.m_bShip_TurnDown );

	if (keys.m_bShip_RollLeft != 0.0f)								// roll left
		this->RollLeft( keys.m_bShip_RollLeft );
	if (keys.m_bShip_RollRight != 0.0f)								// roll right
		this->RollRight( keys.m_bShip_RollRight );

	if (keys.m_bShip_Fire1)									// fire main guns
		this->FireGuns();
}

void CShipPlayer::PostUpdate()
{
	//// update our hud
	//CHUD::PlayerData huddata;
	//huddata.health = this->m_iHealth;

	//huddata.throttle = this->m_fThrottle;

	//NSVector3df trans = *this->m_pNode->GetWorldTranslation();
	//huddata.posx = trans.v.x;
	//huddata.posy = trans.v.y;
	//huddata.posz = trans.v.z;

	//huddata.speed = this->m_kVelocity.GetLength();


	//CHUD::Get()->PostMessage( MSGID_UPDATE_PLAYERSTATS, &huddata );
}






///////////////////////////////////
//
//           AI SHIP
//
///////////////////////////////////

// this is here to stop circular dependency hell from occuring and causing my brain to explode
// i hate single pass compilers!


void * CShipAI::PostMessage(int ID, void * Data)
{
	void * result = 0;
	bool bResult;

	switch (ID)
	{
		case MSGID_INIT:
			result = (void*)this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_NOTIFY_OF_DEATH:
			this->NotifyOfDeath( (CTask*)Data );
			break;

		case MSGID_SET_TARGET:
			this->m_pTarget = (CTask*)Data;
			break;

		case MSGID_GET_TARGET:
			result = this->m_pTarget;
			break;

		case MSGID_IS_PLAYERSHIP:
			bResult = false;
			result = &bResult;
			break;

		case MSGID_NOTIFY_CITYDEATH:
			// ships dont currently target cities
			break;


		default:
			result = CShip::PostMessage(ID, Data);		// if we dont understand it, try our parent
			break;
	}

	return result;
}

CShipAI::CShipAI()
{
}

CShipAI::~CShipAI()
{
}

NSNode * CShipAI::Init( void * Data )
{
	// call our parent Init
	CShip::Init(Data);

	this->m_iHealth = SHIP_HEALTH_MAX;
	this->m_fThrottle = 100.0f;//80.0f;			// full speed ahead!
//	this->m_fEngineSpeed = 0.0f;
	this->m_pTarget = NULL;
	this->m_fTargetSearchTimer = 0.0f;

	this->m_fTurningDirectionSelected = false;
	this->m_eTurningDirection = CShipAI::TURN_LEFT;
	this->m_fBehindTimer = 0.0f;

	this->m_bAvoidingGround = false;

	this->m_bWithinDotAllowance = false;

	// get our worlds minimum height
	CTask * world = (CTask*)CWorldManager::Get()->PostMessage( MSGID_GET_WORLD, NULL );
	this->s_fAIGroundCheckMinimumSafeHeight = *(float*)world->PostMessage(MSGID_GET_MINHEIGHT, NULL);

	this->s_fAITracerSpeed = CTracer::s_fSpeed;

	CMeshManager::MESHES meshtype;

	// create and attach a mesh
	if (this->m_eAlliance == FRIENDLY)
	{
		this->m_fRadius = SHIP_BOUNDINGRADIUS_FRIENDLY;
		this->m_fMass = SHIP_MASS_FRIENDLY;
		meshtype = CMeshManager::SHIP_FRIEND;
	}
	else
	{
		this->m_fRadius = SHIP_BOUNDINGRADIUS_ENEMY;
		this->m_fMass = SHIP_MASS_ENEMY;
		meshtype = CMeshManager::SHIP_ENEMY;
	}

	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pNode->AttachMesh( mesh );

//	if ( this->m_pStateDescription )
//		delete [] this->m_pStateDescription;
//	this->m_pStateDescription = new char[20];
	sprintf( this->m_pStateDescription, "AI Control" );

	return this->m_pNode;
}

void CShipAI::NotifyOfDeath( CTask * ship )
{
	if ( this->m_pTarget == NULL )
		return;

	// Compare our targets vs the ship were being told just died

	bool shipisplayer = *(bool*)ship->PostMessage( MSGID_IS_PLAYERSHIP, NULL );
	bool targisplayer = *(bool*)this->m_pTarget->PostMessage( MSGID_IS_PLAYERSHIP, NULL );

	int shiparraypos = *(int*)ship->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );
	int targarraypos = *(int*)this->m_pTarget->PostMessage( MSGID_GET_ARRAYPOSITION, NULL );
	SHIP_TYPE shiptype = *(SHIP_TYPE*)ship->PostMessage( MSGID_GET_SHIPTYPE, NULL );
	SHIP_TYPE targtype = *(SHIP_TYPE*)this->m_pTarget->PostMessage( MSGID_GET_SHIPTYPE, NULL );

	if ( shiparraypos == targarraypos  &&  targisplayer == shipisplayer  &&  shiptype == targtype )
		this->m_pTarget = NULL;
}



int CShipAI::AINoTarget()
{
	NSVector3df upvec = this->m_pNode->GetUpVector();
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
	NSVector3df crossvec = this->m_pNode->GetCrossVector();

	int orientation = 0;
	int	response = 0;

	// check our ground level
	if ( this->AICheckGroundLevel() )
	{
//		if ( this->m_pStateDescription )
//			delete [] this->m_pStateDescription;
//		this->m_pStateDescription = new char[30];
		sprintf( this->m_pStateDescription, "Avoding Ground" );

		// evade the ground
		response = this->AIAvoidGround();
		return response;
	}

	//////////////////////////////////
	// Get our orientation

	if ( upvec.v.y < 0.0f )
		orientation = orientation | CShipAI::INVERTED;

	// cross vector faces left
	if ( crossvec.v.y > 0.0f )
		orientation = orientation | CShipAI::ROLLED_RIGHT;
	else if ( crossvec.v.y < 0.0f )
		orientation = orientation | CShipAI::ROLLED_LEFT;

	if ( facingvec.v.y < 0.0f )
		orientation = orientation | CShipAI::PITCHED_DOWN;
	else if ( facingvec.v.y > 0.0f )
		orientation = orientation | CShipAI::PITCHED_UP;

	////////////////////////////////////////////////
	// Work out our response based on our orientation

	// un-invert, level the ship out and roll left

	// if were inverted
	if ( orientation & CShipAI::INVERTED )
	{
//		if ( this->m_pStateDescription )
//			delete [] this->m_pStateDescription;
//		this->m_pStateDescription = new char[30];
		sprintf( this->m_pStateDescription, "Un-Inverting" );

		// if were rolled roll the other way
		if ( orientation & CShipAI::ROLLED_LEFT )
			response |= CShipAI::ROLL_RIGHT;
		else if ( orientation & CShipAI::ROLLED_RIGHT)
			response |= CShipAI::ROLL_LEFT;

		// if were upside-down
		if ( orientation & CShipAI::PITCHED_DOWN )
			response |= CShipAI::PITCH_UP;
		else if ( orientation & CShipAI::PITCHED_UP )
			response |= CShipAI::PITCH_DOWN;

		// dont bother doing anything else until were un-inverted!!
	}
	else
	{
//		if ( this->m_pStateDescription )
//			delete [] this->m_pStateDescription;
//		this->m_pStateDescription = new char[30];
		sprintf( this->m_pStateDescription, "Patroling" );

		if ( orientation & CShipAI::PITCHED_DOWN )
			response |= CShipAI::PITCH_UP;
		else if ( orientation & CShipAI::PITCHED_UP)
			response |= CShipAI::PITCH_DOWN;

		// if were rolled right, roll left
		if ( orientation & CShipAI::ROLLED_RIGHT )
			response |= CShipAI::ROLL_LEFT;
		else
		{
			// if were rolled left, find out to what degree were rolled left

			// roll to within 0.45 -> 0.6 y value for our up vector
			if ( upvec.v.y < 0.55f )
				response |= CShipAI::ROLL_RIGHT;
//							response |= CShipAI::YAW_LEFT;
			else if ( upvec.v.y > 0.7f )
				response |= CShipAI::ROLL_LEFT;
//							response |= CShipAI::YAW_RIGHT;
			else	// if were rolled correctly, then start yawing
				response |= CShipAI::YAW_LEFT;
		}
	}

	////////////////////////////////
	// carry out our response
//	this->ApplyResponse( response );
	return response;
}


bool CShipAI::AICheckGroundLevel()
{
	CTask * world = (CTask*)CWorldManager::Get()->PostMessage( MSGID_GET_WORLD, NULL );
	NSVector3df trans = *this->m_pNode->GetWorldTranslation();

	// we need to check if the ship is flying at a level where mountains are
	// if so, we need to get higher!

	if ( trans.v.y <= (this->s_fAIGroundCheckMinimumSafeHeight / 2) )
		return true;

	// keep avoiding ground until were X above the min height level, to avoid stuttering
	if ( this->m_bAvoidingGround )
	{
		if ( trans.v.y <= (this->s_fAIGroundCheckMinimumSafeHeight / 2) + s_fAIGroundCheckBuffer )
			return true;
		else
			this->m_bAvoidingGround = false;
	}


	// if our velocity is +ve in height then dont worry
	if ( this->m_kVelocity.v.y >= 0.0f )
		return false;


	// get the height of the cell the player above
	// it is a reasonable estimate of the cell where the player will hit
	// there are many other complex ways of doing it, but this provides reasonable results
	// with less fuss



	CWorldFlat::WorldGetHeightAtFloat data;
	data.x = trans.v.x;
	data.z = trans.v.z;
	float height = *(float*)world->PostMessage( MSGID_GET_HEIGHTAT_FLOAT, &data );


	// we need to get the Y component of our velocity and figure out
	// if well hit the ground after X many seconds

	float heightaftertime;
	// start with our actual height
	heightaftertime = trans.v.y;

	// get our velocity's y component
	float veloy;
	veloy = this->m_kVelocity.v.y;
	// velocity should be negative

	// after X seconds well be this much lower
	veloy *= this->s_fAIGroundCheckSpeedMultiplier;

	heightaftertime -= abs( veloy );	// abs veloy just to be sure

	// if our height after X many seconds is negative, we need to do something about it!!
	if ( heightaftertime < 0.0f )
		return true;

	return false;
}


int CShipAI::AIWithTarget()
{
////////////////////////////////////////////
// AI Variables

	int orientation = 0;
	int targpos = 0;
	int	response = 0;

	bool worryabouttarget = true;


	////////////////////////////////
	// Get our orientation
	orientation = this->AIGetOrientation();


	// Get our vectors
	NSVector3df upvec = this->m_pNode->GetUpVector();
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
	NSVector3df crossvec = this->m_pNode->GetCrossVector();

	upvec.Normalise();
	facingvec.Normalise();
	crossvec.Normalise();


	// Get our target's vectors
	NSVector3df targtrans = *( (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	NSVector3df trans = *this->m_pNode->GetWorldTranslation();
	NSVector3df targvec = targtrans - trans;

	////////////////////////////////
	// Get the targets height
	float targheight = targtrans.v.y;
	float height = trans.v.y;
	float heightdelta = targheight - height;


	// Pitch toward our target
	NSVector3df losvec = facingvec;
	losvec.SetLength( targvec.GetLength() );


	////////////////////////////////
	// Find out where our target is
	// relative to our orientation

	// get our translation
	NSVector3df targaimtrans = this->AIGetFiringVector();

	// create a plane along our vertical axis facing left
	NSPlane horzplane = NSPlane( trans, crossvec );
	float horzvalue = IntersectPointPlane( targaimtrans, horzplane );

	horzvalue > 0.0f  ?  targpos |= CShipAI::LEFT  :  targpos |= CShipAI::RIGHT;

	// create a plane along our horizontal axis facing up
	NSPlane vertplane = NSPlane( trans, upvec );
	float vertvalue = IntersectPointPlane( targaimtrans, vertplane );

	vertvalue > 0.0f  ?  targpos |= CShipAI::ABOVE  :  targpos |= CShipAI::BELOW;

	// Check if the enemy is behind us, we dont want to fire guns when theyre behind us damnit!
	NSPlane frontplane = NSPlane( trans, facingvec );
	float frontvalue = IntersectPointPlane( targaimtrans, frontplane );

	frontvalue > 0.0f  ?  targpos |= CShipAI::INFRONT  :  targpos |= CShipAI::BEHIND;


	// Finished getting enemy position
	/////////////////////////////////





///////////////////////////////////////////
// Begin Logic





	// if were too close to the target, evade
	if ( targvec.GetLength() < s_fAIEvadeDistance )
	{
		sprintf( this->m_pStateDescription, "Avoiding Collision" );

		if ( targpos & CShipAI::ABOVE )
			response |= CShipAI::PITCH_DOWN;
		else
        	response |= CShipAI::PITCH_UP;

		if ( targpos & CShipAI::LEFT )
			response |= CShipAI::ROLL_RIGHT | CShipAI::YAW_RIGHT;
		else
			response |= CShipAI::ROLL_LEFT | CShipAI::YAW_LEFT;
		return response;
	}



	////////////////////////////////
	// Put the plane upright

	if ( orientation & CShipAI::INVERTED  &&  upvec.v.y < -0.7f )
	{
		sprintf( this->m_pStateDescription, "Un-Inverting" );

		worryabouttarget = false;

		// undo any roll
		if ( orientation & CShipAI::ROLLED_LEFT )
			response |= CShipAI::ROLL_RIGHT;
		else if ( orientation & CShipAI::ROLLED_RIGHT )
			response |= CShipAI::ROLL_LEFT;
	}



	// Make sure we're within X height of our target
	if ( heightdelta > this->s_fAITargetHeightAllowance )
	{
		sprintf( this->m_pStateDescription, "Climbing To Target Height" );

		// climb
		worryabouttarget = false;

		// only change our heading if were not alredy pointing down
//		if ( facingvec.v.y < 0.2 )
		{
			if ( orientation & CShipAI::INVERTED )
			{
				// if were tilted along way down, using upvec isnt enough
				if ( facingvec.v.y < 0.7f )
				{
					response |= CShipAI::PITCH_DOWN;
				}
				else
				{
					response |= CShipAI::PITCH_UP;
				}
			}
			else
			{
				// if were tilted along way down, using upvec isnt enough
				if ( facingvec.v.y > 0.7f )
				{
					response |= CShipAI::PITCH_DOWN;
				}
				else
				{
					response |= CShipAI::PITCH_UP;
				}
			}


			// level out
			if ( orientation & CShipAI::ROLLED_LEFT )
			{
				response |= CShipAI::ROLL_RIGHT;
			}
			else if ( orientation & CShipAI::ROLLED_RIGHT )
			{
				response |= CShipAI::ROLL_LEFT;
			}

			// yaw upward, if were not level, otherwise well spin in circles
			if ( orientation & CShipAI::ROLLED_LEFT  &&  upvec.v.y < 0.7f  && upvec.v.y > -0.7f )
			{
				response |= CShipAI::YAW_RIGHT;
			}
			else if ( orientation & CShipAI::ROLLED_RIGHT  &&  upvec.v.y < 0.7f  && upvec.v.y > -0.7f )
			{
				response |= CShipAI::YAW_LEFT;
			}
		}
	}
	else if ( heightdelta < -this->s_fAITargetHeightAllowance )
	{
		sprintf( this->m_pStateDescription, "Diving To Target Height" );

		// dive
		worryabouttarget = false;
		
		// only change our heading if were not alredy pointing down
//		if ( facingvec.v.y > -0.2 )
		{
			if ( orientation & CShipAI::INVERTED )
			{
				// if were tilted along way down, using upvec isnt enough
				if ( facingvec.v.y < -0.7f )
				{
					response |= CShipAI::PITCH_DOWN;
				}
				else
				{
					response |= CShipAI::PITCH_UP;
				}
			}
			else
			{
				// make sure were not diving too much
				// if were tilted along way down, using upvec isnt enough
				if ( facingvec.v.y > -0.7f )
				{
					response |= CShipAI::PITCH_DOWN;
				}
				else
				{
					response |= CShipAI::PITCH_UP;
				}
			}

			// level out
			if ( orientation & CShipAI::ROLLED_LEFT )
			{
				response |= CShipAI::ROLL_RIGHT;
			}
			else if ( orientation & CShipAI::ROLLED_RIGHT )
			{
				response |= CShipAI::ROLL_LEFT;
			}

			// yaw downward
			if ( orientation & CShipAI::ROLLED_LEFT  &&  upvec.v.y < 0.7f  && upvec.v.y > -0.7f )
			{
				response |= CShipAI::YAW_LEFT;
			}
			else if ( orientation & CShipAI::ROLLED_RIGHT  &&  upvec.v.y < 0.7f  && upvec.v.y > -0.7f )
			{
				response |= CShipAI::YAW_RIGHT;
			}
		}
	}
	else if (	heightdelta > -this->s_fAITargetHeightAllowance  &&
				heightdelta < this->s_fAITargetHeightAllowance )
	{
		// were reasonably near the target's height
//		if ( facingvec.v.y < -0.2f  ||  facingvec.v.y > 0.2f )
//			worryabouttarget = false;



		if ( orientation & CShipAI::INVERTED )
		{
			if ( losvec.v.y < targvec.v.y )
				response |= CShipAI::PITCH_DOWN;
			else if ( losvec.v.y > targvec.v.y )
				response |= CShipAI::PITCH_UP;
		}
		else
		{
			if ( losvec.v.y < targvec.v.y )
				response |= CShipAI::PITCH_UP;
			else if ( losvec.v.y > targvec.v.y )
				response |= CShipAI::PITCH_DOWN;
		}
		

		//// level out
		//if ( orientation & CShipAI::INVERTED )
		//{
		//	if ( orientation & CShipAI::PITCHED_UP )
		//	{
		//		response |= CShipAI::PITCH_UP;
		//	}
		//	else if ( orientation & CShipAI::PITCHED_DOWN )
		//	{
		//		response |= CShipAI::PITCH_DOWN;
		//	}
		//}
		//else
		//{
		//	if ( orientation & CShipAI::PITCHED_UP )
		//	{
		//		response |= CShipAI::PITCH_DOWN;
		//	}
		//	else if ( orientation & CShipAI::PITCHED_DOWN )
		//	{
		//		response |= CShipAI::PITCH_UP;
		//	}
		//}
	}


	this->m_fBehindTimer += this->m_fTimeDelta;

	// Add some randomness to it, so we may turn away from enemies if theyre behind us..
	// makes things more interesting
	// if we go more than s_fMaxBehindTime seconds, rethink our turning direction
	if ( ( !this->m_fTurningDirectionSelected  &&  targpos & CShipAI::BEHIND ) || this->m_fBehindTimer > s_fMaxBehindTime )
	{
		this->m_fBehindTimer = 0.0f;

		// Pick a direction to turn, exactly the same as chosing to turn toward or away from target
		if (  ( (float)rand() / (float)RAND_MAX ) < 0.5f  )
		{
			this->m_eTurningDirection = CShipAI::TURN_LEFT;
		}
		else
		{
			this->m_eTurningDirection = CShipAI::TURN_RIGHT;
		}

		// dont chose again
		this->m_fTurningDirectionSelected = true;
	}
	else if ( targpos & CShipAI::INFRONT )
	{
		// the target is infront, head toward it normally
		this->m_fTurningDirectionSelected = false;
		this->m_fBehindTimer = 0.0f;
	}





	// jutter prevention
	if ( targpos & CShipAI::INFRONT )
	{
		if ( targvec.GetLength() > s_fAIDotDistanceAllowance )
		{
			NSVector3df targdotvec = targvec;
			targdotvec.Normalise();

			float targdot = targdotvec.Dot( facingvec );

			// if were not within the dot allowance yet
			if ( !this->m_bWithinDotAllowance )
			{
				// check if were within the small dot allowance
				if ( targdot > s_fAISmallDotDeviationAllowance )
					this->m_bWithinDotAllowance = true;
			}
			else
			{
				// see if weve deviated too much
				if ( targdot < s_fAILargeDotDeviationAllowance )
					this->m_bWithinDotAllowance = false;
			}

		}	// min distance
		else
		{
			this->m_bWithinDotAllowance = false;
		}
	}	// targ infront
	else
	{
		this->m_bWithinDotAllowance = false;
	}

	// if were not uprighting the plane at this stage
	if ( worryabouttarget  &&  !this->m_bWithinDotAllowance)
	{
		sprintf( this->m_pStateDescription, "Tracking Target" );

		////////////////////////////////
		// Work out our response



		// target is to the left, or is behind us and weve been told to turn left
		if (  ( targpos & CShipAI::LEFT && !this->m_fTurningDirectionSelected )
			||  ( this->m_eTurningDirection & CShipAI::TURN_LEFT  &&  this->m_fTurningDirectionSelected ) )
		{
//			response |= CShipAI::YAW_LEFT;
			// make sure were rolled left
			if ( orientation & CShipAI::ROLLED_LEFT )
			{
				// only roll if were less than ~45 degrees
				if ( crossvec.v.y > -0.8f )
				{
					response |= CShipAI::YAW_LEFT;
					response |= CShipAI::ROLL_LEFT;
				}
				else if ( crossvec.v.y < -0.9f )
				{
//					response |= CShipAI::YAW_RIGHT;
					response |= CShipAI::ROLL_RIGHT;
				}
				else
				{
					if ( facingvec.v.y < -0.1f )
						response |= CShipAI::YAW_RIGHT;
					else
						response |= CShipAI::YAW_LEFT;
					if ( facingvec.v.y < 0.1f  &&  facingvec.v.y > -0.1f )
						response |= CShipAI::PITCH_UP;
				}
			}
			else
				response |= CShipAI::ROLL_LEFT;
		}
		else if (  ( targpos & CShipAI::RIGHT && !this->m_fTurningDirectionSelected )
			||  ( this->m_eTurningDirection & CShipAI::TURN_RIGHT  &&  this->m_fTurningDirectionSelected ) )
		{
//			response |= CShipAI::YAW_RIGHT;
			// make sure were rolled right
			if ( orientation & CShipAI::ROLLED_RIGHT )
			{
				// only roll if were less than 45 degrees
				if ( crossvec.v.y < 0.8f )
				{
					response |= CShipAI::YAW_RIGHT;
					response |= CShipAI::ROLL_RIGHT;
				}
				else if ( crossvec.v.y > 0.9f )
				{
//					response |= CShipAI::YAW_LEFT;
					response |= CShipAI::ROLL_LEFT;
				}
				else
				{
					if ( facingvec.v.y < -0.1f )
						response |= CShipAI::YAW_LEFT;
					else
						response |= CShipAI::YAW_RIGHT;
					if ( facingvec.v.y < 0.1f  &&  facingvec.v.y > -0.1f )
						response |= CShipAI::PITCH_UP;
				}
			}
			else
				response |= CShipAI::ROLL_RIGHT;
		}
	//	else
		{	// only pitch if were not rolling, this prevents the eternal cork-screw
			if ( targpos & CShipAI::ABOVE )
			{
				// we shouldnt be inverted if weve gotten this far
				if ( facingvec.v.y < 0.4f )
				{
					response |= CShipAI::PITCH_UP;
				}
//				else if ( facingvec.v.y > 0.6 )
//				{
//					response |= CShipAI::PITCH_DOWN;
//				}
			}
			else if ( targpos & CShipAI::BELOW )
			{
				if ( facingvec.v.y > -0.4f )
				{
					response |= CShipAI::PITCH_DOWN;
				}
//				else if ( facingvec.v.y < -0.6 )
//				{
//					response |= CShipAI::PITCH_UP;
//				}
			}
		}	// left / right
	}	// worry about target


	////////////////////////////////
	// Check if the target is in our gun sights
	response |= this->AICheckFireGuns( targtrans, targpos );

	if ( response & CShipAI::FIRE_GUNS )
	{
//		if ( this->m_pStateDescription )
//			delete [] this->m_pStateDescription;
//		this->m_pStateDescription = new char[30];
		sprintf( this->m_pStateDescription, "Firing Guns" );
	}


	////////////////////////////////
	// carry out our response
//	this->ApplyResponse( response );
	return response;






////////////////////////////////////////////////////////


//	int orientation = 0;
//	int targpos = 0;
//	int	response = 0;
//
//
//	// if were too close to the target
//	// evade
//	NSVector3df targtrans = *( (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL) )->GetWorldTranslation();
//	NSVector3df trans = *this->m_pNode->GetWorldTranslation();
//
//	NSVector3df targvec = targtrans - trans;
//
//
//	////////////////////////////////
//	// Get our orientation
//
//	orientation = this->AIGetOrientation();
//
//	// Orientation vectors
//	NSVector3df upvec = this->m_pNode->GetUpVector();
//	NSVector3df facingvec = this->m_pNode->GetFacingVector();
//	NSVector3df crossvec = this->m_pNode->GetCrossVector();
//
//	upvec.Normalise();
//	facingvec.Normalise();
//	crossvec.Normalise();
//
//
//	////////////////////////////////
//	// Get the targets height
//	float targheight = targtrans.v.y;
//	float height = trans.v.y;
//	float heightdelta = targheight - height;
//
//
//	// Get our line of sight relative to the targets vector
//	NSVector3df losvec = facingvec;
//	losvec.SetLength( targvec.GetLength() );
//
//	float losvecdifference = (targvec - losvec).GetLength();
//
//
//	////////////////////////////////
//	// Find out where our target is
//	// relative to our orientation
//
//	// get our translation
//	NSVector3df targleadtrans = this->AIGetFiringVector();
//
//	// create a plane along our vertical axis facing left
//
//	NSPlane horzplane = NSPlane( trans, crossvec );
//	float horzvalue = IntersectPointPlane( targleadtrans, horzplane );
//
//	horzvalue < -0.0f  ?  targpos = targpos | CShipAI::RIGHT  :  targpos = targpos | CShipAI::LEFT;
//
//	// create a plane along our horizontal axis facing up
//	NSPlane vertplane = NSPlane( trans, upvec );
//	float vertvalue = IntersectPointPlane( targleadtrans, vertplane );
//
//	vertvalue < -0.0f  ?  targpos = targpos | CShipAI::BELOW  :  targpos = targpos | CShipAI::ABOVE;
//
//	// Finished getting enemy position
//	/////////////////////////////////
//
//
//	// Avoid Collisions
//	if ( targvec.GetLength() < s_fAIEvadeDistance )
//	{
//		sprintf( this->m_pStateDescription, "Avoiding Collision" );
//
//		response |= CShipAI::PITCH_UP;
//		return response;
//	}
//
//
//
//
//	sprintf( this->m_pStateDescription, "Tracking Target" );
//
//														// Yaw
//	if ( targpos & CShipAI::LEFT )							// Left
//		response |= CShipAI::YAW_LEFT;
//	else													// Right
//		response |= CShipAI::YAW_RIGHT;
//
//
//														// Roll
////	if ( targpos & CShipAI::LEFT )							// Left
//	if ( horzvalue < -0.2f )
//	{
//		if ( losvecdifference > 50.0f )							// Large angle between los
//		{
//			response |= CShipAI::ROLL_LEFT;
//		}
//		else
//		{
//			if ( crossvec.v.y > -0.6f )
//				response |= CShipAI::ROLL_LEFT;
//			else if ( crossvec.v.y < -0.9f )
//				response |= CShipAI::ROLL_RIGHT;
//		}
//	}
////	else													// Right
//	else if ( horzvalue > 0.2f )
//	{
//		if ( losvecdifference > 50.0f )							// Large angle between los
//		{
//			response |= CShipAI::ROLL_RIGHT;
//		}
//		else
//		{
//			if ( crossvec.v.y < 0.6f )
//				response |= CShipAI::ROLL_RIGHT;
//			else if ( crossvec.v.y > 0.9f )
//				response |= CShipAI::ROLL_LEFT;
//		}
//	}
//	else
//	{
//		// Only pitch if were not rolling, to prevent corkscrews
//
//														// Pitch
//		if ( targpos & CShipAI::ABOVE )							// Enemy Above
//			response |= CShipAI::PITCH_UP;
//		else													// Enemy Below
//			response |= CShipAI::PITCH_DOWN;
//	}
//
//
//
//	////////////////////////////////
//	// Check if the target is in our gun sights
//	response |= this->AICheckFireGuns( targtrans );
//
//	if ( response & CShipAI::FIRE_GUNS )
//		sprintf( this->m_pStateDescription, "Firing Guns" );
//
//
//	////////////////////////////////
//	// carry out our response
//	return response;



}



NSVector3df CShipAI::AIGetFiringVector()
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

	// if the target is too far away, dont start leading them
	if ( targvec.GetLength() > this->s_fAIFireGunsMinTrackingDistance )
		return targtrans;

	// work out the targets distance and velocity
	float ftargdist = targvec.GetLength();
	float ftargvelo = targvelo.GetLength();

	// work out the time to the target
	float timetotarget = ftargdist / this->s_fAITracerSpeed;

	// multiply his velocity by this
	targvelo *= timetotarget;

	// add the velocity to the world translation
	targtrans += targvelo;

	// this is our new target vector

	return targtrans;
}



int CShipAI::AICheckFireGuns( NSVector3df &targtrans, int targpos )
{
	int response = 0;
	// Work out the delta between the targets vector and our heading
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
//	NSVector3df targtrans = *( (NSNode*)this->m_pTarget->PostMessage( MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	NSVector3df trans = *this->m_pNode->GetWorldTranslation();

	NSVector3df targvec = targtrans - trans;

	// get the distance from the target
	float targdist = targvec.GetLength();

	// if theyre too far away, then dont bother
	if ( targdist > this->s_fAIFireGunsMinDistance )
		return response;

	// normalise the target vector
	targvec.Normalise();

	// do a dot product of the two vectors
	float dotval = facingvec.Dot( targvec );

	// if the target is within the allowable `dot' value AND is infront of us, fire
	if (  dotval > this->s_fAIFireGunsMinDot  &&  targpos & CShipAI::INFRONT  )
		response |= CShipAI::FIRE_GUNS;

	return response;
}


int CShipAI::AIGetOrientation()
{
	int orientation = 0;

	NSVector3df upvec = this->m_pNode->GetUpVector();
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
	NSVector3df crossvec = this->m_pNode->GetCrossVector();

	upvec.Normalise();
	facingvec.Normalise();
	crossvec.Normalise();

	if ( upvec.v.y < 0.0f )
		orientation |= CShipAI::INVERTED;

	// cross vector faces left
	if ( crossvec.v.y > 0.0f )
		orientation |= CShipAI::ROLLED_RIGHT;
	else if ( crossvec.v.y < 0.0f )
		orientation |= CShipAI::ROLLED_LEFT;

	if ( facingvec.v.y < 0.0f )
		orientation |= CShipAI::PITCHED_DOWN;
	else if ( facingvec.v.y > 0.0f )
		orientation |= CShipAI::PITCHED_UP;

	return orientation;
}



int CShipAI::AIAvoidGround()
{
	// work out our orientation

	// same as in AINoTarget()

	int orientation = 0;
	int	response = 0;

	//////////////////////////////////
	// Get our orientation

	orientation = this->AIGetOrientation();


	//////////////////////////////////
	// Create a response

	if ( orientation & CShipAI::INVERTED )
		response |= CShipAI::PITCH_DOWN;
	else
		response |= CShipAI::PITCH_UP;

	// roll and yaw back
	// these are the same either way
	if ( orientation & CShipAI::ROLLED_LEFT )
	{
		response |= CShipAI::ROLL_RIGHT;
		response |= CShipAI::YAW_RIGHT;
	}
	else if ( orientation & CShipAI::ROLLED_RIGHT )
	{
		response |= CShipAI::ROLL_LEFT;
		response |= CShipAI::YAW_LEFT;
	}

	return response;
}






void CShipAI::ApplyResponse( int response )
{
	if ( response & CShipAI::PITCH_UP )
		this->m_fPitchInput = +75.0f;
	else if ( response & CShipAI::PITCH_DOWN )
		this->m_fPitchInput = -75.0f;

	if ( response & CShipAI::ROLL_LEFT )
		this->m_fRollInput = -100.0f;
	else if ( response & CShipAI::ROLL_RIGHT )
		this->m_fRollInput = +100.0f;

	if ( response & CShipAI::YAW_LEFT )
		this->m_fYawInput = -100.0f;
	else if ( response & CShipAI::YAW_RIGHT )
		this->m_fYawInput = +100.0f;

	if ( response & CShipAI::FIRE_GUNS )
		this->FireGuns();
}


void CShipAI::AISearchForTarget()
{
	CTask ** ships;
	CTask ** zeppelins;
	CTask * playership;

	int numships;
	int numzeppelins;

	// Get our arrays and counters
	if ( this->m_eAlliance == FRIENDLY )
	{
		numships = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYSHIPS, NULL );
		ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYSHIPSARRAY, NULL );

		numzeppelins = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMENEMYZEPPELINS, NULL );
		zeppelins = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_ENEMYZEPPELINSARRAY, NULL );
	}
	else
	{
		numships = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYSHIPS, NULL );
		ships = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYSHIPSARRAY, NULL );

		// allow for player
		playership = (CTask*)CShipController::Get()->PostMessage( MSGID_GET_PLAYERSHIP, NULL );
//		if ( playership != NULL )
//			numships += 1;

		numzeppelins = *(int*)CShipController::Get()->PostMessage( MSGID_GET_NUMFRIENDLYZEPPELINS, NULL );
		zeppelins = (CTask**)CShipController::Get()->PostMessage( MSGID_GET_FRIENDLYZEPPELINSARRAY, NULL );
	}


	// go through each object and see which is the closest
	CTask * closestTarget = NULL;
	NSVector3df closestPos = NSVector3df( 1000000.0f, 1000000.0f, 1000000.0f );

	NSVector3df currPos;
	NSVector3df thisPos = *this->m_pNode->GetTranslation();

	// Ships
	for ( int i = 0; i < numships; i++ )
	{
		if ( *(bool*) ships[ i ]->PostMessage(MSGID_IS_ALIVE, NULL) )
		{
			currPos = *( (NSNode*) ships[ i ]->PostMessage( MSGID_GET_NODE, NULL ) )->GetTranslation();
			currPos -= thisPos;

			// if this is the closest or the first, assign it
			if ( currPos.GetLengthSqr() < closestPos.GetLengthSqr() )//  ||  closestPos.GetLengthSqr() <= 0.0f )
			{
				closestTarget = ships[ i ];
				closestPos = currPos;
			}
		}
	}

	// Zeppelins
	for ( int i = 0; i < numzeppelins; i++ )
	{
		if ( *(bool*) zeppelins[ i ]->PostMessage(MSGID_IS_ALIVE, NULL) )
		{
			currPos = *( (NSNode*) zeppelins[ i ]->PostMessage( MSGID_GET_NODE, NULL ) )->GetTranslation();
			currPos -= thisPos;

			// if this is the closest or the first, assign it
			if ( currPos.GetLengthSqr() < closestPos.GetLengthSqr() )//  ||  closestPos.GetLengthSqr() <= 0.0f )
			{
				closestTarget = zeppelins[ i ];
				closestPos = currPos;
			}
		}
	}

	// player ship
	if ( this->m_eAlliance == ENEMY )
	{
		if ( playership != NULL )
		{
			if ( *(bool*)playership->PostMessage(MSGID_IS_ALIVE, NULL) )
			{
				currPos = *( (NSNode*) playership->PostMessage( MSGID_GET_NODE, NULL ) )->GetTranslation();
				currPos -= thisPos;

				// if this is the closest or the first, assign it
				if ( currPos.GetLengthSqr() < closestPos.GetLengthSqr() )//  ||  closestPos.GetLengthSqr() <= 0.0f )
				{
					closestTarget = playership;
					closestPos = currPos;
				}
			}
		}
	}

	this->m_pTarget = closestTarget;

	return;

//	int num;
//
//	// Chose either ships or zeppelins
//	num = (int)(  ( (float)rand() / (float)RAND_MAX ) * 2.0f  );
//
//	// between 0 and 1
//	if ( num < 1 )
//	{
//		// ships
//		num = (int)(  ( (float)rand() / (float)RAND_MAX ) * (float)numships  );
//
//		// The enemy can target the player
//		if ( this->m_eAlliance == ENEMY )
//		{
//			if ( playership != NULL )
//			{
//				if ( num >= numships )
//				{
//					if ( *(bool*)playership->PostMessage(MSGID_IS_ALIVE, NULL) )
//					{
//						// get the player as a target
//						this->m_pTarget = playership;
//						return;
//					}
//				}
//
//				// were not targeting the player, so just use our pre-fetched arrays
//				num--;
//			}
//		}
//
//		// invalid ship
//		if ( numships <= 0  ||  num < 0  ||  num >= numships )
//			return;
//
//
//		// Ship
//		if ( ships[ num ] != NULL )
//		{
//			if ( *(bool*)ships[ num ]->PostMessage(MSGID_IS_ALIVE, NULL) )
//			{
//				this->m_pTarget = ships[ num ];
//			}
//		}
//	}
//	else
//	{
//		// zeppelins
//		num = (int)(  ( (float)rand() / (float)RAND_MAX ) * (float)numzeppelins  );
//
////		num--;
//		// invalid zeppelin
//		if ( numzeppelins < 0  ||  num < 0  ||  num >= numzeppelins )
//			return;
//
//		if ( zeppelins[ num ] != NULL )
//		{
//			if ( *(bool*)zeppelins[ num ]->PostMessage(MSGID_IS_ALIVE, NULL) )
//			{
//				this->m_pTarget = zeppelins[ num ];
//			}
//		}
//	}





//	num = numships + (numzeppelins * 2);	// add some weight to the zeppelins

	// pick a random target
//	num = (int)(  ( (float)rand() / (float)RAND_MAX ) * (float)num  );

	//~ if ( this->m_eAlliance == ENEMY )
	//~ {
		//~ if ( playership != NULL  &&  num == numships + 1 )
		//~ {
			//~ if ( playership != NULL )
			//~ {
				//~ if ( *(bool*)playership->PostMessage(MSGID_IS_ALIVE, NULL) )
				//~ {
					//~ // get the player as a target
					//~ this->m_pTarget = playership;
					//~ return;
				//~ }
			//~ }
		//~ }
		//~ // undo the player, since its not him
		//~ num -= 1;
	//~ }

	//~ if ( num >= numships )
	//~ {
		//~ // Zeppelin
		//~ num -= numships;
		//~ num = num / 2;

		//~ if ( zeppelins[ num ] != NULL )
		//~ {
			//~ if ( *(bool*)zeppelins[ num ]->PostMessage(MSGID_IS_ALIVE, NULL) )
			//~ {
				//~ this->m_pTarget = zeppelins[ num ];
			//~ }
		//~ }
	//~ }
	//~ else
	//~ {
		//~ // Ship
		//~ if ( ships[ num ] != NULL )
		//~ {
			//~ if ( *(bool*)ships[ num ]->PostMessage(MSGID_IS_ALIVE, NULL) )
			//~ {
				//~ this->m_pTarget = ships[ num ];
			//~ }
		//~ }
	//~ }

}



void CShipAI::UpdateInput()
{
	// Primary AI update
	int response = 0;


	if ( this->AICheckGroundLevel() )
	{
		sprintf( this->m_pStateDescription, "Avoiding Ground" );

		this->m_bAvoidingGround = true;

		// evade the ground
		response = this->AIAvoidGround();
	}
	else if ( !this->m_pTarget )
	{
		// if we dont have a target, then just fly around

		// find a target every X ticks
		if ( this->m_fTargetSearchTimer >= this->s_fAITargetSearchTime )
		{
			// try and get a target
			this->AISearchForTarget();

			// reset our timer
			this->m_fTargetSearchTimer = 0.0f;
		}

		// increment our search timer
		this->m_fTargetSearchTimer += this->m_fTimeDelta;

		// if we still have no target
		if ( this->m_pTarget == NULL )
			response = this->AINoTarget();
	}
	else
	{
		response = this->AIWithTarget();
	}

	////////////////////////////////
	// carry out our response
	this->ApplyResponse( response );
}
