#include "Particles.h"


int			PSystem::m_iNumPSystems								= 0;

const int	CityExplosionPSystem::s_iMaxNumParticles			= 20;
const int	CityExplosionPSystem::s_iMinNumParticles			= 10;
const float	CityExplosionPSystem::s_fMaxRotationSpeed			= 15.0f;
const float	CityExplosionPSystem::s_fMinVelocity				= 20.0f;//10.0f
const float	CityExplosionPSystem::s_fMaxVelocity				= 60.0f;//10.0f
const float	CityExplosionPSystem::s_fMinLifeSpan				= 5.0f;//2.0f
const float	CityExplosionPSystem::s_fMaxLifeSpan				= 10.0f;//2.0f
const float	CityExplosionPSystem::s_fVelocityDampening			= 0.01f;
const float	CityExplosionPSystem::s_fGravityStrength			= 35.0f;//1.0f
//const float	CityExplosionPSystem::s_fRadialSpread				= 30.0f;

const int	CityExplosionPSystem::s_iNumSmokePerParticle		= 10;
const float	CityExplosionPSystem::s_fSmokeMaxLifeSpan			= 2.2f;
const float	CityExplosionPSystem::s_fSmokeMinLifeSpan			= 0.5f;
const float	CityExplosionPSystem::s_fSmokeTrailCreationSpeed	= 0.3f;


const float	PlaneShrapnel::s_fGravAccel			= 35.0f;

const float	PlaneShrapnel::s_fMinLifeSpan		= 1.0f;
const float	PlaneShrapnel::s_fMaxLifeSpan		= 3.0f;
const float	PlaneShrapnel::s_fMaxRotationSpeed	= 15.0f;

const unsigned int PlaneSmoke::s_iMaxNumParticles	= 10;
const float	PlaneSmoke::s_fMinLifeSpan				= 0.5f;
const float	PlaneSmoke::s_fMaxLifeSpan				= 2.2f;



//////////////////////////////
// Particle

Particle::Particle()
{
}

Particle::Particle( NSNode * node )
{
	this->m_pNode = node;
}


Particle::~Particle()
{
	delete this->m_pNode;
}



//////////////////////////////
// PSystem

PSystem::PSystem( NSNode * particleBranch, NSVector3df &trans )
{
	this->m_pParticleBranch = particleBranch;

	// create our node name
	char buff[20];
	sprintf(buff, "PSystem%i", this->m_iNumPSystems);
	this->m_iNumPSystems++;

	// create the node
	this->m_pNode = new NSNode( buff );
	this->m_pNode->SetTranslate( trans );

	this->m_pNode->Update();

	this->m_pParticleBranch->AttachChild( this->m_pNode );

	this->m_iNumParticles = 0;
	this->m_bIsFinished = false;
}

PSystem::~PSystem()
{
	// delete our psystem node
	this->m_pParticleBranch->DetachChild( this->m_pNode );
	delete this->m_pNode;

	this->m_iNumPSystems--;
}

bool PSystem::IsFinished()
{
	return this->m_bIsFinished;
}









//////////////////////////////
// PlaneSmoke

PlaneSmoke::PlaneSmoke( NSNode *particleBranch, NSNode *planeNode ) : PSystem( particleBranch, NSVector3df(0.0f, 0.0f, 0.0f) )
{
//	this->m_bSmoke = smoke;
	//NSNode *node;
	this->m_pPlaneNode = planeNode;

	this->m_iNumParticles = 0;
	this->m_bStopped = false;

	this->m_pParticles = new Particle*[ s_iMaxNumParticles ];

	// Update the node
	//node->Update();
	this->m_pNode->Update();
}

PlaneSmoke::~PlaneSmoke()
{
	if ( this->m_pParticles )
	{
		for ( unsigned int i = 0; i < this->m_iNumParticles; i++ )
		{
			NSBillboard * mesh = (NSBillboard*)this->m_pParticles[i]->m_pNode->GetMesh();
			delete mesh;

			delete this->m_pParticles[i];
		}
		delete [] this->m_pParticles;
	}

	this->m_pParticles = NULL;
}

