#include "WorldFlat.h"

void * CWorldFlat::PostMessage(int ID, void * Data)
{
	void * result = 0;

	CWorldFlat::WorldGetHeightAt getheightatData;
	CWorldFlat::WorldGetHeightAtFloat getheightatfloatData;
	float fResult;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init( Data );
			break;
		case MSGID_RENDER:
			this->Render();
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_CHECK_COLLISIONSSPHEREVSGROUND:
			this->CheckCollisionVsSphere( (CTask*)Data );
			break;

		case MSGID_GET_NUMROWS:
			result = &this->m_iMeshWidth;
			break;
		case MSGID_GET_NUMCOLS:
			result = &this->m_iMeshLength;
			break;

		case MSGID_GET_HEIGHTAT:
			getheightatData = *(CWorldFlat::WorldGetHeightAt*)Data;
			result = &this->m_pSystemVertexBuffer[ getheightatData.x * getheightatData.y ];
			break;

		case MSGID_GET_HEIGHTAT_FLOAT:
			getheightatfloatData = *(CWorldFlat::WorldGetHeightAtFloat*)Data;
			fResult = this->GetHighestPointAtCell( getheightatfloatData.x, getheightatfloatData.z );
			result = &fResult;
			break;

		case MSGID_GET_MINHEIGHT:
			result = &this->m_fMinHeight;
			break;


		case MSGID_GET_MOUNTAINLEVEL:
			result = &this->m_fMountainLevel;
			break;

		default:
			assert(0);
	}

	return result;
}




CWorldFlat::CWorldFlat()
{
}

CWorldFlat::~CWorldFlat()
{
}


void CWorldFlat::Init( void * Data )
{
	// Get our initialisation object
	CWorldFlat::WorldFlatSetup * data = (CWorldFlat::WorldFlatSetup*)Data;

	// Setup our mesh variables
	this->m_iMeshWidth = data->iMeshWidth;
	this->m_iMeshLength = data->iMeshLength;
	this->m_iMeshSpacing = data->iMeshSpacing;
	this->m_fMeshHeightScale = data->fMeshHeightScale;
	this->m_iTexSize = data->iTexSize;
	this->m_iAmplitude = data->iAmplitude;
	this->m_fRoughness = data->fRoughness;

	this->m_fIceOffset = 10.0f;
	this->m_fHeightOffset = 80.0f;
	this->m_fSeaLevel = 120.0f;// - this->m_fHeightOffset;
	this->m_fSandLevel = this->m_fSeaLevel + 5.0f;
	this->m_fIceLevel = 85.0f;// - this->m_fHeightOffset;
	this->m_fIceScale = 1.5f;
	this->m_fMountainLevel = 155.0f;// - this->m_fHeightOffset;
	this->m_fMountainPeakLevel = 165.0f;// - this->m_fHeightOffset;
	this->m_fMountainPeakScale = 0.3f;

	// get the max distance from the hypotenuse
	this->m_fMinCollisionDistance = (float)(	pow(this->m_iMeshSpacing * this->m_iMeshWidth, 2) +
												pow(this->m_iMeshSpacing * this->m_iMeshLength, 2)  );
	
	// Equation =           0.0f  +   scale       * (           (amp * 2)             + allow for random jitter     + this->m_fHeightOffset - this->m_fSeaLevel );
	// 5.0f * m_fMeshHeightScale is for the random jitter
	this->m_fMinHeight = this->m_fMeshHeightScale * ( ((float)this->m_iAmplitude * 2) + (5.0f * m_fMeshHeightScale) + this->m_fHeightOffset - this->m_fSeaLevel );

	this->m_pSystemVertexBuffer = NULL;
	this->m_pSystemIndexBuffer = NULL;

	// Create our height map
	// use a proper random seed
	srand( (unsigned)time(NULL) );//430919);
	//int vertSeed = (int)( (float)rand() / (float)RAND_MAX) * 1000;

	int lT, lB, rT, rB;
	lT = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	lB = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	rT = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	rB = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);

	this->m_kHeightMap = new Fractal( this->m_iAmplitude, this->m_fRoughness, this->m_iTexSize );
	m_kHeightMap->create(  lT, rT, lB, rB,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), (int)( ( (float)rand() / (float)RAND_MAX ) * 1000),//vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), (int)( ( (float)rand() / (float)RAND_MAX ) * 1000),//vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000)  );

	// gradient smooth our mesh
	int edgesize = (int)( (float)this->m_iTexSize / 8.0f );
	this->SmoothEdges( edgesize, (int)(this->m_fIceLevel - this->m_fHeightOffset) );//(int)(this->m_fIceLevel) );	// start just above the ice level so we only make water

	// create the texture from our height map
	this->CreateDXTexture();

	this->m_iNumVertices  = this->m_iMeshWidth * this->m_iMeshLength;
	this->m_iNumTriangles = (this->m_iMeshWidth - 1) * (this->m_iMeshLength - 1) * 2;
	this->m_iNumIndices = this->m_iNumTriangles * 3;

	// Create our mesh
	this->GenerateFlatMesh();



	// Check our capabilities for rendering
	const D3DCAPS9 caps = *(D3DCAPS9*)CUFOMain::Get()->PostMessage(MSGID_GET_CAPS, NULL);

	if ( caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR )
		this->m_bCapsMin = true;
	else
		this->m_bCapsMin = false;

	if ( caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR )
		this->m_bCapsMag = true;
	else
		this->m_bCapsMag = false;
}


