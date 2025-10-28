#include "NSCamera.h"

namespace neurosis {


NSCamera::NSCamera() : NSNode(  "camera", NSVector3df( 0.0f, 0.0f, 0.0f )  )
{
	this->m_kLookAt = NSVector3df( 0.0f, 0.0f, 1.0f );			// Look down the Z axis
	this->m_kOriginalLookAt = this->m_kLookAt;
}

NSCamera::~NSCamera()
{

}





///////////////////////////////
// Start Look At functions

void NSCamera::SetLookAt(const NSVertex3df &ver)
{
	this->SetLookAt( ver.x, ver.y, ver.z );
}

void NSCamera::SetLookAt(const NSVertex3df &ver, const NSVector3df &UpAxis)
{
	this->SetLookAt( ver.x, ver.y, ver.z );
	this->SetUpAxis( UpAxis.v.x, UpAxis.v.y, UpAxis.v.z  );
}


void NSCamera::SetLookAt(const NSVector3df &vec)
{
	this->SetLookAt( vec.v.x, vec.v.y, vec.v.z );
}

void NSCamera::SetLookAt(const NSVector3df &vec, const NSVector3df &UpAxis)
{
	this->SetLookAt( vec.v.x, vec.v.y, vec.v.z );
	this->SetUpAxis( UpAxis.v.x, UpAxis.v.y, UpAxis.v.z  );
}

void NSCamera::SetLookAt(nsfloat x, nsfloat y, nsfloat z)
{
	this->m_kLookAt.v.x = x;
	this->m_kLookAt.v.y = y;
	this->m_kLookAt.v.z = z;

	this->m_kOriginalLookAt.v.x = x;
	this->m_kOriginalLookAt.v.y = y;
	this->m_kOriginalLookAt.v.z = z;
}

void NSCamera::SetLookAt(nsfloat x, nsfloat y, nsfloat z, NSVector3df UpAxis)
{
	this->SetLookAt( x, y, z );
	this->SetUpAxis( UpAxis.v.x, UpAxis.v.y, UpAxis.v.z );
}











///////////////////////////////
// Start Get functions

const NSVector3df * NSCamera::GetLookAt()
{
	return &this->m_kLookAt;
}



}; // namespace neurosis
