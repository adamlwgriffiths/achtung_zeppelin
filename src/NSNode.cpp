#include "NSNode.h"

namespace neurosis {


NSNode::NSNode()
{
	char buff[] = "UnNamed";
	size_t strsize = strlen(buff);
	this->m_pName = new char[strsize + 1];
	for ( unsigned int i = 0; i < strsize; i++)
		this->m_pName[i] = buff[i];
	this->m_pName[strsize] = '\0';

	this->m_bVisible = true;
	this->m_iIsChildNum = -1;

	this->m_kLocalMatrix.SetIdentity();
	this->m_kLocalTranslation.SetTranslation( 0.0f, 0.0f, 0.0f );

	this->m_kWorldMatrix.SetIdentity();
	this->m_kWorldTranslation.SetTranslation( 0.0f, 0.0f, 0.0f );

	// create the original facing vector along the Z axis
	this->m_kFacingVector = NSVector3df( 0.0f, 0.0f, -1.0f);
	this->m_kOriginalFacingVector = this->m_kFacingVector;

	// create the up vector
	this->m_kUpAxis = NSVector3df( 0.0f, 1.0f, 0.0f);			// Y axis is up by default
	this->m_kOriginalUpAxis = this->m_kUpAxis;

	// create our cross vector
	this->m_kCross = this->m_kOriginalFacingVector.CrossProduct( this->m_kOriginalUpAxis );
	this->m_kOriginalCross = this->m_kCross;

	// set our parent and children to null
	this->m_pParent = NULL;

	for ( int i = 0; i < NSNODE_MAX_CHILDREN; i++)
		this->m_pChildren[i] = NULL;
	this->m_iNumChildren = 0;

	// initialise our mesh to null
	this->m_kMesh = NULL;
}

NSNode::NSNode( const char * name )
{
	//sprintf( this->m_pName, name );
	size_t strsize = strlen(name);
	this->m_pName = new char[strsize + 1];
	for ( unsigned int i = 0; i < strsize; i++)
		this->m_pName[i] = name[i];
	this->m_pName[strsize] = '\0';

	this->m_bVisible = true;
	this->m_iIsChildNum = -1;

	this->m_kLocalMatrix.SetIdentity();
	this->m_kLocalTranslation.SetTranslation( 0.0f, 0.0f, 0.0f );

	// create the original facing vector along the Z axis
	this->m_kFacingVector = NSVector3df( 0.0f, 0.0f, -1.0f);
	this->m_kOriginalFacingVector = this->m_kFacingVector;

	// create the up vector
	this->m_kUpAxis = NSVector3df( 0.0f, 1.0f, 0.0f);			// Y axis is up by default
	this->m_kOriginalUpAxis = this->m_kUpAxis;

	// create our cross vector
	this->m_kCross = this->m_kOriginalFacingVector.CrossProduct( this->m_kOriginalUpAxis );
	this->m_kOriginalCross = this->m_kCross;

	// set our parent and children to null
	this->m_pParent = NULL;

	for ( int i = 0; i < NSNODE_MAX_CHILDREN; i++)
		this->m_pChildren[i] = NULL;
	this->m_iNumChildren = 0;

	// initialise our mesh to null
	this->m_kMesh = NULL;
}

NSNode::NSNode( const char * name, const NSVector3df &translation )
{
	//sprintf( this->m_pName, name );
	size_t strsize = strlen(name);
	this->m_pName = new char[strsize + 1];
	for ( unsigned int i = 0; i < strsize; i++)
		this->m_pName[i] = name[i];
	this->m_pName[strsize] = '\0';

	this->m_bVisible = true;
	this->m_iIsChildNum = -1;

	this->m_kLocalMatrix.SetIdentity();
	this->m_kLocalTranslation.SetTranslation( translation );

	// create the original facing vector along the Z axis
	this->m_kFacingVector = NSVector3df( 0.0f, 0.0f, -1.0f);
	this->m_kOriginalFacingVector = this->m_kFacingVector;

	// create the up vector
	this->m_kUpAxis = NSVector3df( 0.0f, 1.0f, 0.0f);			// Y axis is up by default
	this->m_kOriginalUpAxis = this->m_kUpAxis;

	// create our cross vector
	this->m_kCross = this->m_kOriginalFacingVector.CrossProduct( this->m_kOriginalUpAxis );
	this->m_kOriginalCross = this->m_kCross;

	// set our parent and children to null
	this->m_pParent = NULL;

	for ( int i = 0; i < NSNODE_MAX_CHILDREN; i++)
		this->m_pChildren[i] = NULL;
	this->m_iNumChildren = 0;

	// initialise our mesh to null
	this->m_kMesh = NULL;
}

NSNode::NSNode( const char * name, nsfloat tx, nsfloat ty, nsfloat tz )
{
	//sprintf( this->m_pName, name );
	size_t strsize = strlen(name);
	this->m_pName = new char[strsize + 1];
	for ( unsigned int i = 0; i < strsize; i++)
		this->m_pName[i] = name[i];
	this->m_pName[strsize] = '\0';

	this->m_bVisible = true;
	this->m_iIsChildNum = -1;

	this->m_kLocalMatrix.SetIdentity();
	this->m_kLocalTranslation.SetTranslation( tx, ty, tz  );

	// create the original facing vector along the Z axis
	this->m_kFacingVector = NSVector3df( 0.0f, 0.0f, -1.0f);
	this->m_kOriginalFacingVector = this->m_kFacingVector;

	// create the up vector
	this->m_kUpAxis = NSVector3df( 0.0f, 1.0f, 0.0f);			// Y axis is up by default
	this->m_kOriginalUpAxis = this->m_kUpAxis;

	// create our cross vector
	this->m_kCross = this->m_kOriginalFacingVector.CrossProduct( this->m_kOriginalUpAxis );
	this->m_kOriginalCross = this->m_kCross;

	// set our parent and children to null
	this->m_pParent = NULL;

	for ( int i = 0; i < NSNODE_MAX_CHILDREN; i++)
		this->m_pChildren[i] = NULL;
	this->m_iNumChildren = 0;

	// initialise our mesh to null
	this->m_kMesh = NULL;
}

NSNode::~NSNode()
{
	// if weve got a child, kill them too
	// this could be dangerous if we have references to them from other places
	for ( int i = 0; i < this->m_iNumChildren; i++ )
	{
		this->m_pChildren[i]->_NotifyOfParent( NULL );
		delete this->m_pChildren[i];
		this->m_pChildren[i] = NULL;
	}

	if (this->m_pParent != NULL)
		this->m_pParent->DetachChild( this );
	this->m_pParent = NULL;

	// we dont delete our mesh incase another node is using it
	this->m_kMesh = NULL;

	delete [] this->m_pName;

	this->m_iNumChildren = 0;
}


///////////////////////////////
// Rotation Functions
void NSNode::RotateX( nsfloat amount )
{
	NSMatrix4 mat;
	mat.SetIdentity();
	mat.SetRotationX( amount );
	this->m_kLocalMatrix = mat * this->m_kLocalMatrix;
	this->Update();
}

void NSNode::RotateY( nsfloat amount )
{
	NSMatrix4 mat;
	mat.SetIdentity();
	mat.SetRotationY( amount );
	this->m_kLocalMatrix = mat * this->m_kLocalMatrix;
	this->Update();
}

void NSNode::RotateZ( nsfloat amount )
{
	NSMatrix4 mat;
	mat.SetIdentity();
	mat.SetRotationZ( amount );
	this->m_kLocalMatrix = mat * this->m_kLocalMatrix;
	this->Update();
}

void NSNode::SetRotationX( nsfloat amount )
{
	this->m_kLocalMatrix.SetRotationX( amount );
	this->Update();
}

void NSNode::SetRotationY( nsfloat amount )
{
	this->m_kLocalMatrix.SetRotationY( amount );
	this->Update();
}

void NSNode::SetRotationZ( nsfloat amount )
{
	this->m_kLocalMatrix.SetRotationZ( amount );
	this->Update();
}

void NSNode::SetMatrix( NSMatrix4 &mat )
{
	this->m_kLocalMatrix = mat;
}

NSMatrix4 * NSNode::GetMatrix()
{
	return &this->m_kLocalMatrix;
}




///////////////////////////////
// Translation functions
void NSNode::SetTranslate( const NSVertex3df &ver )
{
	this->m_kLocalTranslation = NSVector3df(ver.x, ver.x, ver.z);
}


void NSNode::SetTranslate( const NSVector3df &vec )
{
	this->m_kLocalTranslation = vec;
}


void NSNode::SetTranslate( nsfloat x, nsfloat y, nsfloat z )
{
	this->m_kLocalTranslation = NSVector3df(x, y, z);
}

void NSNode::Translate( const NSVector3df &vec )
{
	this->m_kLocalTranslation += vec;
}

void NSNode::Translate( nsfloat x, nsfloat y, nsfloat z )
{
	this->m_kLocalTranslation += NSVector3df( x, y, z );
}


const NSVector3df * NSNode::GetTranslation()
{
	return &m_kLocalTranslation;
}

NSVector3df	NSNode::GetFacingVector()
{
	return this->m_kFacingVector;
}

NSVector3df	NSNode::GetUpVector()
{
	return this->m_kUpAxis;
}

NSVector3df	NSNode::GetCrossVector()
{
	return this->m_kCross;
}





void NSNode::SetUpAxis(const NSVector3df &vec)
{
	this->SetUpAxis (vec.v.x, vec.v.y, vec.v.z);
}

void NSNode::SetUpAxis(const NSVertex3df &ver)
{
	this->SetUpAxis (ver.x, ver.y, ver.z);
}

void NSNode::SetUpAxis(nsfloat x, nsfloat y, nsfloat z)
{
	this->m_kOriginalUpAxis.v.x = x;
	this->m_kOriginalUpAxis.v.y = y;
	this->m_kOriginalUpAxis.v.z = z;

	// update our cross vector for the new up axis
	this->m_kOriginalCross = this->m_kOriginalFacingVector.CrossProduct( this->m_kOriginalUpAxis );
	this->m_kOriginalCross.Normalise();

	// update our facing vector for the new up axis
	this->m_kOriginalFacingVector = this->m_kUpAxis.CrossProduct( this->m_kOriginalCross );
	this->m_kOriginalFacingVector.Normalise();

	this->Update();
}


const NSVector3df * NSNode::GetUpAxis()
{
	return &this->m_kUpAxis;
}







void NSNode::TranslateForward( nsfloat amount )
{
	// get our facing vector
	NSVector3df tempvec = this->m_kFacingVector;
	// scale it to the length we want
	tempvec.SetLength( amount );
	// add it to our current position to go forward
	this->SetTranslate( *this->GetTranslation() + tempvec );
}

void NSNode::TranslateBackward( nsfloat amount )
{
	// get our facing vector
	NSVector3df tempvec = this->m_kFacingVector;
	// scale it to the length we want
	tempvec.SetLength( amount );
	// subract it from our current position to go backward
	this->SetTranslate( *this->GetTranslation() - tempvec );
}

void NSNode::TranslateLeft( nsfloat amount )
{
	// get our cross product
	NSVector3df tempfacing = this->m_kFacingVector;
	NSVector3df tempvec = this->m_kCross; //tempfacing.CrossProduct( this->m_kUpAxis );
	// scale it to the length we want
	tempvec.SetLength( amount );
	// subtract it from our current location to go left
	this->SetTranslate( *this->GetTranslation() + tempvec );
}

void NSNode::TranslateRight( nsfloat amount )
{
	// get our cross product
	NSVector3df tempfacing = this->m_kFacingVector;
	NSVector3df tempvec = this->m_kCross; //tempfacing.CrossProduct( this->m_kUpAxis );
	// scale it to the length we want
	tempvec.SetLength( amount );
	// subtract it from our current location to go left
	this->SetTranslate( *this->GetTranslation() - tempvec );
}

void NSNode::TranslateUp( nsfloat amount )
{
	// get our facing vector
	NSVector3df tempvec = this->m_kUpAxis;
	// scale it to the length we want
	tempvec.SetLength( amount );
	// add it to our current position to go upwards
	this->SetTranslate( *this->GetTranslation() + tempvec );
}

void NSNode::TranslateDown( nsfloat amount )
{
	// get our facing vector
	NSVector3df tempvec = this->m_kUpAxis;
	// scale it to the length we want
	tempvec.SetLength( amount );
	// subtract it from our current position to go downwards
	this->SetTranslate( *this->GetTranslation() - tempvec );
}



void NSNode::_SetIsChildNum( int num )
{
	this->m_iIsChildNum = num;
}

int NSNode::_GetIsChildNum()
{
	return this->m_iIsChildNum;
}

NSNode * const NSNode::GetParent()
{
	return this->m_pParent;
}


bool NSNode::AttachChild( NSNode * child )
{
	// make sure we dont go over our max child limit
	if (this->m_iNumChildren >= NSNODE_MAX_CHILDREN )
	{
		OutputDebugString( _T("NSNode::AttachChild - Maximum # Children Reached!!\n") );
		// the child may cause a memory leak if its not handled, but its better than crashing!
		return false;
	}

	// add the child
	this->m_pChildren[ this->m_iNumChildren ] = child;

	// notify our child it has a new parent
	this->m_pChildren[ this->m_iNumChildren ]->_NotifyOfParent( this );
	this->m_pChildren[ this->m_iNumChildren ]->_SetIsChildNum( this->m_iNumChildren );
	
	// increment our counter
	this->m_iNumChildren++;

	return true;
}

void NSNode::DetachChild( NSNode * child )
{
	int childnum = child->_GetIsChildNum();

	// make sure its one of our children
	if ( child->GetParent() != this )
		return;

	if ( childnum > -1  &&  childnum < this->m_iNumChildren )
	{
		// notify the node it no longer has a parent
		this->m_pChildren[ childnum ]->_NotifyOfParent( NULL );
		this->m_pChildren[ childnum ]->_SetIsChildNum( -1 );

		// clear our pointer and decrement our counter
		this->m_iNumChildren--;
		this->m_pChildren[ childnum ] = this->m_pChildren[ this->m_iNumChildren ];
		this->m_pChildren[ this->m_iNumChildren ] = NULL;

		// update the nodes number
		// dont update if the childnum = the number of children
		// which is 0 children, or if the moved child is the last, and therefore null now
		if ( childnum != this->m_iNumChildren )
			this->m_pChildren[ childnum ]->_SetIsChildNum( childnum );

		return;
	}

	//for (int i = 0; i < this->m_iNumChildren; i++)
	//{
	//	if ( this->m_pChildren[i] == child )
	//	{
	//		// notify the node it no longer has a parent
	//		this->m_pChildren[i]->_NotifyOfParent( NULL );

	//		// clear our pointer and decrement our counter
	//		this->m_pChildren[i] = this->m_pChildren[ this->m_iNumChildren - 1 ];
	//		this->m_pChildren[ this->m_iNumChildren - 1 ] = NULL;
	//		this->m_iNumChildren--;
	//		return;
	//	}
	//}
}

void NSNode::KillChild( NSNode * child )
{
	int childnum = child->_GetIsChildNum();

	// make sure its one of our children
	if ( child->GetParent() != this )
		return;

	if ( childnum > -1  &&  childnum < this->m_iNumChildren )
	{
		this->m_pChildren[ childnum ]->KillChildren();
		this->m_pChildren[ childnum ]->_NotifyOfParent( NULL );
		delete this->m_pChildren[ childnum ];

		// clear our pointer and decrement our counter
		this->m_iNumChildren--;
		this->m_pChildren[ childnum ] = this->m_pChildren[ this->m_iNumChildren ];
		this->m_pChildren[ this->m_iNumChildren ] = NULL;

		// update the nodes number
		// dont update if the childnum = the number of children
		// which is 0 children, or if the moved child is the last, and therefore null now
		if ( childnum != this->m_iNumChildren )
			this->m_pChildren[ childnum ]->_SetIsChildNum( childnum );

		return;
	}

	//for (int i = 0; i < this->m_iNumChildren; i++)
	//{
	//	if ( this->m_pChildren[i] == child )
	//	{
	//		// tell it to kill any nodes it has
	//		this->m_pChildren[i]->KillChildren();
	//		delete this->m_pChildren[i];

	//		// shuffle the array
	//		this->m_pChildren[i] = this->m_pChildren[ this->m_iNumChildren - 1 ];
	//		this->m_pChildren[ this->m_iNumChildren - 1 ] = NULL;

	//		// decrement our counter
	//		this->m_iNumChildren--;
	//		return;
	//	}
	//}
}


void NSNode::DetachChildren()
{
	for (int i = 0; i < this->m_iNumChildren; i++)
	{
		// notify the node it no longer has a parent
		this->m_pChildren[i]->_NotifyOfParent( NULL );
		this->m_pChildren[i]->_SetIsChildNum( -1 );

		// clear our pointer
		this->m_pChildren[i] = NULL;
	}
}

void NSNode::KillChildren()
{
	for (int i = 0; i < this->m_iNumChildren; i++)
	{
		// tell it to kill any nodes it has
		this->m_pChildren[i]->KillChildren();
		delete this->m_pChildren[i];

		// clear the position
		this->m_pChildren[i] = NULL;
	}
	this->m_iNumChildren = 0;
}



int NSNode::GetNumChildren()
{
	return this->m_iNumChildren;
}

NSNode ** NSNode::GetChildren()
{
	return this->m_pChildren;
}

void NSNode::_NotifyOfParent( NSNode * parent )
{
	// if we have a parent, detach ourselves from it to avoid crashes
//	if (this->m_pParent != NULL)
//		this->m_pParent->_NotifyOfDetach( this );

	this->m_pParent = parent;
}


void NSNode::_NotifyOfDetach( NSNode * child )
{
	int childnum = child->_GetIsChildNum();

	// make sure its one of our children
	if ( child->GetParent() != this )
		return;

	if ( childnum > -1  &&  childnum < this->m_iNumChildren )
	{
		// clear our pointer and decrement our counter
		this->m_iNumChildren--;
		this->m_pChildren[ childnum ] = this->m_pChildren[ this->m_iNumChildren ];
		this->m_pChildren[ this->m_iNumChildren ] = NULL;

		// update the nodes number
		// dont update if the childnum = the number of children
		// which is 0 children, or if the moved child is the last, and therefore null now
		if ( childnum != this->m_iNumChildren )
			this->m_pChildren[ childnum ]->_SetIsChildNum( childnum );
		
		return;
	}
	//for (int i = 0; i < this->m_iNumChildren; i++)
	//{
	//	if ( this->m_pChildren[i] == child )
	//	{
	//		// clear our pointer and decrement our counter
	//		this->m_pChildren[i] = this->m_pChildren[ this->m_iNumChildren - 1 ];
	//		this->m_pChildren[ this->m_iNumChildren - 1 ] = NULL;
	//		this->m_iNumChildren--;
	//		return;
	//	}
	//}
}





void NSNode::AttachMesh( NSElement * mesh )
{
	this->m_kMesh = mesh;
}

NSElement * NSNode::GetMesh()
{
	return this->m_kMesh;
}

void NSNode::DetachMesh()
{
	this->m_kMesh = NULL;
}



NSMatrix4 * NSNode::GetWorldMatrix()
{
	return &this->m_kWorldMatrix;
}

const NSVector3df * NSNode::GetWorldTranslation()
{
	return &this->m_kWorldTranslation;
}


void NSNode::SetVisibility( bool visibility )
{
	this->m_bVisible = visibility;
}

bool NSNode::GetVisibility() const
{
	return this->m_bVisible;
}



void NSNode::Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation )
{
	// the last two parameters dont mean anything anymore


	// if were invisible, our children are invisible too
	if ( !this->m_bVisible )
		return;

	// we dont directly affect the given parameters because if this is a child node, its
	// peers may require them _untouched_

	// add our rotation and translation to the above, we dont want to override the values we received
//	NSMatrix4 currRot = this->m_kLocalMatrix * rotation;
	// adjust our current translation by the previous rotation, so children are relative to their parents
//	NSVector3df currTrans = translation + (rotation * this->m_kLocalTranslation);

	// set the device to our current location
	// get the current rotation and add the translation to it then convert to a DX matrix
//	NSMatrix4 tempmat = currRot;
//	tempmat.SetTranslation( currTrans );

	NSMatrix4 tempmat = this->m_kWorldMatrix;
	tempmat.SetTranslation( this->m_kWorldTranslation );

	D3DXMATRIXA16 mat;
	mat = *tempmat.GetDXMatrix();


	// set the translation
	device->SetTransform( D3DTS_WORLD, &mat );

	// render this node
	if (this->m_kMesh != NULL)
		this->m_kMesh->Render( device );

	// tell our children to render
	// send our new rotation and translation to the children
	for (int i = 0; i < this->m_iNumChildren; i++)
	{
//		this->m_pChildren[i]->Render( device, currRot, currTrans );
		this->m_pChildren[i]->Render( device, rotation, translation );
	}
}



