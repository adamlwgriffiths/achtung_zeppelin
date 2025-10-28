/***************************************************************************
                        NSBillboard.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : Billboard class that extends NSElement

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSBILLBOARD_H
#define NSBILLBOARD_H

#include "NeurosisPrerequisites.h"
#include "NSElement.h"
#include "NSVertex3.h"

namespace neurosis {


class NSBillboard : public NSElement
{
	friend class NSBillboard;

	public:
		NSBillboard();
		~NSBillboard();

		bool			Load( LPDIRECT3DDEVICE9 device, char * filename );
		void			Unload();

		void			SetSize( float size );
		void			SetColour( NSColourf &colour );
		void			SetColour( NSColouraf &colour );

		void			Render( LPDIRECT3DDEVICE9 device );

	protected:
		LPDIRECT3DTEXTURE9			m_pTexture;
		LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer;

		float			m_fSize;
};


}; // namespace neurosis

#endif // #ifndef NSBILLBOARD_H
