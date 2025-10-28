/***************************************************************************
                          NSCamera.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A basic Camera class.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

//#ifdef NSCAMERA_H
//#	error NSCAMERA_H cross reference lock!
//#endif

#ifndef NSCAMERA_H
#define NSCAMERA_H

#include "NeurosisPrerequisites.h"
#include "NSNode.h"
#include "NSVertex3.h"
#include "NSVector3.h"


namespace neurosis {


class NSCamera : public NSNode
{
	public:
		NSCamera();
		virtual ~NSCamera();

		void			SetLookAt( const NSVertex3df &ver);
		void			SetLookAt( const NSVertex3df &ver, const NSVector3df &UpAxis);
		void			SetLookAt( const NSVector3df &vec);
		void			SetLookAt( const NSVector3df &vec, const NSVector3df &UpAxis);
		void			SetLookAt( nsfloat x, nsfloat y, nsfloat z);
		void			SetLookAt( nsfloat x, nsfloat y, nsfloat z, NSVector3df UpAxis);



		const NSVector3df *	GetLookAt();


	protected:
		NSVector3df		m_kLookAt;
		NSVector3df		m_kOriginalLookAt;
};


}; // namespace neurosis

#endif // #ifndef NSCAMERA_H