void PlaneSmoke::Update( float fTimeDelta )
{
	if ( this->m_bIsFinished || this->m_pParticles == NULL )
		return;

	// check for old ones
	for ( unsigned int i = 0; i < this->m_iNumParticles; i++ )
	{
		if ( this->m_pParticles[i]->m_fCurrLifeSpan > this->m_pParticles[i]->m_fMaxLifeSpan )
		{
			delete this->m_pParticles[ i ];
			this->m_iNumParticles--;
			this->m_pParticles[ i ] = this->m_pParticles[ this->m_iNumParticles ];
			this->m_pParticles[ this->m_iNumParticles ] = NULL;
		}
	}




	// update our particles
	for ( unsigned int i = 0; i < this->m_iNumParticles; i++ )
	{
		this->m_pParticles[i]->m_fCurrLifeSpan += fTimeDelta;

		((NSBillboard*)this->m_pParticles[i]->m_pNode->GetMesh() )->SetSize( (this->m_pParticles[i]->m_fCurrLifeSpan * 10.0f) + 0.5f );
	}




	// create new smoke particles
	if ( !this->m_bStopped )
	{
		if ( this->m_iNumParticles < s_iMaxNumParticles  &&  (float)rand() / (float)RAND_MAX  > 0.8f )
		{
			char buff[20];
			sprintf( buff, "ShipSmoke%i", this->m_iNumParticles );	// not entirely accurate.. but meh

			// create the node
			NSNode *node = new NSNode( buff );

			node->SetTranslate( *this->m_pPlaneNode->GetTranslation() );

			this->m_pNode->AttachChild( node );

			// get a mesh for the node
			CMeshManager::MESHES meshtype;
			meshtype = CMeshManager::SMOKE_BLACK;

			NSBillboard * mesh = (NSBillboard*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);
			NSBillboard * cpymesh = new NSBillboard();
			memcpy( cpymesh, mesh, sizeof(NSBillboard) );
			node->AttachMesh( cpymesh );

			((NSBillboard*)mesh)->SetSize( 0.5f );


			this->m_pParticles[ this->m_iNumParticles ] = new Particle( node );

			// create a random lifespan
			this->m_pParticles[ this->m_iNumParticles ]->m_fCurrLifeSpan = 0.0f;
			this->m_pParticles[ this->m_iNumParticles ]->m_fMaxLifeSpan = ( (float)rand() / (float)RAND_MAX ) * (s_fMaxLifeSpan - s_fMinLifeSpan) + s_fMinLifeSpan;

			this->m_iNumParticles++;
		}
	}
	else
	{
		// once our particles are dead, finish
		if ( this->m_iNumParticles <= 0 )
			this->m_bIsFinished = true;
	}

}


void PlaneSmoke::Stop()
{
	this->m_bStopped = true;
}











//////////////////////////////
// PlaneShrapnel

PlaneShrapnel::PlaneShrapnel( NSNode *particleBranch, NSVector3df &trans ) : PSystem( particleBranch, trans )
{
//	this->m_bSmoke = smoke;

	// create our node name
	char buff[20];
	sprintf( buff, "ShipShrap" );

	// create the node
	NSNode * node = new NSNode( buff );

	this->m_pNode->AttachChild( node );
	// nodes are at 0, 0, 0 by default

	// get a mesh for the node
	CMeshManager::MESHES meshtype;
//	if ( this->m_bSmoke )
//		meshtype = CMeshManager::SMOKE_BLACK;
//	else
		meshtype = CMeshManager::PLANE_SHRAPNEL_1;

	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);
	node->AttachMesh( mesh );

