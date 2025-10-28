#include "HeightMap.h"

void * CHeightMap::PostMessage(int ID, void * Data)
{
	void * result = 0;

	unsigned char tempheight;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init( Data );
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;
		case MSGID_GET_TEXTURE:
			result = m_pTexture;
			break;
		case MSGID_GENERATE_HEIGHTMAP:
			this->CreateNewHeightMap();
			break;
		case MSGID_GET_HEIGHTAT:
			tempheight = this->GetHeightAt( ( (CHeightMap::HeightAt*)Data )->x, ( (CHeightMap::HeightAt*)Data )->y );
			result = &tempheight;
			break;
		case MSGID_GET_HEIGHTDATA:
			result = &this->m_pData;
			break;

		default:
			assert(0);
	}

	return result;
}



CHeightMap::CHeightMap()
{
	// initialise random number generator
	srand( 10000 );

	// set default values
	//sprintf( this->m_cFilename, "heightmap.txt");
	this->m_iWidth = 128;
	this->m_iHeight = 64;
	this->m_iOctaves = 6;
	this->m_iSeaLevel = 120;
	this->m_fPersistence = 0.25;


//	this->m_iMinDelta = 480;
//	this->m_iMaxDelta = 512;
									
//	this->m_fFilter = 0.01f;
//	this->m_iWaterPercent = 64;
}

CHeightMap::~CHeightMap()
{
}



void CHeightMap::Init( void * Data )
{
	CHeightMap::HeightMapSetup data = *(HeightMapSetup*)Data;

	// store our width and height values
	if (data.iWidth > 0)
		this->m_iWidth = data.iWidth;
	if (data.iHeight > 0)
		this->m_iHeight = data.iHeight;
	if (data.iOctaves > 0)
		this->m_iOctaves = data.iOctaves;
	if (data.fPersistance > 0.0f)
		this->m_fPersistence = data.fPersistance;


	this->m_pData.iWidth = this->m_iWidth;
	this->m_pData.iHeight = this->m_iHeight;

	// initialise to NULL
	this->m_pData.data = NULL;
	this->m_pTexture = NULL;
	//this->m_fRandoms = NULL;
}

void CHeightMap::Destroy()
{
	if ( this->m_pData.data != NULL )
		this->UnloadHeightMap();

//	if ( this->m_fRandoms )
//		delete [] this->m_fRandoms;

    if( m_pTexture != NULL )
        m_pTexture->Release();
}




unsigned char CHeightMap::GetHeightAt( int x, int y )
{
	return this->m_pData.data[ (y * this->m_iWidth) + x ];
}



// we dont need these, so well forget them to save on maintenance

////////////////////////////////////////
//// Load a height map
//// This function borrowed from Ian Foley
////////////////////////////////////////
//void CHeightMap::LoadHeightMap()
//{
//	FILE* pFile;
//
//	// check to see if the data has been set
//	if ( this->m_pData.data )
//		UnloadHeightMap();
//
//	// open the RAW height map dataset
//	pFile= _tfopen( m_cFilename,_T("rb"));
//	if (pFile == NULL)
//	{
//		// bad filename
//		OutputDebugString( _T("CHeightMap::SaveHeightMap - Could not load file\n") );
//		return;
//	}
//
//	// allocate the memory for our height data
//	this->m_pData.data = new unsigned char[ this->m_iWidth * this->m_iHeight ];
//
//	// check to see if memory was successfully allocated
//	if (this->m_pData.data == NULL)
//	{
//		// the memory could not be allocated something is seriously wrong here
//		OutputDebugString( _T("CHeightMap::SaveHeightMap - Could not allocate memory for map load\n") );
//		return;
//	}
//
//	// read the heightmap into context
//	fread( this->m_pData.data, 1, this->m_iWidth * this->m_iHeight, pFile );
//	
//	// Close the file
//	fclose( pFile );
//
//	// The heightmap has been successfully loaded
//	OutputDebugString( _T("CHeightMap::SaveHeightMap - File loaded successfully\n") );
//	return;
//}
//
////////////////////////////////////////
//// Save a height map
//// This function borrowed from Ian Foley
////////////////////////////////////////
//void CHeightMap::SaveHeightMap()
//{
//	FILE* pFile;
//
//	// open a file to write to
//	pFile = _tfopen( m_cFilename,_T("wb"));
//
//	if( pFile == NULL )
//	{
//		// bad filename
//		OutputDebugString( _T("CHeightMap::SaveHeightMap - Could not create file\n") );
//		return;
//	}
//
//	// check to see if our height map actually has data in it
//	if( this->m_pData.data == NULL )
//	{
//		// something is seriously wrong here
//		OutputDebugString( _T("CHeightMap::SaveHeightMap - File empty\n") );
//		return;
//	}
//
//	// write the data to the file
//	fwrite( this->m_pData.data, 1, this->m_iWidth * this->m_iHeight, pFile );
//	
//	// Close the file
//	fclose( pFile );
//
//	// The heightmap has been successfully saved
//	OutputDebugString( _T("CHeightMap::SaveHeightMap - File saved\n") );
//}



