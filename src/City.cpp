#include "City.h"

int			CCity::m_iNumCities				= 0;

const float	CCity::s_fCityFallSpeed			= 40.0f;
const float	CCity::s_fCityFallMaxJitter		= 0.7f;
float		CCity::s_fMass					= 1000.0f;


void * CCity::PostMessage(int ID, void * Data)
{
	void * result = 0;
	//float fResult = 0.0f;
	NSVector3df vecresult;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init(Data);							// intialise game structures
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			result = &this->m_bFinished;
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_APPLY_DAMAGE:
#ifdef CITY_DAMAGEABLE
			this->ApplyDamage( *(int*)Data );
#endif // #ifdef CITY_DAMAGEABLE
			break;

		case MSGID_GET_ALLIANCE:
			result = &this->m_eAlliance;
			break;

		case MSGID_GET_BOUNDINGRADIUS:
			result = &this->m_fRadius;
			break;

		case MSGID_GET_NODE:
			result = this->m_pNode;
			break;

		case MSGID_GET_MASS:
			result = &this->s_fMass;
			break;

		case MSGID_GET_VELOCITY:
			//fResult = 0.0f;
			vecresult = NSVector3df( 0.0f, 0.0f, 0.0f );
			result = &vecresult;
			break;

		case MSGID_SET_ARRAYPOSITION:
			this->m_iArrayPosition = *(int*)Data;
			break;

		case MSGID_GET_ARRAYPOSITION:
			result = &this->m_iArrayPosition;
			break;


		default:
			assert(0);									// should never get here
	}

	return result;
}


CCity::CCity()
{
}

CCity::~CCity()
{
}




void CCity::Init(void * Data)
{
	CitySetupData * data = (CitySetupData*)Data;

	this->m_pCityBranch = data->cityBranch;

	this->m_iHealth = CITY_HEALTH_MAX;
	this->m_bAlive = true;
	this->m_pParticleSystem = NULL;
	this->m_bFinished = false;

	// create a name for the node
	//char * buff = new char[10];
	char buff[10];
	sprintf(buff, "city%i", this->m_iNumCities);
	this->m_iNumCities++;

	NSVector3df pos = NSVector3df( data->x, data->y, data->z );

	// create our node
	this->m_pNode = new NSNode( buff, pos );

	// check if its a spherical world
	this->m_bFlatWorld = *(bool*)CWorldManager::Get()->PostMessage(MSGID_IS_FLATWORLD, NULL);
	if ( ! this->m_bFlatWorld )
	{
		pos.Normalise();

		// Re-orient the city to face outward

		// get the cross vector between up and its position to rotate around
		NSVector3df upvec = NSVector3df( 0.0f, 1.0f, 0.0f );
		NSVector3df rotvec = upvec.CrossProduct( pos );
		// rotvec is facing left

		// now we need to get the angle between the two facing and up vectors

		float hyp = pow(pos.v.x, 2.0f) + pow(pos.v.z, 2.0f);
		hyp = sqrt(hyp);

		// get the polar co-ordinates of Y / hyp
		float theta = atan(  ( pos.v.y / hyp )  );

		theta -= NS_HALF_PI;

		// rotate the city around the rotation axis
		NSMatrix4 * mat = this->m_pNode->GetMatrix();
		mat->SetRotationAboutAxis( rotvec, -theta );
	}



	// add a random rotation
	this->m_pNode->RotateY(  ( (float)rand() / (float)RAND_MAX ) * NS_TWO_PI  );

	this->m_pCityBranch->AttachChild( this->m_pNode );

	this->m_eAlliance = data->alliance;


	// create and attach a mesh
	int rnum = (int)( ( (float)rand() / (float)RAND_MAX ) * 4.0f);

	CMeshManager::MESHES meshtype;
	if ( this->m_eAlliance == FRIENDLY )
	{
		this->m_fRadius = CITY_BOUNDINGRADIUS_FRIENDLY;

		if ( rnum <= 1 )
			meshtype = CMeshManager::CITY_FRIEND_1;
		else if ( rnum == 2 )
			meshtype = CMeshManager::CITY_FRIEND_2;
		else
			meshtype = CMeshManager::CITY_FRIEND_3;
	}
	else
	{
		this->m_fRadius = CITY_BOUNDINGRADIUS_ENEMY;

		if ( rnum <= 1 )
			meshtype = CMeshManager::CITY_ENEMY_1;
		else if ( rnum == 2 )
			meshtype = CMeshManager::CITY_ENEMY_2;
		else
			meshtype = CMeshManager::CITY_ENEMY_3;
	}

	NSElement * mesh = (NSElement*) CMeshManager::Get()->PostMessage(MSGID_GET_MESH, &meshtype);

	this->m_pNode->AttachMesh( mesh );

	this->m_pNode->Update();
}