//	if ( this->m_bSmoke )
//		((NSBillboard*)mesh)->SetSize( 0.5f );


	this->m_pParticles = new Particle*[1];
	this->m_pParticles[0] = new Particle( node );

	// create a random lifespan
	this->m_pParticles[0]->m_fCurrLifeSpan = 0.0f;
	this->m_pParticles[0]->m_fMaxLifeSpan = ( (float)rand() / (float)RAND_MAX ) * (s_fMaxLifeSpan - s_fMinLifeSpan) + s_fMinLifeSpan;

	this->m_kVelocity = NSVector3df( 0.0f, -1.0f, 0.0f );

	// create a random rotation for the particle
	this->m_fRotationX = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;
	this->m_fRotationY = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;
	this->m_fRotationZ = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;

	// invert some of the rotations
	if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
		this->m_fRotationX *= -1;
	if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
		this->m_fRotationY *= -1;
	if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
		this->m_fRotationZ *= -1;

	// Update the node
	//node->Update();
	this->m_pNode->Update();
}

PlaneShrapnel::~PlaneShrapnel()
{
	if ( this->m_pParticles )
	{
		delete this->m_pParticles[0];
		delete [] this->m_pParticles;
	}

	this->m_pParticles = NULL;
}

void PlaneShrapnel::Update( float fTimeDelta )
{
	if ( this->m_bIsFinished || this->m_pParticles == NULL )
		return;

	if ( this->m_pParticles[0]->m_fCurrLifeSpan > this->m_pParticles[0]->m_fMaxLifeSpan )
	{
		delete this->m_pParticles[0];
		delete [] this->m_pParticles;

		this->m_pParticles = NULL;

		this->m_bIsFinished = true;

		return;
	}

	this->m_pParticles[0]->m_fCurrLifeSpan += fTimeDelta;

	// Apply our rotation
	NSMatrix4 * mat = this->m_pParticles[0]->m_pNode->GetMatrix();
	NSMatrix4 rotation;
	rotation.SetRotationY( this->m_fRotationY * fTimeDelta );
	*mat *= rotation;
	rotation.SetRotationX( this->m_fRotationX * fTimeDelta );
	*mat *= rotation;
	rotation.SetRotationZ( this->m_fRotationZ * fTimeDelta );
	*mat *= rotation;

	float velo = this->m_kVelocity.GetLength();
	velo += ( s_fGravAccel * fTimeDelta );

	this->m_kVelocity.SetLength( velo );

	this->m_pParticles[0]->m_pNode->SetTranslate( this->m_kVelocity );
}










//////////////////////////////
// CityExplosionPSystem