void CWorldFlat::Destroy()
{
	if (this->m_pVertexBuffer != NULL)
		this->m_pVertexBuffer->Release();
	this->m_pVertexBuffer = NULL;

	if (this->m_pIndexBuffer != NULL)
		this->m_pIndexBuffer->Release();
	this->m_pIndexBuffer = NULL;

	//// dont clear the texture, the heightmap will do this
	// it doesnt own it anymore! so we have to
	if (this->m_pTexture != NULL)
		this->m_pTexture->Release();
	this->m_pTexture = NULL;

	delete this->m_kHeightMap;
	this->m_kHeightMap = NULL;

	delete [] this->m_pSystemVertexBuffer;
	delete [] this->m_pSystemIndexBuffer;
}


int CWorldFlat::GetVertexPointOnTexture( int x, int y )
{
	float widthscale = ((float)x) / ((float)this->m_iMeshWidth);
	float heightscale = ((float)y) / ((float)this->m_iMeshLength);

	int texx = (int)( ((float)this->m_kHeightMap->get_size()) * widthscale );
	int texy = (int)( ((float)this->m_kHeightMap->get_size()) * heightscale );

	int height = this->m_kHeightMap->get_point( texx, texy );
	return height;
}


bool CWorldFlat::GenerateFlatMesh()
{
	HRESULT hr = 0;

	this->m_iNumVertsPerRow = this->m_iMeshWidth;
	this->m_iNumVertsPerCol = this->m_iMeshLength;
	this->m_iNumCellsPerRow = this->m_iNumVertsPerRow - 1;
	this->m_iNumCellsPerCol = this->m_iNumVertsPerCol - 1;
	this->m_iWidth = this->m_iNumCellsPerRow * this->m_iMeshSpacing;

	float height, scale;
	float heightA, heightB, heightC;

	float h = 0.0f;			// water level is at y = 0.0f
	int d = this->m_iNumCellsPerCol * this->m_iMeshSpacing;
	scale = this->m_fMeshHeightScale;


	// Compute vertices
	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );
	hr = pd3dDevice->CreateVertexBuffer(	this->m_iNumVertices * sizeof( NSVertex3df ),
											D3DUSAGE_WRITEONLY, NSVertex3df::FVF,
											D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);

	// create our system side buffer
	this->m_pSystemVertexBuffer = new NSVector3df[ this->m_iNumVertices ];

	if (FAILED(hr))
		return false;

	// coordinates to end generating vertices at
	this->m_iEndX =  this->m_iWidth / 2;
	this->m_iEndZ = -this->m_iWidth / 2;

	// coordinates to start generating vertices at
	this->m_iStartX = -this->m_iWidth / 2;
	this->m_iStartZ =  this->m_iWidth / 2;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	float uCoordIncrementSize = 1.0f / (float)this->m_iNumVertsPerRow;
	float vCoordIncrementSize = 1.0f / (float)this->m_iNumVertsPerCol;

	NSVertex3df * v = 0;
	m_pVertexBuffer->Lock(0, 0, (void**)&v, 0);

	float random;

	int i = 0;
	for (int z = this->m_iStartZ; z >= this->m_iEndZ; z -= this->m_iMeshSpacing)
	{
		int j = 0;
		for (int x = this->m_iStartX; x <= this->m_iEndX; x += this->m_iMeshSpacing)
		{
			// compute the correct index into the vertex buffer and heightmap
			// based on where we are in the nested loop.
			int index = i * this->m_iNumVertsPerRow + j;
			height = (float)this->GetVertexPointOnTexture( j, i ) + this->m_fHeightOffset;


			// if were not affecting the water.. add a random jitter to mix the heights better
			if (height > this->m_fMountainLevel - 5.0f)
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * this->m_fMeshHeightScale;

				// add more jitter at the caps
				if ( height > this->m_fMountainPeakLevel - 5.0f )
					random *= 0.7f;
				else
					random *= 0.4f;

				// add / subtract randomly
				if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
					random *= -1;

				height += random;
			}

			D3DXVECTOR3 n;

			// if its sea level, dont adjust the height
			// allow for ice to have height
			if (height <= this->m_fSeaLevel  && height > this->m_fIceLevel)			// Sea
			{
				height = h;		// height is 0.0f
				n.x = 0.0f;		// normal is straight up
				n.y = 1.0f;
				n.z = 0.0f;
			}
			else
			{
				// if its ice, give it an offset since its already below the water
				if ( height < this->m_fSeaLevel)									// Ice
					height = h + (scale * this->m_fIceScale) * (this->m_fIceLevel - height);
					//height = h + scale * (height - this->m_fIceLevel) + this->m_fIceOffset;
				else																// Land
					height = h + scale * (height - this->m_fSeaLevel);

				// get the heights of the previous triangles
				heightA = (float)height;

				// Since we're adding random jitter, we need to access the previous vertices,
				// using the height map will be incorrect..
				// also, the original code from Ian Foley gave weird results on my mountains
				// this elegant solution fixes that
				// i = Y
				// j = X
				int tempindex;
				if ( i > 0 )
				{
					tempindex = (this->m_iMeshWidth * i) + (j - 1);
					heightB = this->m_pSystemVertexBuffer[ tempindex ].v.y;
				}
				else
					heightB = (float)height;

				if ( j > 0 )
				{
					tempindex = (this->m_iMeshWidth * (i - 1)) + (j);
					heightC = this->m_pSystemVertexBuffer[ tempindex].v.y;
				}
				else
					heightB = (float)height;

				// build two vectors on the quad
				D3DXVECTOR3 uu( (float)this->m_iMeshSpacing, heightB - heightA, 0.0f );
				D3DXVECTOR3 vv( 0.0f, heightC - heightA, -(float)this->m_iMeshSpacing );

				// find the normal by taking the cross product of two
				// vectors on the quad.
				D3DXVec3Cross(&n, &uu, &vv);
				D3DXVec3Normalize(&n, &n);
			}

			v[index] = NSVertex3df(	(float)x,(float)height,(float)z,
										n.x, n.y, n.z,
										(float)j * uCoordIncrementSize, (float)i * vCoordIncrementSize);

			// store our system vertex buffer
			this->m_pSystemVertexBuffer[index] = NSVector3df( (float)x, (float)height, (float)z );

			j++; // next column
		}
		i++; // next row
	}

	m_pVertexBuffer->Unlock();

	m_pSystemIndexBuffer = new DWORD[ this->m_iNumIndices ];

	// Compute indices
