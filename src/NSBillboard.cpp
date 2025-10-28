#include "NSBillboard.h"

namespace neurosis
{

NSBillboard::NSBillboard()
{
	this->m_pTexture = NULL;
	this->m_pVertexBuffer = NULL;

	this->m_fSize = 1.0f;
}


NSBillboard::~NSBillboard()
{
//	this->Unload();		// dont unload manually
}

bool NSBillboard::Load( LPDIRECT3DDEVICE9 device, char * filename )
{
	//////////////////////////////////////
	// The following code was `borrowed'
	// from Frank Luna's particle system
	//////////////////////////////////////

	// vertex buffer's size does not equal the number of particles in our system.  We
	// use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
	// size we choose for the vertex buffer is specified by the _vbSize variable.

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
				sizeof( NSVertex3dfc ),
				D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
				NSVertex3dfc::FVF,
				D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
				&this->m_pVertexBuffer,
				0
			);
	
	if( FAILED(hr) )
	{
		MessageBox(0, "NSBillboard::Load - CreateVertexBuffer() - Failed", "Neurosis", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
				device,
				filename,
				&this->m_pTexture);

	if( FAILED(hr) )
	{
		MessageBox(0, "NSBillboard::Load - D3DXCreateTextureFromFile() - Failed", "Neurosis", 0);
		return false;
	}

	return true;
}


void NSBillboard::Unload()
{
	///////////////////////////////
	// Code taken from Frank Luna's particle system
	///////////////////////////////

	if( this->m_pTexture )
		this->m_pTexture->Release();
	if( this->m_pVertexBuffer )
		this->m_pVertexBuffer->Release();

	this->m_pTexture = NULL;
	this->m_pVertexBuffer = NULL;
}



void NSBillboard::Render( LPDIRECT3DDEVICE9 device )
{
	////////////////////////////////
	// Code taken from Frank Luna's particle system
	////////////////////////////////

	////////////////////////////
	// Setup the device

	float min = 0.0f;
	float max = 1.0f;

	// read, but dont write to the Z Buffer
	device->SetRenderState( D3DRS_ZWRITEENABLE,			false );
	device->SetRenderState( D3DRS_LIGHTING,				false );
	device->SetRenderState( D3DRS_POINTSPRITEENABLE,	true );
	device->SetRenderState( D3DRS_POINTSCALEENABLE,		true ); 
	device->SetRenderState( D3DRS_POINTSIZE,			*( (DWORD*)&this->m_fSize )  );
	device->SetRenderState( D3DRS_POINTSIZE_MIN,		*( (DWORD*)&min )  );

	// control the size of the particle relative to distance
	device->SetRenderState( D3DRS_POINTSCALE_A,			*( (DWORD*)&min ) );
	device->SetRenderState( D3DRS_POINTSCALE_B,			*( (DWORD*)&min ) );
	device->SetRenderState( D3DRS_POINTSCALE_C,			*( (DWORD*)&max ) );
		
	// use alpha from texture
	device->SetTextureStageState( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
	device->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1 );

	device->SetRenderState( D3DRS_ALPHABLENDENABLE,		true );
	device->SetRenderState( D3DRS_SRCBLEND,				D3DBLEND_SRCALPHA );
    device->SetRenderState( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );


	////////////////////////////
	// Render the billboard

	device->SetTexture( 0, this->m_pTexture );
	device->SetFVF( NSVertex3dfc::FVF );
	device->SetStreamSource( 0, this->m_pVertexBuffer, 0, sizeof( NSVertex3dfc ) );

	device->DrawPrimitive( D3DPT_POINTLIST, 0, 1 );


	////////////////////////////
	// Reset the device

	device->SetRenderState( D3DRS_ZWRITEENABLE,			true );
	device->SetRenderState( D3DRS_LIGHTING,				true );
	device->SetRenderState( D3DRS_POINTSPRITEENABLE,	false );
	device->SetRenderState( D3DRS_POINTSCALEENABLE,		false );
	device->SetRenderState( D3DRS_ALPHABLENDENABLE,		false );

	// unset the texture
	device->SetTexture( 0, 0 );
}


void NSBillboard::SetSize( float size )
{
	this->m_fSize = size;
}


void NSBillboard::SetColour( NSColourf &colour )
{
	NSVertex3dfc * vert = 0;

	this->m_pVertexBuffer->Lock(
				0,
				sizeof( NSVertex3dfc ),
				(void**)&vert,
				NULL//D3DLOCK_DISCARD//D3DLOCK_NOOVERWRITE
			);

	//vert->SetColour( colour );
	vert->m_kColour = D3DXCOLOR( 0.0f, colour.r, colour.g, colour.b );

	this->m_pVertexBuffer->Unlock();
}

void NSBillboard::SetColour( NSColouraf &colour )
{
	NSVertex3dfc * vert = 0;

	this->m_pVertexBuffer->Lock(
				0,
				sizeof( NSVertex3dfc ),
				(void**)&vert,
				NULL//D3DLOCK_DISCARD//D3DLOCK_NOOVERWRITE
			);

	//vert->SetColour( colour );
	vert->m_kColour = D3DXCOLOR( colour.a, colour.r, colour.g, colour.b );

	this->m_pVertexBuffer->Unlock();
}


}; // namespace neurosis