CityExplosionPSystem::CityExplosionPSystem( NSNode * particleBranch, NSVector3df &trans, float fRadius, NSVector3df &velocity ) : PSystem( particleBranch, trans )
{
	// super class constructor creates our particle node

	// create a random number of particles
	this->m_iNumParticles = (int)( ( (float)rand() / (float)RAND_MAX ) * ( (float)this->s_iMaxNumParticles - (float)this->s_iMinNumParticles )	);
	this->m_iNumParticles += this->s_iMinNumParticles;

	// create our arrays
	this->m_pParticles = new Particle * [ this->m_iNumParticles ];


	this->m_fRotationX = new float [ this->m_iNumParticles ];
	this->m_fRotationY = new float [ this->m_iNumParticles ];
	this->m_fRotationZ = new float [ this->m_iNumParticles ];

	this->m_kVelocity = new NSVector3df [ this->m_iNumParticles ];

	this->m_kPSystemVelocity = velocity;


	for ( int i = 0; i < this->m_iNumParticles; i++ )
	{
		// create a particle
	
		// create our node name
		char buff[20];
		sprintf( buff, "CtyExpPart%i", i );

		// create the node
		NSNode * node = new NSNode( buff );

		this->m_pNode->AttachChild( node );
		// nodes are at 0, 0, 0 by default

		// get a mesh for the node
		CMeshManager::MESHES meshtype;
		meshtype = CMeshManager::CITY_SHRAPNEL_1;

		NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);
		node->AttachMesh( mesh );


		this->m_pParticles[i] = new Particle( node );

		// create a random lifespan
		this->m_pParticles[i]->m_fCurrLifeSpan = 0.0f;
		this->m_pParticles[i]->m_fMaxLifeSpan = ( (float)rand() / (float)RAND_MAX ) * (this->s_fMaxLifeSpan - this->s_fMinLifeSpan) + this->s_fMinLifeSpan;

		// create a random velocity that will always be up
		this->m_kVelocity[i].v.x = ( (float)rand() / (float)RAND_MAX );// * this->s_fMaxVelocity;
		this->m_kVelocity[i].v.y = ( (float)rand() / (float)RAND_MAX );// * this->s_fMaxVelocity;
		this->m_kVelocity[i].v.z = ( (float)rand() / (float)RAND_MAX );// * this->s_fMaxVelocity;

		// invert some of the velocity
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			this->m_kVelocity[i].v.x *= -1;
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			this->m_kVelocity[i].v.z *= -1;

		// set the velocity length
		this->m_kVelocity[i].SetLength(  ( (float)rand() / (float)RAND_MAX ) * (this->s_fMaxVelocity - this->s_fMinVelocity) + this->s_fMinVelocity );

		// spread the nodes around the explosion at distance s_fRadialSpread using their velocity
		NSVector3df trans = this->m_kVelocity[i];
		trans.SetLength( fRadius );//this->s_fRadialSpread );
		node->SetTranslate( trans );

		// create a random rotation for the particle
		this->m_fRotationX[i] = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;
		this->m_fRotationY[i] = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;
		this->m_fRotationZ[i] = ( (float)rand() / (float)RAND_MAX ) * this->s_fMaxRotationSpeed;

		// invert some of the rotations
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			this->m_fRotationX[i] *= -1;
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			this->m_fRotationY[i] *= -1;
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			this->m_fRotationZ[i] *= -1;

		// Update the node
		node->Update();
	}

	// our particles
	// pointer to [num main particles] [num smoke trail particles per main particle]
	this->m_pSmokeTrails = new Particle ** [ this->m_iNumParticles ];

	// create a node for each group so we dont reach our max number of children
	this->m_pSmokeNodes = new NSNode * [ this->m_iNumParticles ];

	for ( int i = 0; i < this->m_iNumParticles; i++)
	{
		this->m_pSmokeTrails[i] = new Particle * [ this->s_iNumSmokePerParticle ];

		char trailbuff[20];
		sprintf( trailbuff, "CitySmkGroup%i", i );

		// create the node
		this->m_pSmokeNodes[i] = new NSNode( trailbuff );
		this->m_pNode->AttachChild( this->m_pSmokeNodes[i] );

		for ( int j = 0; j < this->s_iNumSmokePerParticle; j++ )
		{
			char buff[20];
			sprintf( buff, "CitySmkPart%i", i );

			// create the node
			NSNode * node = new NSNode( buff );

			//this->m_pNode->AttachChild( node );
			this->m_pSmokeNodes[i]->AttachChild( node );
			// nodes are at 0, 0, 0 by default

			// get a mesh for the node
			CMeshManager::MESHES meshtype;
			meshtype = CMeshManager::SMOKE_BLACK;

			NSBillboard * mesh = (NSBillboard*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);
			NSBillboard * cpymesh = new NSBillboard();
			memcpy( cpymesh, mesh, sizeof(NSBillboard) );
			node->AttachMesh( cpymesh );		// this will leak memory, since we cant delete the mesh

			this->m_pSmokeTrails[i][j] = new Particle( node );

			// make our nodes invisible
			this->m_pSmokeTrails[i][j]->m_pNode->SetVisibility( false );

			// create a lifespan for the smoke
			this->m_pSmokeTrails[i][j]->m_fCurrLifeSpan = 0.0f;
			this->m_pSmokeTrails[i][j]->m_fMaxLifeSpan = ( (float)rand() / (float)RAND_MAX ) * (this->s_fSmokeMaxLifeSpan - this->s_fSmokeMinLifeSpan) + this->s_fSmokeMinLifeSpan;
		}
	}

	// create our counter for our smoke trails
	this->m_iCurrSmokeTrail = new int [ this->m_iNumParticles ];

	for ( int i = 0; i < this->m_iNumParticles; i++ )
		this->m_iCurrSmokeTrail[i] = 0;

	this->m_fSmokeTrailTimer = new float [ this->m_iNumParticles ];

	for ( int i = 0; i < this->m_iNumParticles; i++ )
		this->m_fSmokeTrailTimer[i] = this->s_fSmokeTrailCreationSpeed;	// create the first straight away


	this->m_pNode->Update();
}