#ifdef INDEX_32BIT
		hr = pd3dDevice->CreateIndexBuffer( this->m_iNumIndices * sizeof(DWORD), // 3 indices per triangle
										D3DUSAGE_WRITEONLY, D3DFMT_INDEX32,
										D3DPOOL_MANAGED, &this->m_pIndexBuffer, NULL);

		if ( FAILED(hr) )
			return false;

		DWORD* indices = 0;
#else
		hr = pd3dDevice->CreateIndexBuffer( this->m_iNumIndices * sizeof(WORD), // 3 indices per triangle
										D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,
										D3DPOOL_MANAGED, &this->m_pIndexBuffer, NULL);
		if ( FAILED(hr) )
			return false;
		WORD* indices = 0;
#endif // #ifdef INDEX_32BIT

		m_pIndexBuffer->Lock(0, 0, (void**)&indices, 0);

			// index to start of a group of 6 indices that describe the
			// two triangles that make up a quad
		int baseIndex = 0;

			// loop through and compute the 2 triangles of each quad
		for (int i = 0; i < this->m_iNumCellsPerCol; i++)
		{
			for (int j = 0; j < this->m_iNumCellsPerRow; j++)
			{
				indices[baseIndex]     =   i   * this->m_iNumVertsPerRow + j;
				indices[baseIndex + 1] =   i   * this->m_iNumVertsPerRow + j + 1;
				indices[baseIndex + 2] = (i+1) * this->m_iNumVertsPerRow + j;

				indices[baseIndex + 3] = (i+1) * this->m_iNumVertsPerRow + j;
				indices[baseIndex + 4] =   i   * this->m_iNumVertsPerRow + j + 1;
				indices[baseIndex + 5] = (i+1) * this->m_iNumVertsPerRow + j + 1;

				// store our system index buffer
				m_pSystemIndexBuffer[baseIndex]	=		indices[baseIndex];
				m_pSystemIndexBuffer[baseIndex + 1]	=	indices[baseIndex + 1];
				m_pSystemIndexBuffer[baseIndex + 2]	=	indices[baseIndex + 2];

				m_pSystemIndexBuffer[baseIndex + 3]	=	indices[baseIndex + 3];
				m_pSystemIndexBuffer[baseIndex + 4]	=	indices[baseIndex + 4];
				m_pSystemIndexBuffer[baseIndex + 5]	=	indices[baseIndex + 5];

					// next quad
				baseIndex += 6;
			}
		}

		m_pIndexBuffer->Unlock();

	return true;
}