// called by our own functions
// take our current world and modify it according to any modifications weve made
void NSNode::Update()
{
	/////////////////////////////////
	// Update our Vectors

	// update our facing vector
	this->m_kFacingVector = this->m_kLocalMatrix * this->m_kOriginalFacingVector;

	// update our UpAxis vector
	this->m_kUpAxis = this->m_kLocalMatrix * this->m_kOriginalUpAxis;

	// update our Cross Vector
	this->m_kCross = this->m_kLocalMatrix * this->m_kOriginalCross;

	// update our children if we have any
	for (int i = 0; i < this->m_iNumChildren; i++)
		this->m_pChildren[i]->Update();
}


// called by parent nodes
// updates any children
void NSNode::_Update( NSMatrix4 &rotation, NSVector3df &translation )
{
//	this->Update();

	// add our rotation and translation to the above, we dont want to override the values we received
	NSMatrix4 currRot = this->m_kLocalMatrix * rotation;
	// adjust our current translation by the previous rotation, so children are relative to their parents
	NSVector3df currTrans = translation + (rotation * this->m_kLocalTranslation);

	// get the current rotation and add the translation
	this->m_kWorldMatrix = currRot;
	this->m_kWorldTranslation.SetTranslation( currTrans );

	// update our children if we have any
	for (int i = 0; i < this->m_iNumChildren; i++)
		this->m_pChildren[i]->_Update( currRot, currTrans );
}


}; // namespace neurosis