CityExplosionPSystem::~CityExplosionPSystem()
{
	for ( int i = 0; i < this->m_iNumParticles; i++)
	{
		delete this->m_pParticles[i];
	}
	delete [] this->m_pParticles;

	for ( int i = 0; i < this->m_iNumParticles; i++)
	{
		for ( int j = 0; j < this->s_iNumSmokePerParticle; j++ )
		{
//			delete this->m_pSmokeTrails[i][j]->m_pNode;

			// The memcpy doesnt actually copy the mesh... eugh.. ive really messed up my particle system
			// our meshes are memcpy'ied
			// we must delete them
			NSBillboard * mesh = (NSBillboard*)this->m_pSmokeTrails[i][j]->m_pNode->GetMesh();
			delete mesh;

			delete this->m_pSmokeTrails[i][j];
		}
		delete [] this->m_pSmokeTrails[i];
	}
	delete [] this->m_pSmokeTrails;

	// delete the group nodes
	for ( int i = 0; i < this->m_iNumParticles; i++)
	{
		delete this->m_pSmokeNodes[i];
	}
	delete [] this->m_pSmokeNodes;

	delete [] this->m_iCurrSmokeTrail;
	delete [] this->m_fSmokeTrailTimer;

	delete [] this->m_fRotationX;
	delete [] this->m_fRotationY;
	delete [] this->m_fRotationZ;

	delete [] this->m_kVelocity;
}