//void CWorldFlat::Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation )
void CWorldFlat::Render()
{
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	NSMatrix4 tempmat = NSMatrix4();
	tempmat.SetIdentity();
	tempmat.SetTranslation( 0.0f, 0.0f, 0.0f );

	D3DXMATRIXA16 mat;
	mat = *tempmat.GetDXMatrix();


	// set the translation
	device->SetTransform( D3DTS_WORLD, &mat );

	// render the world mesh

	// set our blending mode
	if (this->m_bCapsMag)
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	if (this->m_bCapsMin)
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// set the texture
	device->SetTexture( 0, this->m_pTexture );
	//device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	// render the mesh
	device->SetFVF( NSVertex3df::FVF );
	device->SetStreamSource( 0, this->m_pVertexBuffer, 0, sizeof(NSVertex3df) );
	device->SetIndices( this->m_pIndexBuffer );
	device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, this->m_iNumVertices, 0, this->m_iNumTriangles );

	// unset the texture
	device->SetTexture( 0, 0 );

	// unset the blending mode
	if (this->m_bCapsMag)
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	if (this->m_bCapsMin)
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
}




void CWorldFlat::SmoothEdges( int GradientSize, int GradientMax )
{
	int addHeight = (int)(this->m_fIceLevel - this->m_fHeightOffset + 1.0f);//0;
	int heightIncrement = GradientMax / GradientSize;
	int newHeight;
	int currHeight;
	float percentage;
	//int x2 = 0;


	for ( int y = 0; y < GradientSize; y++)
	{
		// fractal size = size + 1
		for ( int x = 0; x <= this->m_iTexSize; x++)
		{
			//// make the joins seamless
			//if ( x2 == this->m_iTexSize)
			//	x2 = 0;
			//else
			//	x2 = x;

			percentage = ( (float)y / (float)GradientSize );

			// North Edge
			// smooth the ice cap with the surrounding terrain
			currHeight = this->m_kHeightMap->get_point(x, y);

			// because of our colour function, if we drop just 1 value down from (IceLevel + 1),
			// we will see white, and it will look weird, so prevent ice from forming at the edges
			if (currHeight < addHeight)
				currHeight = addHeight;
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(x, y, newHeight);

			// South Edge
			currHeight = this->m_kHeightMap->get_point(x, this->m_iTexSize - y);
			if (currHeight < addHeight)
				currHeight = addHeight;
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(x, this->m_iTexSize - y, newHeight);


			// West Edge
			currHeight = this->m_kHeightMap->get_point(y, x);
			if (currHeight < addHeight)
				currHeight = addHeight;
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(y, x, newHeight);


			// East Edge
			currHeight = this->m_kHeightMap->get_point(this->m_iTexSize - y, x);
			if (currHeight < addHeight)
				currHeight = addHeight;
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(this->m_iTexSize - y, x, newHeight);
		}
//		addHeight += heightIncrement;
	}
}






