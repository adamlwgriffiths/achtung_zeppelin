/***************************************************************************
                          NSCollision.h  -  description
                             -------------------
    begin                : Wed Sept 29 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : Defines some collision detection routines.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSCOLLISION_H
#define NSCOLLISION_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"
#include "NSMatrix4.h"
#include "NSPlane.h"
#include "NSSphere.h"


namespace neurosis {


nsfloat	IntersectSpherePlane( const NSSphere &s, const NSPlane &p );
//nsfloat	IntersectSphereBox( const NSSphere &s, const NSBox &b );
//nsfloat	IntersectBoxPlane( const NSBox &b, const NSPlane &p );
nsfloat	IntersectPointPlane( const NSVector3df &v, const NSPlane &p );

nsfloat	IntersectSphereSphere( const NSSphere &s1, const NSSphere &s2 );
//nsfloat	IntersectBoxBox( const NSBox &b1, const NSBox &b2 );

nsfloat IntersectSphereMatrixSphere( const NSSphere &s1, const NSSphere &s2, const NSMatrix4 &s2mat );

nsfloat IntersectMatrixSphereMatrixSphere(	const NSSphere &s1, const NSMatrix4 &s1mat,
											const NSSphere &s2, const NSMatrix4 &s2mat );


void	ReflectVector( const NSVector3df &normal, NSVector3df &vector );

}; // namespace neurosis


#endif // #ifndef NSCOLLISION_H