void CCity::Destroy()
{
	this->m_pCityBranch->DetachChild( this->m_pNode );
	delete this->m_pNode;

	// dont kill our particle system since the manager will kill that for us
	
	this->m_iNumCities--;
}


void CCity::Update(void * Data)
{
	// if were alive we dont need to do anything
	if ( this->m_bAlive )
		return;

	float fTimeDelta = *(float*)Data;

	// otherwise weve been shot
	// move the city down into the ground
	NSVector3df trans;
	trans = this->m_kOriginalPosition;
	float dx, dz;

	if ( this->m_bFlatWorld )
	{
		// move the city straight down
		trans.v.y -= ( this->s_fCityFallSpeed * fTimeDelta );
		this->m_kOriginalPosition = trans;	// update our falling position

		// move it side to side with some jitter
		dx = ( (float)rand() / (float)RAND_MAX ) * this->s_fCityFallMaxJitter;
		dz = ( (float)rand() / (float)RAND_MAX ) * this->s_fCityFallMaxJitter;

		// invert some of the velocity
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			dx *= -1;
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			dz *= -1;

		trans.v.x += dx;
		trans.v.z += dz;
	}
	else
	{
		// move the city in by its translation
		NSVector3df fallvec;
		fallvec = trans;
		fallvec.Invert();
		fallvec.SetLength( this->s_fCityFallSpeed * fTimeDelta );

		// get a side vector
		NSVector3df upvec = NSVector3df( 0.0f, 1.0f, 0.0f );
		NSVector3df sidevec = upvec.CrossProduct( fallvec );

		// get a forward vector
		NSVector3df forwardvec = sidevec.CrossProduct( sidevec );

		// move it side to side with some jitter
		dx = ( (float)rand() / (float)RAND_MAX ) * this->s_fCityFallMaxJitter;
		dz = ( (float)rand() / (float)RAND_MAX ) * this->s_fCityFallMaxJitter;

		// invert some of the velocity
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			dx *= -1;
		if (  ( (float)rand() / (float)RAND_MAX ) * 1.0f  > 0.5f )
			dz *= -1;

		sidevec.SetLength( dx );
		forwardvec.SetLength( dz );

		// add the components
		trans += fallvec;
		this->m_kOriginalPosition += fallvec;	// update our falling position
		trans += sidevec;
		trans += forwardvec;
	}

	this->m_pNode->SetTranslate( trans );

	// check if our particle system is done, if so, remove ourselves
	if ( this->m_pParticleSystem == NULL || this->m_pParticleSystem->IsFinished() == true )
	{
		this->m_bFinished = true;
		this->m_pCityBranch->DetachChild( this->m_pNode );
	}
}


void CCity::ApplyDamage( int amount )
{
	// damage our city
	this->m_iHealth -= amount;

	// if weve lost all of our health and we havent died previously
	if ( this->m_iHealth <= CITY_HEALTH_MIN  &&  this->m_bAlive )
	{
		// explode
		this->m_bAlive = false;

		// create a particle system
		CParticleManager::NewSystemData data;
		data.trans = *this->m_pNode->GetWorldTranslation();
		//data.velocity = NSVector3df( 0.0f, 10.0f, 0.0f );
		data.type = CParticleManager::CITY_EXPLOSION;
		data.radius = 30.0f;

		this->m_pParticleSystem = (CityExplosionPSystem*)CParticleManager::Get()->PostMessage( MSGID_CREATE_PSYSTEM, &data );

		this->m_kOriginalPosition = *this->m_pNode->GetWorldTranslation();

		// we do this after weve made the city collapse now
		// remove our city node
		//this->m_pCityBranch->DetachChild( this->m_pNode );
		// this is done during the removal of the city
		//delete this->m_pNode;
	}
}