bool CWorldFlat::CreateDXTexture()
{
	HRESULT hr = 0;

	int size = this->m_kHeightMap->get_size();

	// create an empty texture
	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );
	hr = D3DXCreateTexture( pd3dDevice, size, size, 0, 0,
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

	int r, g, b;
	int tempHeight;

	float random;

	// scale the height from 0 + heightoffset -> amplitude * 2 + heightoffset
	// to 0 + heightoffset -> 240 + heightoffset
	// this prevents modulus div zeros because i didnt take into account different heights when
	// i assigned the colour functions
	// a simple and reasonably elegant solution
	float heightscale;
	heightscale = 240.0f / ( (float)this->m_iAmplitude * 2.0f );

	// fractal is size + 1
	for (int x = 0; x <= size; x++)
	{
		for (int y = 0; y <= size; y++)
		{
			D3DXCOLOR colour;

			int height;
			height = this->m_kHeightMap->get_point(x, y);// + (int)m_fHeightOffset;

			// scale the height to between 0 + heightoffset -> amplitude * 2 + heightoffset
			height = (int)( (float)height * heightscale) + (int)this->m_fHeightOffset;

			// if were not affecting the water.. add a random jitter to mix the heights better
			if (height > this->m_fSeaLevel)
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 2.0f;

				//// add / subtract randomly
				//if (  ( (float)rand() / (float)RAND_MAX ) > 0.5f  )
				//	random *= -1;

				height += (int)random;
			}

			r = g = b = 0;

			// work out colours using a modulus..
			// this way we get smooth gradients instead of
			// blocky crap.. and its oh so much neater!
			if (height <= this->m_fSeaLevel/* + this->m_fHeightOffset*/)
			{
				// we dont jitter the water with a random amount or it looks crap

				// white ice, blue sea
				r = 255 - ( 3 * (255 % height) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % height) );
				b = 255 - ( 255 % height );
//				if (r > 160)
//					r = 160;
			}
			else if	(height >= this->m_fMountainLevel  &&  height < this->m_fMountainPeakLevel)
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
				height += (int)random;

				// grey mountains
				tempHeight = 180 - (height - (int)this->m_fMountainLevel);

				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % tempHeight) );
				b = 255 - ( 2 * (255 % tempHeight) );
			}
			else if	(height >= this->m_fMountainPeakLevel)
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
				height += (int)random;

				// snowy mountain peaks
				tempHeight = (int)( this->m_fMountainPeakScale * ( 170 - (int)( height - (int)this->m_fMountainPeakLevel ) )  );

				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % tempHeight) );
				b = 255 - ( 2 * (255 % tempHeight) );
			}
			else if (height < this->m_fSandLevel)
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
				height += (int)random;

				tempHeight = height + 10;

				r = (int)( 1.7f * (float)(255 % tempHeight) );
				g = (int)( 1.7f * (float)(255 % tempHeight) );
				b = ( (255 % tempHeight) );
			}
			else
			{
				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 10;
				height += (int)random;

				// green land
				tempHeight = height + 10;

				r = 256 % ( tempHeight );
				g = 256 % ( tempHeight );
				b = 128 % ( tempHeight + 10 ) / 4;
			}

			colour = D3DCOLOR_XRGB(r,g,b);

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
		MessageBox(0,_T("WorldFlat::CreateDXTexture - D3DXFilterTexture() Failed!"),0,0);
		return false;
	}


	return true;
}


