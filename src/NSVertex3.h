/***************************************************************************
                          NSVertex3d.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A series of Vertice classes

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSVERTEX3D_H
#define NSVERTEX3D_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"
#include "NSColour.h"


namespace neurosis {



// this is a crappy DX version of my vertex struct
class NSVertex3df
{
	public:
		nsfloat x, y, z;					// Translation Vector
		nsfloat nx, ny, nz;					// Normal Vector
		nsfloat tu, tv;						// Texture co-ordinates

		static const unsigned long FVF;

		// Constructors
		NSVertex3df() {};
		NSVertex3df(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		{
			this->x = x; this->y = y; this->z = z;
			this->nx = nx; this->ny = ny; this->nz = nz;
			this->tu = u; this->tv = v;
		};

		inline NSVertex3df *	operator = (const NSVector3df &v2)
			{
				this->x = v2.v.x;
				this->y = v2.v.y;
				this->z = v2.v.z;
				return this;
			};
};


class NSVertex3dfc : public NSVertex3df
{
	public:
		nsfloat		x, y, z;					// Translation Vector
		D3DCOLOR	m_kColour;
//		nsfloat		nx, ny, nz;					// Normal Vector
//		nsfloat		tu, tv;

		void		SetColour( NSColourf &colour );
		void		SetColour( NSColouraf &colour );

		static const unsigned long FVF;
};


}; // namespace neurosis


#endif // #ifndef NSVERTEX_H
