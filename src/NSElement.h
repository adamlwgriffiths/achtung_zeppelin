/***************************************************************************
                          NSElement.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A base class for Renderable objects.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

//#ifdef NSELEMENT_H
//#	error NSELEMENT_H cross reference lock!
//#endif

#ifndef NSELEMENT_H
#define NSELEMENT_H

#include "NeurosisPrerequisites.h"
//#	include <D3DX9.h>

namespace neurosis {


class NSElement
{
	public:
		NSElement() {};
		virtual ~NSElement() {};

		virtual bool	Load( LPDIRECT3DDEVICE9 device, char * filename ) = 0;
		virtual void	Unload() = 0;

		virtual void	Render( LPDIRECT3DDEVICE9 device ) = 0;
};


}; // namespace neurosis


#endif // #ifndef NSELEMENT_H