int CWorldFlat::GetNumTilesToCheck( float radius )
{
	float num = radius / (float)this->m_iMeshSpacing;
	// add 1 because .9 will be rounded to 0
	// and we always want to check neighbours incase were on an edge
	return (int)(num + 1.0f);
}


bool CWorldFlat::GetCellPlayerIsAbove( float playerX, float playerZ, int &x, int &z )
{
	// make sure the player is within the map
	if ( playerX < (float) this->m_iStartX  ||  playerX > (float) this->m_iEndX )
		return false;
	// z is inverted.. hurray
	if ( playerZ > (float) this->m_iStartZ  ||  playerZ < (float) this->m_iEndZ )
		return false;


	// map starts extends from startX -> endX  && startZ -> endZ
	// map centre is at origin

	// cell width = this->m_iMeshSpacing
	int cellX = 0;
	int cellZ = 0;
	for ( int i = this->m_iStartZ; i >= this->m_iEndZ; i -= this->m_iMeshSpacing )
	{
		// check if the objects z (i) value is inside this cell
		if ( playerZ < i   &&   playerZ >= (i - this->m_iMeshSpacing) )
		{

			// if were within this row, then check for the correct column
			for ( int j = this->m_iStartX; j <= this->m_iEndX; j += this->m_iMeshSpacing )
			{
				// check if player is within grid, <= so we count the edge
				if ( playerX > j  &&  playerX <= (j + this->m_iMeshSpacing) )
				{
					z = cellZ;
					x = cellX;
					return true;
				}
				cellX++;
			}

		}
		cellZ++;
	}
		
	return false;
}



