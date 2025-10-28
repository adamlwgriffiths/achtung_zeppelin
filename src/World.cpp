#include "World.h"



void CWorld::Init( void * Data )
{
	CWorld::WorldSetup * data = (CWorld::WorldSetup*)Data;

	this->m_fMeshHeightScale = data->fMeshHeightScale;
	this->m_iTexSize = data->iTexSize;
	this->m_iAmplitude = data->iAmplitude;
	this->m_fRoughness = data->fRoughness;

	this->m_fHeightOffset = 80.0f;
	this->m_fSeaLevel = 120.0f;
	this->m_fIceLevel = 85.0f;




	this->m_pSystemVertexBuffer = NULL;
	this->m_pSystemIndexBuffer = NULL;

	// Create our height map
	// use a proper random seed
	srand( (unsigned)time(NULL) );//430919);
	int vertSeed = (int)( (float)rand() / (float)RAND_MAX) * 1000;

	int lT, lB, rT, rB;
	lT = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	lB = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	rT = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	rB = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);

	this->m_kHeightMap = new Fractal( this->m_iAmplitude, this->m_fRoughness, this->m_iTexSize );
	m_kHeightMap->create(  lT, rT, lB, rB,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000)  );

	// create the texture from our height map
//	this->CreateDXTexture( data->pd3dDevice );
}



//bool CWorld::CreateDXTexture( LPDIRECT3DDEVICE9 pd3dDevice )
//{
//	HRESULT hr = 0;
//
//	int size = this->m_kHeightMap->get_size();
//
//	// create an empty texture
//	//LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );
//	hr = D3DXCreateTexture( pd3dDevice, size, size, 0, 0,
//							D3DFMT_X8R8G8B8, // 32 bit XRGB format
//							D3DPOOL_MANAGED, &this->m_pTexture );
//
//	if (FAILED(hr))
//		return false;
//
//	D3DSURFACE_DESC textureDesc;
//	this->m_pTexture->GetLevelDesc(0 /*level*/, &textureDesc);
//
//	// make sure we got the requested format because our code
//	// that fills the texture is hard coded to a 32 bit pixel depth.
//	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
//		return false;
//
//	D3DLOCKED_RECT lockedRect;
//	this->m_pTexture->LockRect(0/*lock top surface*/, &lockedRect,
//								0 /* lock entire tex*/, 0/*flags*/);
//	DWORD * imageData = (DWORD*)lockedRect.pBits;
//
//	int r, g, b;
//	int tempHeight;
//
//	// fractal is size + 1
//	for (int x = 0; x <= size; x++)
//	{
//		for (int y = 0; y <= size; y++)
//		{
//			D3DXCOLOR colour;
//
//			int height;
//			height = this->m_kHeightMap->get_point(x, y) + (int)m_fHeightOffset;
//
//			r = g = b = 0;
//
//			// work out colours using a modulus..
//			// this way we get smooth gradients instead of
//			// blocky crap.. and its oh so much neater!
//			if (height <= this->m_fSeaLevel + this->m_fHeightOffset)
//			{
//				// white ice, blue sea
//				r = 255 - ( 3 * (255 % height) );//(int)pow(2.0f, fheight);
//				g = 255 - ( 2 * (255 % height) );
//				b = 255 - ( 255 % height );
////				if (r > 160)
////					r = 160;
//			}
//			else if	(height >= this->m_fMountainLevel + this->m_fHeightOffset  &&  height < this->m_fMountainPeakLevel + this->m_fHeightOffset)
//			{
//				// grey mountains
//				tempHeight = 180 - (height - (int)this->m_fMountainLevel);
//
//				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
//				g = 255 - ( 2 * (255 % tempHeight) );
//				b = 255 - ( 2 * (255 % tempHeight) );
//			}
//			else if	(height >= this->m_fMountainPeakLevel + this->m_fHeightOffset)
//			{
//				// snowy mountain peaks
//				tempHeight = (int)( this->m_fMountainPeakScale * ( 170 - (int)( height - (int)this->m_fMountainPeakLevel ) )  );
//
//				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
//				g = 255 - ( 2 * (255 % tempHeight) );
//				b = 255 - ( 2 * (255 % tempHeight) );
//			}
//			else
//			{
//				// green land
//				tempHeight = height + 10;
//
//				r = 256 % ( tempHeight );
//				g = 256 % ( tempHeight );
//				b = 128 % ( tempHeight + 10 ) / 4;
//			}
//
//			colour = D3DCOLOR_XRGB(r,g,b);
//
//			// fill locked data, note we divide the pitch by four because the
//			// pitch is given in bytes and there are 4 bytes per DWORD.
//			int pos = (y * lockedRect.Pitch / 4) + x;
//			imageData[ pos ] = (D3DCOLOR)colour;
//		}
//	}
//
//	this->m_pTexture->UnlockRect(0);
//
//	hr = D3DXFilterTexture( this->m_pTexture, 0, 0, D3DX_DEFAULT );
//
//	if (FAILED(hr))
//	{
//		MessageBox(0,_T("WorldSphere::CreateDXTexture - D3DXFilterTexture() Failed!"),0,0);
//		return false;
//	}
//
//
//	return true;
//}




void CWorld::ApplyDamageToShip( CTask * s,
								NSVector3df &collisionNormal,
								NSVector3df &collisionVelocity,
								int lowestDamage, float damageScale )
{
	float mass;
	mass = *(float*)s->PostMessage(MSGID_GET_MASS, NULL);

	// apply damaged based upon the dot product of the collision
	float diffLength = abs(  collisionNormal.Dot( collisionVelocity)  );
	int damage = (int)(  damageScale * ( (diffLength * SHIP_COLLISION_VSGROUNDDAMAGESCALE) * mass )   );

	// dont apply the damage unless its > lowestDamage
	// this way we dont do damage on landing
	if ( damage > lowestDamage )
		s->PostMessage(MSGID_APPLY_DAMAGE, &damage );
}




void CWorld::DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const
{
	float newvelocity = velocity->GetLength();
	newvelocity *= WORLD_COLLISION_RETURNSPEEDSCALE;
	velocity->SetLength( newvelocity );
}