void CityExplosionPSystem::Update( float fTimeDelta )
{
	// update our finished flag
	if ( this->m_iNumParticles < 1 )
	{
		this->m_bIsFinished = true;
		return;
	}

	// check for dead particles
	for ( int i = 0; i < this->m_iNumParticles; i++ )
	{
		// if the particle is past its lifespan
		if ( this->m_pParticles[i]->m_fCurrLifeSpan >= this->m_pParticles[i]->m_fMaxLifeSpan )
		{
			// delete it
			delete this->m_pParticles[i];

			// do the same for the smoke trail
			for ( int j = 0; j < this->s_iNumSmokePerParticle; j++ )
			{
				// this is done by the particle!
//				delete this->m_pSmokeTrails[i][j]->m_pNode;

				// the memcpy doesnt actually copy the mesh data.. so it doesnt matter..
				// our meshes are memcpy'ied
				// we must delete them
				//NSElement * mesh = this->m_pSmokeTrails[i][j]->m_pNode->GetMesh();
				//delete mesh;

				delete this->m_pSmokeTrails[i][j];
			}
			delete [] this->m_pSmokeTrails[i];

			// delete that group's node
			delete this->m_pSmokeNodes[i];

			// shuffle down the array and decrement our counter
			this->m_iNumParticles--;
			this->m_pParticles[i] = this->m_pParticles[ this->m_iNumParticles ];

			this->m_pSmokeTrails[i] = this->m_pSmokeTrails[ this->m_iNumParticles ];
			this->m_iCurrSmokeTrail[i] = this->m_iCurrSmokeTrail[ this->m_iNumParticles ];
			this->m_fSmokeTrailTimer[i] = this->m_fSmokeTrailTimer[ this->m_iNumParticles ];
			this->m_pSmokeNodes[i] = this->m_pSmokeNodes[ this->m_iNumParticles ];

			this->m_pParticles[ this->m_iNumParticles ] = NULL;

			this->m_iCurrSmokeTrail[ this->m_iNumParticles ] = NULL;
			this->m_pSmokeTrails[ this->m_iNumParticles ] = NULL;
			this->m_fSmokeTrailTimer[ this->m_iNumParticles ] = NULL;
			this->m_pSmokeNodes[ this->m_iNumParticles ] = NULL;
		}

	}

	// update our alive particles
	NSVector3df trans;
	NSVector3df velo;

	// work out the dampening scale for this update
	float veloscale = this->s_fVelocityDampening * fTimeDelta;
	veloscale = 1.0f - veloscale;

	// scale our overall scale
	velo = this->m_kPSystemVelocity;
	velo *= veloscale;
	this->m_kPSystemVelocity = velo;

	for ( int i = 0; i < this->m_iNumParticles; i++ )
	{
		// add to our lifespan
		this->m_pParticles[i]->m_fCurrLifeSpan += fTimeDelta;

		// apply our rotation FIRST
		NSMatrix4 * mat = this->m_pParticles[i]->m_pNode->GetMatrix();
		NSMatrix4 rotation;
		rotation.SetRotationY( this->m_fRotationY[i] * fTimeDelta );
		*mat *= rotation;
		rotation.SetRotationX( this->m_fRotationX[i] * fTimeDelta );
		*mat *= rotation;
		rotation.SetRotationZ( this->m_fRotationZ[i] * fTimeDelta );
		*mat *= rotation;

		// update the velocity
		velo = this->m_kVelocity[i];
		velo *= veloscale;
		// add gravity
		velo.v.y -= (this->s_fGravityStrength * fTimeDelta);
		this->m_kVelocity[i] = velo;

		// update the translation
		trans = *this->m_pParticles[i]->m_pNode->GetTranslation();
		trans += (velo * fTimeDelta);

		trans += this->m_kPSystemVelocity * fTimeDelta;

		// add gravity
		//trans.v.y -= (this->s_fGravityStrength * fTimeDelta);

		this->m_pParticles[i]->m_pNode->SetTranslate( trans );

		this->m_pParticles[i]->m_pNode->Update();

		// collision detect
	}

	// update our smoke trails

	// for each particle
	for ( int i = 0; i < this->m_iNumParticles; i++ )
	{
		// update each smoke trail

		// if our counter isnt at s_iNumSmokeTrails per particle,
		// then we still have some left to initialise
		int * currtrail = &this->m_iCurrSmokeTrail[i];
		float * currtimer = &this->m_fSmokeTrailTimer[i];

		if ( (*currtrail) < this->s_iNumSmokePerParticle )
		{
			// add to our timer
			(*currtimer) += fTimeDelta;

			// if our timer is at the right time
			if ( (*currtimer) >= this->s_fSmokeTrailCreationSpeed )
			{
				// show the next one
				this->m_pSmokeTrails[i][ *currtrail ]->m_pNode->SetVisibility( true );

				NSVector3df trans = *this->m_pParticles[i]->m_pNode->GetTranslation();
				this->m_pSmokeTrails[i][ *currtrail ]->m_pNode->SetTranslate(trans);

				// increment our current smoke trail
				(*currtrail)++;
				(*currtimer) = 0.0f;
			}
		}

		for ( int j = 0; j < *currtrail; j++ )
		{
			// update each active smoke particle
			this->m_pSmokeTrails[i][j]->m_fCurrLifeSpan += fTimeDelta;

			// if the particle is dead.. respawn it
			if ( this->m_pSmokeTrails[i][j]->m_fCurrLifeSpan > this->m_pSmokeTrails[i][j]->m_fMaxLifeSpan )
			{
				NSVector3df trans = *this->m_pParticles[i]->m_pNode->GetTranslation();
				this->m_pSmokeTrails[i][j]->m_pNode->SetTranslate(trans);

				this->m_pSmokeTrails[i][j]->m_fCurrLifeSpan = 0.0f;
			}

			// update the nodes colour
			NSBillboard * mesh = (NSBillboard*)this->m_pSmokeTrails[i][j]->m_pNode->GetMesh();
			NSColouraf colour;
			float currlife = this->m_pSmokeTrails[i][j]->m_fCurrLifeSpan;
			float maxlife = this->m_pSmokeTrails[i][j]->m_fMaxLifeSpan;

			// currlife may be slightly bigger on the last update
			if ( currlife > maxlife )
				currlife = maxlife;
			float scale = currlife / maxlife;
			colour.a = scale;
			colour.r = scale;
			colour.g = scale;
			colour.b = scale;
			mesh->SetColour( colour );

			float size = 15.0f * ( 1.0f - scale ) + 0.1f;
			mesh->SetSize( size );
		}
	}
}