bool CWorldFlat::CheckCollisionWithCell( CTask * s, NSSphere &sp, int x, int z )
{
	// make sure were not checking out of bounds
	if ( x < 0  || x >= this->m_iNumCellsPerCol )
		return false;
	if ( z < 0  || z >= this->m_iNumCellsPerRow )
		return false;

	// Vertex Array allocation
	// z1,x1  z1,x2  z1,x3 ... z1,xN
	// z2,x1  z2,x2  z2,x3 ... z2,xN
	// zN,x1  zN,x2  zN,x3 ... zN,xN


	// Index Array allocation
	// tri1,1  tri1,2  tri1,3
	// tri2,1  tri2,2  tri2,3
	// triN,1  triN,2  triN,3


	NSVector3df v11, v12, v13, v21, v22, v23;

	// get the 2 triangles from the cell
	// v11, v21 and v13, v23 are duplicates
	int cell = (this->m_iNumCellsPerCol * (z * 6)) + (x * 6);
	v11 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell ]  ];
	v12 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 1 ]  ];
	v13 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 2 ]  ];

	v21 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 3 ]  ];
	v22 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 4 ]  ];
	v23 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 5 ]  ];


	////////////////////////////////
	// Triangle 1

	// create a plane for the first triangle
	NSVector3df planepos = v12;
	NSVector3df tv1 = v11 - v12;
	NSVector3df tv2 = v13 - v12;

	NSVector3df planenorm = tv2.CrossProduct( tv1 );
	planenorm.Normalise();

	NSPlane plane = NSPlane( planepos, planenorm );

	NSVector3df upvec = NSVector3df( 0.0f, 1.0f, 0.0f );
	NSVector3df v2v;

	NSVector3df trinorm = planenorm;		// used for collision response

	NSVector3df * velocity;

	// work out if the sphere intersects with the first triangles plane
	if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
	{
//		OutputDebugString( _T("Triangle1 Plane Collision\n") );

		// work out if the sphere is < the triangles 3 planes (its above the triangle)

		///////////////////////////////////
		// Triangle Side 1 (v11 -> v12)
		planepos = v11;
		// get a vector from v12 to v11
		v2v = v11 - v12;
		// cross it with the up vector to get our plane normal
		planenorm = upvec.CrossProduct( v2v );

		plane = NSPlane( planepos, planenorm );

		if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
		{
			///////////////////////////////////
			// Triangle Side 2 (v11 -> v13)
			planepos = v11;
			// get a vector from v13 to v11
			v2v = v11 - v13;
			// cross it with the up vector to get our plane normal
			planenorm = v2v.CrossProduct( upvec );

			plane = NSPlane( planepos, planenorm );

			if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
			{
				///////////////////////////////////
				// Triangle Side 3 (v12 -> v13)
				planepos = v12;
				// get a vector from v13 to v11
				v2v = v12 - v13;
				// cross it with the up vector to get our plane normal
				planenorm = upvec.CrossProduct( v2v );

				plane = NSPlane( planepos, planenorm );

				if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
				{
					// collision
					velocity = (NSVector3df*) s->PostMessage(MSGID_GET_VELOCITY, NULL);

					// apply damage
					// we can land on land, so well allow the disregarding of small damage values
					this->ApplyDamageToShip( s, trinorm, *velocity,
							WORLDFLAT_COLLISION_MINDAMAGE_LAND, WORLDFLAT_COLLISION_DAMAGESCALE_LAND );

					ReflectVector( trinorm, *velocity );

					this->DampenVelocity( velocity, trinorm );

					// tell the ship its colliding
					s->PostMessage( MSGID_SET_COLLIDING, NULL );

//					OutputDebugString( _T("Collision\n") );
					return true;
				}
			}
		}
	}


	////////////////////////////////
	// Triangle 2

	// create a plane for the second triangle
	planepos = v21;
	tv1 = v21 - v22;
	tv2 = v23 - v22;

	planenorm = tv2.CrossProduct( tv1 );
	planenorm.Normalise();

	plane = NSPlane( planepos, planenorm );

	trinorm = planenorm;		// used for collision response

	// work out if the sphere intersects with the first triangles plane
	if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
	{
//		OutputDebugString( _T("Triangle2 Plane Collision\n") );

		// work out if the sphere is < the triangles 3 planes (its above the triangle)

		///////////////////////////////////
		// Triangle Side 1 (v21 -> v22)
		planepos = v21;
		// get a vector from v22 to v21
		v2v = v21 - v22;
		// cross it with the up vector to get our plane normal
		planenorm = upvec.CrossProduct( v2v );

		plane = NSPlane( planepos, planenorm );

		if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
		{
			///////////////////////////////////
			// Triangle Side 2 (v21 -> v23)
			planepos = v23;
			// get a vector from v23 to v21
			v2v = v23 - v21;
			// cross it with the up vector to get our plane normal
			planenorm = upvec.CrossProduct( v2v );

			plane = NSPlane( planepos, planenorm );

			if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
			{
				///////////////////////////////////
				// Triangle Side 3 (v22 -> v23)
				planepos = v22;
				// get a vector from v23 to v22
				v2v = v22 - v23;
				// cross it with the up vector to get our plane normal
				planenorm = upvec.CrossProduct( v2v );

				plane = NSPlane( planepos, planenorm );

				if (  IntersectSpherePlane( sp, plane ) <= 1.0f  )
				{
					// collision
					velocity = (NSVector3df*) s->PostMessage(MSGID_GET_VELOCITY, NULL);

					// apply damage
					// we can land on land, so well allow the disregarding of small damage values
					this->ApplyDamageToShip( s, trinorm, *velocity,
							WORLDFLAT_COLLISION_MINDAMAGE_LAND, WORLDFLAT_COLLISION_DAMAGESCALE_LAND );

					ReflectVector( trinorm, *velocity );

					this->DampenVelocity( velocity, trinorm );

					// tell the ship its colliding
					s->PostMessage( MSGID_SET_COLLIDING, NULL );

//					OutputDebugString( _T("Collision\n") );
					return true;
				}
			}
		}
	}

	// no collision
	return false;
}

