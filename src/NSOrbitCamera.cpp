#include "NSOrbitCamera.h"

namespace neurosis {

NSOrbitCamera::NSOrbitCamera()
{
	this->m_pTarget = NULL;
	this->m_fRotation = 0.0f;
	this->m_fElevation = 0.0f;
	this->m_fZoom = 0.0f;
}

NSOrbitCamera::NSOrbitCamera( NSNode * pTarget )
{
	this->m_pTarget = NULL;
	//this->m_kLookAt = *pTarget->GetTranslation();
	this->m_kLookAt = NSVector3df( 0.0f, 0.0f, 0.0f );
	this->m_kOriginalLookAt = this->m_kLookAt;
//	this->m_pTarget = pTarget;
	this->m_fRotation = 0.0f;
	this->m_fElevation = 0.0f;
	this->m_fZoom = 0.0f;

//	pTarget->AttachChild( this );
	this->SetTarget( pTarget );
}

NSOrbitCamera::NSOrbitCamera( NSNode * pTarget, nsfloat fRotation, nsfloat fElevation, nsfloat fZoom )
{
	this->m_pTarget = NULL;
	//this->m_kLookAt = *pTarget->GetTranslation();
	this->m_kLookAt = NSVector3df( 0.0f, 0.0f, 0.0f );
	this->m_kOriginalLookAt = this->m_kLookAt;
//	this->m_pTarget = pTarget;
	this->m_fRotation = fRotation;
	this->m_fElevation = fElevation;
	this->m_fZoom = fZoom;

//	pTarget->AttachChild( this );
	this->SetTarget( pTarget );
}

NSOrbitCamera::~NSOrbitCamera()
{
	// detach from our target
	this->SetTarget( NULL );

	this->m_pTarget = NULL;
}

void NSOrbitCamera::Zoom( nsfloat fZoom )
{
	//if (this->m_pTarget == NULL)
	//	return;
	this->m_fZoom += fZoom;
}

void NSOrbitCamera::Elevate( nsfloat fElevation )
{
	//if (this->m_pTarget == NULL)
	//	return;
	this->m_fElevation += fElevation;
}

void NSOrbitCamera::Orbit( nsfloat fRotation )
{
//	if (this->m_pTarget == NULL)
//		return;
	this->m_fRotation += fRotation;
}

void NSOrbitCamera::SetZoom( nsfloat fZoom )
{
	//if (this->m_pTarget == NULL)
	//	return;
	this->m_fZoom = fZoom;
}

void NSOrbitCamera::SetElevation( nsfloat fElevation )
{
	//if (this->m_pTarget == NULL)
	//	return;
	this->m_fElevation = fElevation;
}

void NSOrbitCamera::SetRotation( nsfloat fRotation )
{
	//if (this->m_pTarget == NULL)
	//	return;
	this->m_fRotation = fRotation;
}

nsfloat NSOrbitCamera::GetZoom()
{
	return this->m_fZoom;
}

nsfloat NSOrbitCamera::GetElevation()
{
	return this->m_fElevation;
}

nsfloat NSOrbitCamera::GetRotation()
{
	return this->m_fRotation;
}



void NSOrbitCamera::SetTarget( NSNode * target )
{
	if (this->m_pTarget != NULL)
		this->m_pTarget->DetachChild( this );

	this->m_pTarget = target;

	if (this->m_pTarget != NULL)
		this->m_pTarget->AttachChild( this );
}

NSNode * NSOrbitCamera::GetTarget()
{
	return this->m_pTarget;
}


void NSOrbitCamera::Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation )
{
	// things shouldnt be attached to the camera
	// so i wont bother updating the matrix since i dont plan on attaching anything to it..
	// just incase.. well make them render anyway..
	// but we wont update the matrix since the look at makes it incorrect
//	NSMatrix4 currRot = rotation;
	//NSVector3df currTrans = this->m_kTranslation + translation;

	// adjust our current translation by the previous rotation, so children are relative to their parents
//	NSVector3df currTrans = translation + (rotation * this->m_kLocalTranslation);

	// rotate all of our vectors so that we keep i sync with our parent / target
//	this->m_kLocalTranslation.SetTranslation( currTrans );
//	this->m_kUpAxis.SetTranslation( rotation * this->m_kUpAxis );
//	this->m_kLookAt.SetTranslation( translation );


	// tell our children to render
	for (int i = 0; i < this->m_iNumChildren; i++)
	{
		// send our new rotation and translation to the children
//		this->m_pChildren[i]->Render( device, currRot, currTrans );
		this->m_pChildren[i]->Render( device, rotation, translation );
	}
}



void NSOrbitCamera::Update()
{
	if ( this->m_pTarget != NULL )
	{
		////////////////////////////////
		// Update the Look At Target
		//this->m_kLookAt = *this->m_pTarget->GetTranslation();
		// we dont update this anymore since the camera is local to the target

		////////////////////////////////
		// Update our Orbit Vector

		// we create a vector around the origin, and then once weve created it
		// we translate it to our targets position

		// create a vector of length m_fZoom along the Negative Z Axis
		NSVector3df pos = NSVector3df( 0.0f, 0.0f, -1.0f);
		pos.SetLength( this->m_fZoom );

		// rotate around X first, otherwise the Y rotation will screw this up
		// use our matrix so that we get our parent node's orientation
		NSMatrix4 mat;
		mat.SetIdentity();
		mat.SetRotationX( -this->m_fElevation );
		pos = mat * pos;

		// update our up vector
		// we need to make a new temporary vector based on our original UpVector
		// and then set our upaxis to its position or the vector will go nuts
		NSVector3df tempUpAxis = this->m_kOriginalUpAxis;
		tempUpAxis = mat * tempUpAxis;

		// rotate around the Y axis now
		mat.SetIdentity();
		mat.SetRotationY( this->m_fRotation );
		pos = mat * pos;

		// rotate our up vector again
		tempUpAxis = mat * tempUpAxis;
		this->m_kUpAxis = tempUpAxis;

		this->m_kLocalTranslation.SetTranslation( pos );
	}

	// update our children if we have any
	for (int i = 0; i < this->m_iNumChildren; i++)
		this->m_pChildren[i]->Update();
}


void NSOrbitCamera::_Update( NSMatrix4 &rotation, NSVector3df &translation )
{
//	this->Update();

	NSMatrix4 currRot = rotation;
//	NSVector3df currTrans = this->m_kTranslation + translation;

	// adjust our current translation by the previous rotation, so children are relative to their parents
	NSVector3df currTrans = translation + (rotation * this->m_kLocalTranslation);

	// rotate all of our vectors so that we keep i sync with our parent / target
	this->m_kWorldTranslation.SetTranslation( currTrans );
	this->m_kUpAxis.SetTranslation( rotation * this->m_kUpAxis );

	// we only do this so that the UFO Camera can use it as a cockpit camera
	this->m_kLookAt.SetTranslation( translation + rotation * this->m_kOriginalLookAt );

	// update our children if we have any
	for (int i = 0; i < this->m_iNumChildren; i++)
		this->m_pChildren[i]->_Update( rotation, translation );
}


}; // namespace neurosis
