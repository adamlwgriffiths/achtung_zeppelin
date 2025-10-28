/***************************************************************************
                          NSNode.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : An object with a 3d translation and rotation.
                           Provides a base class for all objects in 3D space.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

//#ifdef NSSKYBOX_H
//#	error NSSKYBOX_H cross reference lock!
//#endif

#ifndef NSSKYBOX_H
#define NSSKYBOX_H

#include "NeurosisPrerequisites.h"
#include "NSNode.h"



namespace neurosis {


class NSSkyBox : public NSNode
{
	public:
		NSSkyBox();
		NSSkyBox( const char * name );
		NSSkyBox( const char * name, const NSVector3df &translation );
		NSSkyBox( const char * name, nsfloat tx, nsfloat ty, nsfloat tz );
		//~NSSkyBox();

		virtual void	Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation );

	protected:

};



}; // namespace neurosis


#endif // #ifndef NSSKYBOX_H
