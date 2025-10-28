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

//#ifdef NSNODE_H
//#	error NSNODE_H cross reference lock!
//#endif

#ifndef NSNODE_H
#define NSNODE_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"
#include "NSVertex3.h"
#include "NSMatrix4.h"
//#include "NSElement.h"
#include "NSMesh.h"
#include <stdio.h>


#define NSNODE_MAX_CHILDREN			100			// maximum number of child nodes per node


namespace neurosis {


class NSNode
{
	public:
		NSNode();
		NSNode( const char * name );
		NSNode( const char * name, const NSVector3df &translation );
		NSNode( const char * name, nsfloat tx, nsfloat ty, nsfloat tz );
		virtual ~NSNode();

		NSVector3df		GetFacingVector();
		NSVector3df		GetUpVector();
		NSVector3df		GetCrossVector();

		void			Translate( const NSVector3df &vec );
		void			Translate( nsfloat x, nsfloat y, nsfloat z );
		void			SetTranslate( const NSVertex3df &ver );
		void			SetTranslate( const NSVector3df &vec );
		void			SetTranslate( nsfloat x, nsfloat y, nsfloat z );
		const NSVector3df *	GetTranslation();

		void			TranslateForward( nsfloat amount );
		void			TranslateBackward( nsfloat amount );
		void			TranslateLeft( nsfloat amount );
		void			TranslateRight( nsfloat amount );
		void			TranslateUp( nsfloat amount );
		void			TranslateDown( nsfloat amount );

		void			RotateX( nsfloat amount );
		void			RotateY( nsfloat amount );
		void			RotateZ( nsfloat amount );
		void			SetRotationX( nsfloat amount );
		void			SetRotationY( nsfloat amount );
		void			SetRotationZ( nsfloat amount );


		void			SetUpAxis( const NSVector3df &vec);
		void			SetUpAxis( const NSVertex3df &ver);
		void			SetUpAxis( nsfloat x, nsfloat y, nsfloat z);
		const NSVector3df *	GetUpAxis();

		void			SetMatrix( NSMatrix4 &mat );
		NSMatrix4 *		GetMatrix();

		NSMatrix4 *		GetWorldMatrix();
		const NSVector3df * GetWorldTranslation();

		bool			AttachChild( NSNode * child );
		void			DetachChild( NSNode * child );
		void			KillChild( NSNode * child );

		void			DetachChildren();
		void			KillChildren();

		int				GetNumChildren();
		NSNode **		GetChildren();

		void			AttachMesh( NSElement * mesh );
		NSElement *		GetMesh();
		void			DetachMesh();

		void			SetVisibility( bool visibility );
		bool			GetVisibility() const;

		NSNode * const	GetParent();

		virtual void	Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation );
		virtual void	Update();

		/** Start Internal Use Only **/
		virtual void	_Update( NSMatrix4 &rotation, NSVector3df &translation );
		void			_NotifyOfParent( NSNode * parent );	// called to tell a node it has a parent
		void			_NotifyOfDetach( NSNode * child );	// called to tell a node it has lost a child
		void			_SetIsChildNum( int num );
		int				_GetIsChildNum();
		/** End Internal Use Only **/

	protected:
		char *			m_pName;

		bool			m_bVisible;

		NSMatrix4		m_kLocalMatrix;						// purely for rotation
		NSVector3df		m_kLocalTranslation;				// were not storing the translation in the matrix

		NSMatrix4		m_kWorldMatrix;
		NSVector3df		m_kWorldTranslation;

		NSVector3df		m_kFacingVector;					// our current forward vector
		NSVector3df		m_kUpAxis;							// our current up vector
		NSVector3df		m_kCross;							// or current side vector

		NSVector3df		m_kOriginalFacingVector;			// our original forward vector
		NSVector3df		m_kOriginalUpAxis;					// our original up vector
		NSVector3df		m_kOriginalCross;					// our original side vector

		NSNode *		m_pParent;							// our parent, if any
		NSNode *		m_pChildren[NSNODE_MAX_CHILDREN];	// our children
		int				m_iNumChildren;						// the number of children we have

		int				m_iIsChildNum;						// the number this node represents in its parents child array

		NSElement *		m_kMesh;							// our mesh, if one
};



}; // namespace neurosis


#endif // #ifndef NSNODE_H
