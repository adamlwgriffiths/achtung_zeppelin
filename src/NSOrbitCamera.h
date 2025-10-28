/***************************************************************************
                          NSOrbitCamera.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A basic Orbit Camera class.
                           Over-rides a bit of functionality from NSCamera.
						   The Get / SetTranslate functions are relative
						   to the target to make calculation alot more simple.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

//#ifdef NSCAMERA_H
//#	error NSCAMERA_H cross reference lock!
//#endif

#ifndef NSORBITCAMERA_H
#define NSORBITCAMERA_H

#include "NeurosisPrerequisites.h"
#include "NSCamera.h"
#include "NSVector3.h"
#include "NSMatrix4.h"


namespace neurosis {


class NSOrbitCamera : public NSCamera
{
	public:
		NSOrbitCamera();
		NSOrbitCamera( NSNode * pTarget );
		NSOrbitCamera( NSNode * pTarget, nsfloat fRotation, nsfloat fElevation, nsfloat fZoom );
		~NSOrbitCamera();

		void			Zoom( nsfloat fZoom );
		void			Elevate( nsfloat fElevation );
		void			Orbit( nsfloat fRotation );

		void			SetZoom( nsfloat fZoom );
		void			SetElevation( nsfloat fElevation );
		void			SetRotation( nsfloat fRotation );

		nsfloat			GetZoom();
		nsfloat			GetElevation();
		nsfloat			GetRotation();

		void			SetTarget( NSNode * target );

		NSNode *		GetTarget();

		void			Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation );
		void			Update();

		void			_Update( NSMatrix4 &rotation, NSVector3df &translation );

	protected:
		NSNode *		m_pTarget;

		nsfloat			m_fRotation;
		nsfloat			m_fElevation;
		nsfloat			m_fZoom;
};


}; // namespace neurosis

#endif // #ifndef NSORBITCAMERA_H
