/***************************************************************************
                          NSSphere.h  -  description
                             -------------------
    begin                : Wed Sept 29 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A sphere in 3d space

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSSPHERE_H
#define NSSPHERE_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"


namespace neurosis {


class NSSphere
{
	public:
		NSVector3df		m_pPosition;
		nsfloat			m_fRadius;

		NSSphere() {};
		NSSphere( NSVector3df position, nsfloat radius )
		{ this->m_pPosition = position; this->m_fRadius = radius; };
		~NSSphere()
		{};
};


}; // namespace neurosis


#endif // #ifndef NSSPHERE_H
