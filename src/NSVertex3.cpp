#include "NSVertex3.h"

namespace neurosis
{

// D3DFVF_XYZ		= 0x002
// D3DFVF_NORMAL	= 0x010
// D3DFVF_DIFFUSE	= 0x040
// D3DFVF_TEX1		= 0x100
const unsigned long NSVertex3df::FVF = 0x002 | 0x010 | 0x100;

const unsigned long NSVertex3dfc::FVF = 0x002 | 0x040;


void NSVertex3dfc::SetColour( NSColourf &colour )
{
	this->m_kColour = D3DXCOLOR( 0.0f, colour.r, colour.g, colour.b );
	//D3DCOLOR_XRGB( colour.r, colour.g, colour.b );
}

void NSVertex3dfc::SetColour( NSColouraf &colour )
{
	this->m_kColour = D3DXCOLOR( colour.a, colour.r, colour.g, colour.b );
	//D3DCOLOR_XRGB( colour.r, colour.g, colour.b );
}

}; // namespace neurosis