void CHeightMap::UnloadHeightMap()
{
	// delete our height map if its been initialised
	if( this->m_pData.data )
	{
		//delete the data
		delete [] this->m_pData.data;
		this->m_pData.data = NULL;
	}
}



//////////////////////////////////////
// Pregenerate our random numbers so
// we dont get different numbers for
// each x,y point.
//////////////////////////////////////
//void CHeightMap::GenerateRandoms()
//{
//	// pregenerate our random numbers
//	if ( this->m_fRandoms )
//		delete [] this->m_fRandoms;
//
//	int buffersize = this->m_iWidth * this->m_iHeight;
//	this->m_fRandoms = new float [buffersize];
//
//	for ( int i = 0; i < buffersize; i++)
//	{
//		// create a random between 0.0f and  1.0f
//		this->m_fRandoms[i] = (   ( (float)rand() / RAND_MAX )   );
//	}
//}

//////////////////////////////////////
// Return the random number for this
// point
//////////////////////////////////////
float CHeightMap::Noise( float x, float y )
{
	//int x2, y2;
	//x2 = x;
	//y2 = y;

	//// make sure we dont exceed the array.. and at the same time make the land wrap around!
	//if (x2 >= this->m_iWidth)
	//	x2 -= this->m_iWidth;
	//if (y2 >= this->m_iHeight)
	//	y2 -= this->m_iHeight;

	//int pos = ( y2 * this->m_iWidth ) + x2;
	//return this->m_fRandoms[ pos ];

	float n = x + y * 57;
    n = pow( abs((n << 13)), abs(n) );
    return (float)( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);  
}

//////////////////////////////////////
// Interpolate between 2 values at
// a distance of dist
//////////////////////////////////////
float CHeightMap::Interpolate(float y1, float y2, float dist)
{
	// create a vector and scale it by dist to get the height value at that point
	// make difference based upon
	float endy = y2 - y1;
	NSVector3df vec = NSVector3df( 0.0f, endy, 0.0f );
	vec.SetLength(dist);
	// return + or -ve length
	return vec.v.y < 0 ? -vec.GetLength() : vec.GetLength();
}

//////////////////////////////////////
// Smooth a co-ordinate by the surrounding
// heights
//////////////////////////////////////
float CHeightMap::SmoothedNoise( int x, int y )
{
    float corners   = ( Noise( x-1, y-1) + Noise( x+1, y-1) + Noise( x-1, y+1) + Noise( x+1, y+1) ) / 16;
    float sides     = ( Noise( x-1, y)   + Noise( x+1, y)   + Noise( x, y-1)   + Noise( x, y+1) )   /  8;
    float center    =  Noise( x, y) / 4;

	return abs(corners + sides + center);
}

