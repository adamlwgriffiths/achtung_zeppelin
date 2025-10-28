#include "NSLight.h"

namespace neurosis {


NSLight::NSLight( LPDIRECT3DDEVICE9 device, char * name, nsfloat tx, nsfloat ty, nsfloat tz, const NSColourf &colour, nsfloat range, nsfloat attenuation ) : NSNode(name, tx, ty, tz)
{
	ZeroMemory( &m_kLight, sizeof(D3DLIGHT9) );

	this->SetType( NSLIGHT_OMNI, type );

	// setup colours
	this->m_kLight.Diffuse.r  = colour.r;
	this->m_kLight.Diffuse.g  = colour.g;
	this->m_kLight.Diffuse.b  = colour.b;

	// set position
	this->m_kLight.Position = D3DXVECTOR3( tx, ty, tz);

	this->m_kLight.Range = range;
	this->m_kLight.Attenuation0 = attenuation;

	if ( NSLight::s_iNumLights > 8 )
		assert(0);

	this->m_iLightNum = NSLight::s_iNumLights;

	device->SetLight( this->m_iLightNum, &this->m_kLight );
	device->LightEnable( this->m_iLightNum, TRUE );
	device->SetRenderState( D3DRS_LIGHTING, TRUE );

	// increment our number of lights
	NSLight::s_iNumLights++;
}

NSLight::NSLight( LPDIRECT3DDEVICE9 device, char * name, nsfloat tx, nsfloat ty, nsfloat tz, NSLIGHT_TYPE type, const NSColourf &colour, nsfloat range, nsfloat attenuation ) : NSNode(name, tx, ty, tz)
{
	ZeroMemory( &m_kLight, sizeof(D3DLIGHT9) );

	this->SetType( device, type );

	// setup colours
	this->m_kLight.Diffuse.r  = colour.r;
	this->m_kLight.Diffuse.g  = colour.g;
	this->m_kLight.Diffuse.b  = colour.b;

	// set position
	this->m_kLight.Position = D3DXVECTOR3( tx, ty, tz);

	this->m_kLight.Range = range;
	this->m_kLight.Attenuation0 = attenuation;

	if ( NSLight::s_iNumLights > 8 )
		assert(0);

	this->m_iLightNum = NSLight::s_iNumLights;

	device->SetLight( this->m_iLightNum, &this->m_kLight );
	device->LightEnable( this->m_iLightNum, TRUE );
	device->SetRenderState( D3DRS_LIGHTING, TRUE );

	// increment our number of lights
	NSLight::s_iNumLights++;
}

NSLight::~NSLight()
{
	NSLight::s_iNumLights--;
}



void NSLight::SetColour( const NSColourf &colour )
{
}

NSColourf& NSLight::GetColour()
{
}


void NSLight::SetType( LPDIRECT3DDEVICE9 device, NSLight::LIGHT_TYPE type )
{
	this->m_eLightType = type;

	switch (type)
	{
		case NSLIGHT_AMBIENT:
			assert(0);
			// not implemented
			// hurray for directX and non standard lighting schemes
			break;

		case NSLIGHT_DIRECTIONAL:
			this->m_kLight.Type = D3DLIGHT_DIRECTIONAL;
			break;

		case NSLIGHT_SPOT:
			this->m_kLight.Type = D3DLIGHT_SPOT;
			assert(0);
			// not implemented
			break;

		case NSLIGHT_OMNI:
		default:
			this->m_kLight.Type = D3DLIGHT_POINT;
			break;
	}

	device->SetLight( this->m_iLightNum, &this->m_kLight );
}

NSLIGHT_TYPE NSLight::GetType()
{
	return this->m_eLightType;
}


void NSLight::SetDirection( NSVector3df & dir )
{
	this->m_kDirection = dir;
	this->m_kLight.Direction = D3DXVECTOR3( dir.v.x, dir.v.y, dir.v.z );
}

NSVector3df & NSLight::GetDirection()
{
	return this->m_kDirection;
}


void NSLight::Enable( LPDIRECT3DDEVICE9 device )
{
	device->LightEnable( this->m_iLightNum, TRUE );
}

void NSLight::Disable( LPDIRECT3DDEVICE9 device )
{
	device->LightEnable( this->m_iLightNum, FALSE );
}



void NSLight::_Update( NSMatrix4 &rotation, NSVector3df &translation )
{
//	this->Update();

	// add our rotation and translation to the above, we dont want to override the values we received
	NSMatrix4 currRot = this->m_kLocalMatrix * rotation;
	// adjust our current translation by the previous rotation, so children are relative to their parents
	NSVector3df currTrans = translation + (rotation * this->m_kLocalTranslation);

	// get the current rotation and add the translation
	this->m_kWorldMatrix = currRot;
	this->m_kWorldTranslation.SetTranslation( currTrans );

	// update our light
	this->m_kLight.Position = D3DXVECTOR3( m_kWorldTranslation.v.x, m_kWorldTranslation.v.y, m_kWorldTranslation.v.z );

	// update our children if we have any
	for (int i = 0; i < this->m_iNumChildren; i++)
		this->m_pChildren[i]->_Update( currRot, currTrans );
}



}; // namespace neurosis