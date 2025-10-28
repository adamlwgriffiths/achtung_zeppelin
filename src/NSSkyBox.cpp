#include "NSSkyBox.h"

namespace neurosis {


NSSkyBox::NSSkyBox()
{
}

NSSkyBox::NSSkyBox( const char * name ) : NSNode( name )
{
}

NSSkyBox::NSSkyBox( const char * name, const NSVector3df &translation ) : NSNode( name, translation )
{
}

NSSkyBox::NSSkyBox( const char * name, nsfloat tx, nsfloat ty, nsfloat tz ) : NSNode( name, tx, ty, tz )
{
}


void NSSkyBox::Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation )
{
	// if were invisible, our children are invisible too
	if ( !this->m_bVisible )
		return;

	// undo any rotations
	NSMatrix4 tempmat;// = this->m_kWorldMatrix;
	tempmat.SetIdentity();
	tempmat.SetTranslation( this->m_kWorldTranslation );

	D3DXMATRIXA16 mat;
	mat = *tempmat.GetDXMatrix();

	// set the translation
	device->SetTransform( D3DTS_WORLD, &mat );

	// disable the Z buffer
	device->SetRenderState( D3DRS_ZENABLE, false );

	// render this node
	if (this->m_kMesh != NULL)
		this->m_kMesh->Render( device );

	// reenable the z buffer
	device->SetRenderState( D3DRS_ZENABLE, true );

	// tell our children to render
	// send our new rotation and translation to the children
	for (int i = 0; i < this->m_iNumChildren; i++)
	{
		this->m_pChildren[i]->Render( device, rotation, translation );
	}
}




}; // namespace neurosis

