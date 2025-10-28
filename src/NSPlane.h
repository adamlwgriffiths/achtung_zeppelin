/***************************************************************************
                          NSPlane.h  -  description
                             -------------------
    begin                : Wed Sept 29 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : An infinite plane in 3D space.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSPLANE_H
#define NSPLANE_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"


namespace neurosis {


class NSPlane
{
	public:
		NSVector3df		m_pPosition;
		NSVector3df		m_pNormal;

		NSPlane() {};
		NSPlane( NSVector3df position, NSVector3df normal )
		{ this->m_pPosition = position; this->m_pNormal = normal; };
		~NSPlane()
		{};
};


}; // namespace neurosis


#endif // #ifndef NSPLANE_H