void CWorldFlat::ApplyDamageToShip( CTask * s,
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



void CWorldFlat::DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const
{
	// Apply friction
	float newvelocity = velocity->GetLength();
	newvelocity *= WORLDFLAT_COLLISION_FRICTIONSCALE;
	velocity->SetLength( newvelocity );

	// limit the vertical velocity return
	NSVector3df planenorm = normal;
	float dotvalue = planenorm.Dot( *velocity );

	// make the normals length the same height as the velocity
	planenorm.Scale( dotvalue );
	planenorm.Scale(   ( 1.0f - WORLDFLAT_COLLISION_VERTICALRETURNSCALE )   );

	*velocity -= planenorm;
}



bool CWorldFlat::CheckCollisionWithWater( CTask * s, NSSphere &sp )
{
	NSVector3df * velocity;

	NSPlane waterplane = NSPlane( NSVector3df(0.0f, 0.0f, 0.0f), NSVector3df(0.0f, 1.0f, 0.0f) );

	// if weve hit the water plane
	if (  IntersectSpherePlane( sp, waterplane ) <= 1.0f  )
	{
		// trigger a collision with the planes normal
		velocity = (NSVector3df*) s->PostMessage(MSGID_GET_VELOCITY, NULL);

		// apply damage
		// water will always do damage since we dont want to be able to land on it!
		this->ApplyDamageToShip( s, waterplane.m_pNormal, *velocity,
					WORLDFLAT_COLLISION_MINDAMAGE_WATER, WORLDFLAT_COLLISION_DAMAGESCALE_WATER );

		ReflectVector( waterplane.m_pNormal, *velocity );

		this->DampenVelocity( velocity, waterplane.m_pNormal );

		// tell the ship its colliding
		s->PostMessage( MSGID_SET_COLLIDING, NULL );

		return true;
	}

	return false;
}


void CWorldFlat::CheckCollisionVsSphere( CTask * s )
{
	// we use system side buffers to prevent copying data via the bus

	// create our variables here to save allocating new memory all the time

	// collidee variables
	NSSphere sp;
	float sradius;
	NSVector3df spos;

	spos = *( (NSNode*) s->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();
	sradius = *(float*) s->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	sp = NSSphere( spos, sradius );


	// dont bother continuing if the player is above our amplitude
	if (spos.v.y > this->m_fMinHeight)
		return;


	// Check collisions vs water
	if ( this->CheckCollisionWithWater( s, sp ) )
		return;


	// we need to do this constantly incase we get a collision with a sphere of
	// a different size
	// work out how many tiles we need to check using the sphere's radius
	int numTilesToCheck = this->GetNumTilesToCheck( sp.m_fRadius );


	// work out the tile which the sphere is above
	int x, z;
	// if the player is outside the map, return
	if ( ! this->GetCellPlayerIsAbove( spos.v.x, spos.v.z, x, z ) )
		return;


	// for each tile
	for ( int i = x - numTilesToCheck; i <= x + numTilesToCheck; i++ )
	{
		for ( int j = z - numTilesToCheck; j <= z + numTilesToCheck; j++ )
		{
			if ( this->CheckCollisionWithCell( s, sp, i, j ) )//x, z ) )
				return;
		}
	}

	// no collision
	return;
}


float CWorldFlat::GetHighestPointAtCell( float x, float z )
{
	// work out the cell the player is above
	int cellx = 0;
	int cellz = 0;

	float highest = 0.0f;

	if (  ! this->GetCellPlayerIsAbove( x, z, cellx, cellz )  )
		return highest;

	int cell = (this->m_iNumCellsPerCol * (cellz * 6)) + (cellx * 6);

	NSVector3df v;

	for ( int i = 0; i < 6; i++ )
	{
		v = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + i ]  ];

		if ( v.v.y > highest )
			highest = v.v.y;
	}

	return highest;
}