//////////////////////////////////////
// Create and interpolate the noise
// for this point
//////////////////////////////////////
float CHeightMap::InterpolatedNoise( float x, float y )
{
	int iX = (int)x;
	float fX = x - iX;

	int iY = (int)y;
	float fY = y - iY;

	float v1 = SmoothedNoise( iX,      iY);
	float v2 = SmoothedNoise( iX + 1,  iY);
	float v3 = SmoothedNoise( iX,      iY + 1);
	float v4 = SmoothedNoise( iX + 1,  iY + 1);

	float i1 = Interpolate( v1 , v2 , fX);
	float i2 = Interpolate( v3 , v4 , fX);

	return Interpolate( i1 , i2 , fY);
}


//////////////////////////////////////
// Generate each of the octave heights
// for this point and return their sum
//////////////////////////////////////
float CHeightMap::CreatePerlinNoise( int x, int y )
{
	// clear our data array
	for (int i = 0; i < this->m_iWidth * this->m_iHeight; i++)
		this->m_pData.data[i] = 0;

	float total = 0.0f;
	float freq = 0.0f;
	float amp = 0.0f;

	// for each point, create noise for each octave
	for ( int i = 0; i < this->m_iOctaves; i++ )
	{
		// set our new frequency and amplitude values
		freq = exp( (float)i );
		amp = pow( this->m_fPersistence, i);

		total += this->InterpolatedNoise( (float)x * freq, (float)y * freq) * amp;
	}

	// return the noise for this point
	return total;
}

//////////////////////////////////////
// Generate a new height map using the
// Perlin Noise algorithm
//////////////////////////////////////
bool CHeightMap::CreateNewHeightMap()
{
	//////////////////////
	// Setup

	int iBufferSize = this->m_iWidth * this->m_iHeight;

	if( m_pData.data )
		UnloadHeightMap();

	//allocate the memory for our height data
	m_pData.data = new unsigned char [ iBufferSize ];

	//check to see if memory was successfully allocated
	if ( m_pData.data == NULL )
	{
		//something is seriously wrong here
		OutputDebugString( _T("CHeightMap::CreateNewHeightMap - Could not allocate memory for height map") );
		return false;
	}

	// generate our random numbers
	//this->GenerateRandoms();


	//////////////////////
	// Generate Heightmap

	// Use twice the frequency and half the
	// amplitude for each successive noise
	// function added. 

//	m_fPersistence = 0.25f;		// 1/4 persistance
	float frequency = 0.0f;
	float amplitude = 0.0f;

	// for each point, create the noise
	for ( int x = 0; x < this->m_iWidth; x++ )
	{
		for ( int y = 0; y < this->m_iHeight; y++ )
		{
			// scale our 0.0f - 1.0f  floats to this char ( 0 - 255)
			this->m_pData.data[ (y * this->m_iWidth) + x ] = (unsigned char) (  255.0f * CreatePerlinNoise( x, y )  );
		}
	}

	// create a texture using this new height map
	this->CreateDXTexture();

	return true;
}


