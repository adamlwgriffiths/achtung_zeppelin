#include "Tracer.h"

int		CTracer::s_iNumTracers				= 0;
float	CTracer::s_fRadius					= 1.0f;
int		CTracer::s_iDamage					= 10;
float	CTracer::s_fGravityPullStrength		= 2.0f;
float	CTracer::s_fLifetime				= 2.5f;
float	CTracer::s_fSpeed					= 400.0f;


void * CTracer::PostMessage(int ID, void * Data)
{
	void * result = 0;

	bool bResult;

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

		case MSGID_IS_LIFETIMEEXPIRED:
			if ( this->m_fLifetime >= this->s_fLifetime )
				bResult = true;
			else
				bResult = false;
			result = &bResult;
			break;

		case MSGID_GET_ALLIANCE:
			result = &this->m_eAlliance;
			break;

		case MSGID_GET_BOUNDINGRADIUS:
			result = &this->s_fRadius;
			break;

		case MSGID_GET_NODE:
			result = this->m_pNode;
			break;

		case MSGID_GET_DAMAGEAMOUNT:
			result = &this->s_iDamage;
			break;

		case MSGID_GET_OWNER:
			result = this->m_pOwner;
			break;

		case MSGID_TRACER_HIT:
			this->m_fLifetime = this->s_fLifetime;
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CTracer::CTracer()
{
}

CTracer::~CTracer()
{
}


void CTracer::Init(void * Data)
{
	TracerSetupData data = *(TracerSetupData*)Data;

	// create a name for the node
	char buff[10];
	sprintf(buff, "tracer%i", this->s_iNumTracers);
	this->s_iNumTracers++;

	// create the node
	this->m_pNode = new NSNode( buff );
	this->m_pNode->SetTranslate( data.pos );
//	this->m_pNode->SetMatrix( *( (NSNode*)data.owner->PostMessage( MSGID_GET_NODE, NULL ) )->GetMatrix() );

	this->m_eAlliance = data.alliance;
	this->m_bFlatWorld = data.flatworld;
	this->m_pOwner = data.owner;
	this->m_fLifetime = 0.0f;
	this->m_kShipVelocity = data.shipvelo;

	// get our velocity vector
	this->m_kVelocity = data.dir;
	this->m_kVelocity.SetLength( this->s_fSpeed );




	// set our mesh
	CMeshManager::MESHES meshtype;
	if ( this->m_eAlliance == FRIENDLY )
		meshtype = CMeshManager::TRACER_FRIEND;
	else
		meshtype = CMeshManager::TRACER_ENEMY;

	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pNode->AttachMesh( mesh );

	this->m_pBulletBranch = data.bulletBranch;
	this->m_pBulletBranch->AttachChild( this->m_pNode );

	//// create a random rotation for the tracer
	//m_fRotationX = ( (float)rand() / (float)RAND_MAX ) * TRACER_ROTATION_SPEED;
	//m_fRotationY = ( (float)rand() / (float)RAND_MAX ) * TRACER_ROTATION_SPEED;
	//m_fRotationZ = ( (float)rand() / (float)RAND_MAX ) * TRACER_ROTATION_SPEED;

	//// invert some of the rotations
	//if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
	//	m_fRotationX *= -1;
	//if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
	//	m_fRotationY *= -1;
	//if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
	//	m_fRotationZ *= -1;




	NSVector3df velo = this->m_kVelocity;
	velo.Normalise();

	float theta = 0.0f;


	// get the length of the hypotenuse of x, z
	float hyp = pow( velo.v.x, 2.0f ) + pow( velo.v.z, 2.0f );
	hyp = sqrt(hyp);

	theta = atan(  ( velo.v.y / hyp )  );

	// turn theta so that 0 is upward
//	theta -= NS_HALF_PI;
	if ( (velo.v.x < 0.0f  &&  velo.v.z < 0.0f)   ||   (velo.v.x < 0.0f  &&  velo.v.z > 0.0f) )
		theta += NS_PI;

	// make sure theta isnt -ve
	if (theta < 0.0f)
		theta += NS_TWO_PI;


	// get the cross vector between up and its position to rotate around
	NSVector3df facingvec = this->m_pNode->GetFacingVector();
	NSVector3df upvec = NSVector3df( 0.0f, 1.0f, 0.0f );
	NSVector3df rotvec = upvec.CrossProduct( facingvec );//velo );
	// rotvec is facing left

	// rotate the city around the rotation axis
	NSMatrix4 * mat = this->m_pNode->GetMatrix();
	mat->SetRotationAboutAxis( rotvec, -theta );
//	mat->SetRotationY( theta );




	///////////////////////////////
	// Work out the X of the cell

	float collX, collY;

	collX = velo.v.x;
	collY = velo.v.z;

	// get the angle the collidee is at
	theta = atan(  ( collY / collX )  );

	if ( (collX < 0.0f   &&  collY > 0.0f)  ||  (collX < 0.0f   &&  collY < 0.0f) )
		theta -= NS_PI;

	// theta = 0 is pointing right, and as it increases it goes clock wise (because -ve z is where +ve y would be)
	// we need theta 0 to point up, and as it increases, to turn clock wise
	theta += NS_HALF_PI;

	// make sure theta isnt -ve
	if (theta < 0.0f)
		theta += NS_TWO_PI;



	// rotate the city around the rotation axis
	NSMatrix4 mat2;
	mat2.SetIdentity();
	mat2.SetRotationAboutAxis( upvec, -theta );
	*mat *= mat2;
}


void CTracer::Destroy()
{
	this->m_pBulletBranch->DetachChild( this->m_pNode );
	delete this->m_pNode;

	this->s_iNumTracers--;
}



void CTracer::Update(void * Data)
{
	this->m_fTimeDelta = *(float*)Data;

	// increment our lifetime
	this->m_fLifetime += this->m_fTimeDelta;

	//// apply our rotation FIRST
	//NSMatrix4 * mat = this->m_pNode->GetMatrix();
	//NSMatrix4 rotation;
	//rotation.SetRotationY( this->m_fRotationY * this->m_fTimeDelta );
	//*mat *= rotation;
	//rotation.SetRotationX( this->m_fRotationX * this->m_fTimeDelta );
	//*mat *= rotation;
	//rotation.SetRotationZ( this->m_fRotationZ * this->m_fTimeDelta );
	//*mat *= rotation;

	// apply our translation
	NSVector3df trans = *this->m_pNode->GetTranslation();

	NSVector3df velo = this->m_kVelocity;
	velo.Scale( this->m_fTimeDelta );
	trans += velo;

	// add gravity
	NSVector3df gravityvec;
	if (this->m_bFlatWorld)
	{
		// Flat
		gravityvec = NSVector3df( 0.0f, -this->s_fGravityPullStrength * this->m_fTimeDelta, 0.0f );
	}
	else
	{
		// Spherical
		gravityvec = *this->m_pNode->GetTranslation();
		gravityvec.SetLength( -this->s_fGravityPullStrength * this->m_fTimeDelta );
	}
	trans += gravityvec;

	// inherit some velocity from the parent ship
//	this->m_kShipVelocity.Scale( TRACER_SHIPVELOCITY_INHERITANCE * this->m_fTimeDelta );
//	trans += this->m_kShipVelocity;

	this->m_pNode->SetTranslate( trans );
	this->m_pNode->Update();

	// check for collisions
	// ships
	CShipController::Get()->PostMessage( MSGID_CHECK_COLLISIONSSPHEREVSBULLET, this );

	// zeppelins
	CShipController::Get()->PostMessage( MSGID_CHECK_COLLISIONSZEPPELINVSBULLET, this );

	// cities
	CCityManager::Get()->PostMessage( MSGID_CHECK_COLLISIONSBOXVSBULLET, this );

}