bool CHeightMap::CreateDXTexture()
{
	HRESULT hr = 0;

	// create an empty texture
	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );
	hr = D3DXCreateTexture( pd3dDevice, this->m_iWidth, this->m_iHeight, 0, 0,
							D3DFMT_X8R8G8B8, // 32 bit XRGB format
							D3DPOOL_MANAGED, &this->m_pTexture );

	if (FAILED(hr))
		return false;

	D3DSURFACE_DESC textureDesc;
	this->m_pTexture->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;

	D3DLOCKED_RECT lockedRect;
	this->m_pTexture->LockRect(0/*lock top surface*/, &lockedRect,
								0 /* lock entire tex*/, 0/*flags*/);
	DWORD * imageData = (DWORD*)lockedRect.pBits;

	//int r, g, b;

	for (int x = 0; x < this->m_iWidth; x++)
	{
		for (int y = 0; y < this->m_iHeight; y++)
		{
			D3DXCOLOR colour;

			int height;
			height = this->GetHeightAt(x, y);

			//r = g = b = 0;

			//if (height <= this->m_iSeaLevel)
			//{
			//	b = (int)exp( (float)height);
			//	if (b > 255)
			//	{
			//		g = (int)exp( (float)(b - 255) );
			//		b = 255;
			//	}
			//	if (g > 255)
			//	{
			//		r = (int)exp( (float)(g - 255) );
			//		g = 255;
			//	}
			//	if (r > 255)
			//		r = 255;
			//}


			if (height <= 0)
				colour = D3DCOLOR_XRGB(0,0,0);
			else if (height < 8)
				colour = D3DCOLOR_XRGB(0,0,68);
			else if (height < 16)
				colour = D3DCOLOR_XRGB(0,17,255);
			else if (height < 24)
				colour = D3DCOLOR_XRGB(0,51,255);
			else if (height < 32)
				colour = D3DCOLOR_XRGB(0,85,255);
			else if (height < 40)
				colour = D3DCOLOR_XRGB(0,119,255);
			else if (height < 48)
				colour = D3DCOLOR_XRGB(0,153,255);
			else if (height < 56)
				colour = D3DCOLOR_XRGB(0,204,255);
			else if (height < 64)
				colour = D3DCOLOR_XRGB(34,221,255);
			else if (height < 72)
				colour = D3DCOLOR_XRGB(68,238,255);
			else if (height < 80)
				colour = D3DCOLOR_XRGB(102,255,255);
			else if (height < 88)
				colour = D3DCOLOR_XRGB(119,255,255);
			else if (height < 96)
				colour = D3DCOLOR_XRGB(136,255,255);
			else if (height < 104)
				colour = D3DCOLOR_XRGB(153,255,255);
			else if (height < 112)
				colour = D3DCOLOR_XRGB(170,255,255);
			else if (height < 120)
				colour = D3DCOLOR_XRGB(187,255,255);
			else if (height < 128)	// Sea level is 120
				colour = D3DCOLOR_XRGB(0,68,0);
			else if (height < 136)
				colour = D3DCOLOR_XRGB(34,102,0);
			else if (height < 144)
				colour = D3DCOLOR_XRGB(34,136,0);
			else if (height < 152)
				colour = D3DCOLOR_XRGB(119,170,0);
			else if (height < 160)
				colour = D3DCOLOR_XRGB(187,221,0);
			else if (height < 168)
				colour = D3DCOLOR_XRGB(255,187,34);
			else if (height < 176)
				colour = D3DCOLOR_XRGB(238,170,34);
			else if (height < 184)
				colour = D3DCOLOR_XRGB(221,136,34);
			else if (height < 192)
				colour = D3DCOLOR_XRGB(204,136,34);
			else if (height < 200)
				colour = D3DCOLOR_XRGB(187,102,34);
			else if (height < 208)
				colour = D3DCOLOR_XRGB(170,85,34);
			else if (height < 216)
				colour = D3DCOLOR_XRGB(153,85,34);
			else if (height < 224)
				colour = D3DCOLOR_XRGB(136,68,34);
			else if (height < 232)
				colour = D3DCOLOR_XRGB(119,51,34);
			else if (height < 240)
				colour = D3DCOLOR_XRGB(85,51,17);
			else if (height < 248)
				colour = D3DCOLOR_XRGB(68,34,0);
			else if (height >= 248)
				colour = D3DCOLOR_XRGB(255,255,255);

			// fill locked data, note we divide the pitch by four because the
			// pitch is given in bytes and there are 4 bytes per DWORD.
			int pos = (y * lockedRect.Pitch / 4) + x;
			imageData[ pos ] = (D3DCOLOR)colour;
		}
	}

	this->m_pTexture->UnlockRect(0);

	hr = D3DXFilterTexture( this->m_pTexture, 0, 0, D3DX_DEFAULT );

	if (FAILED(hr))
	{
		MessageBox(0,_T("D3DXFilterTexture() - FAILED"),0,0);
		return false;
	}


	return true;
}
