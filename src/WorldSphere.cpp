#include "WorldSphere.h"

void * CWorldSphere::PostMessage(int ID, void * Data)
{
	void * result = 0;

	CWorldFlat::WorldGetHeightAt getheightatData;
//	CWorldFlat::WorldGetHeightAtFloat getheightatfloatData;
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
			result = &this->m_iNumSlices;
			break;
		case MSGID_GET_NUMCOLS:
			result = &this->m_iNumStacks;
			break;

		case MSGID_GET_HEIGHTAT:
			getheightatData = *(CWorldFlat::WorldGetHeightAt*)Data;
			result = &this->m_pSystemVertexBuffer[ getheightatData.x * getheightatData.y ];
			break;

		case MSGID_GET_HEIGHTAT_FLOAT:
			// Hack since sphere isnt implemented in the final game anyway
			result = &this->m_fRadius;
			break;

		case MSGID_GET_MINHEIGHT:
			result = &this->m_fMinHeight;
			break;


		case MSGID_GET_RADIUS:
			result = &this->m_fRadius;
			break;

		case MSGID_GET_MOUNTAINLEVEL:
			fResult = this->m_fMountainLevel + this->m_fHeightOffset;
			result = &fResult;
			break;

		default:
			assert(0);
	}

	return result;
}




CWorldSphere::CWorldSphere()
{
}

CWorldSphere::~CWorldSphere()
{
}


void CWorldSphere::Init( void * Data )
{
	// Get our initialisation object
	CWorldSphere::WorldSphereSetup * data = (CWorldSphere::WorldSphereSetup*)Data;

	// Setup our mesh variables
	this->m_fRadius = data->fRadius;
	this->m_iNumSlices = data->iNumSlices;
	this->m_iNumStacks = data->iNumStacks;
	this->m_fMeshHeightScale = data->fMeshHeightScale;
	this->m_iTexSize = data->iTexSize;
	this->m_iAmplitude = data->iAmplitude;
	this->m_fRoughness = data->fRoughness;

	this->m_fHeightOffset = 80.0f;
	this->m_fSeaLevel = 120.0f - this->m_fHeightOffset;
	this->m_fSandLevel = this->m_fSeaLevel + 5.0f;
	this->m_fIceLevel = 85.0f - this->m_fHeightOffset;
	this->m_fIceScale = 1.5f;
	this->m_fMountainLevel = 155.0f - this->m_fHeightOffset;
	this->m_fMountainPeakLevel = 165.0f - this->m_fHeightOffset;
	this->m_fMountainPeakScale = 0.3f;

	this->m_fActualSeaLevel = this->m_fRadius;

	// 5.0f * m_fMeshHeightScale is for the random jitter
	this->m_fMinHeight = this->m_fRadius + (   this->m_fMeshHeightScale * (  ( (float)this->m_iAmplitude * 2 ) + (5.0f * m_fMeshHeightScale) - this->m_fSeaLevel  )  );

	this->m_pSystemVertexBuffer = NULL;
	this->m_pSystemIndexBuffer = NULL;

	// Create our height map
	// use a proper random seed
	srand( (unsigned)time(NULL) );//430919);
	int vertSeed = (int)( (float)rand() / (float)RAND_MAX) * 1000;

	int lT, lB;
	lT = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);
	lB = (int)( ( (float)rand() / (float)RAND_MAX ) * (float)this->m_iAmplitude);

	this->m_kHeightMap = new Fractal( this->m_iAmplitude, this->m_fRoughness, this->m_iTexSize );
	m_kHeightMap->create(  lT, lT, lB, lB,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000), vertSeed,
						(int)( ( (float)rand() / (float)RAND_MAX ) * 1000)  );

	// gradient smooth our mesh
	int polesize = (int)( (float)this->m_iTexSize / 8.0f );
	this->SmoothPoles( polesize, (int)(this->m_fIceLevel - this->m_fHeightOffset) );

	// create the texture from our height map
	this->CreateDXTexture();

	// Create our mesh
	this->GenerateSphericalMesh();


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


void CWorldSphere::Destroy()
{
	if (this->m_pWorldMesh != NULL)
		this->m_pWorldMesh->Release();
	this->m_pWorldMesh = NULL;

	if (this->m_pTexture != NULL)
		this->m_pTexture->Release();
	this->m_pTexture = NULL;

	delete this->m_kHeightMap;
	this->m_kHeightMap = NULL;

	delete [] this->m_pSystemVertexBuffer;
	delete [] this->m_pSystemIndexBuffer;
}


int CWorldSphere::GetVertexPointOnTexture( float x, float y )
{
	int texx = (int)(x * (float)this->m_iTexSize);
	int texy = (int)(y * (float)this->m_iTexSize);

	int height = this->m_kHeightMap->get_point( texx, texy );
	return height;
}


bool CWorldSphere::GenerateSphericalMesh()
{
		// Duplicated last vertex to avoid corruption at join
		// Need extra vertex at end in same position as starting one
		// so it is not shared.
	this->m_iNumVertices = (this->m_iNumSlices + 1) * (this->m_iNumStacks - 1) + 2 * this->m_iNumSlices;  // middle rings + 2*poles
	this->m_iNumTriangles = 2 * this->m_iNumSlices * (this->m_iNumStacks - 1);
	this->m_iNumIndices = this->m_iNumTriangles * 3;

	// compute the increment size of the texture coordinates
	// from one vertex to the next.
	double uInc = 1.0f / (double)this->m_iNumSlices;
	double vInc = 1.0f / (double)this->m_iNumStacks;

	double rr = (double)this->m_fRadius;
	double d;    // distance of point on surface from axis
	double dx;
	double dz;
	double h;    // vertical distance of point from equator
	double fDeltaRingAngle = (NS_PI / (double)this->m_iNumStacks);
	double fDelta  = (2.0f * NS_PI / (double)this->m_iNumSlices);
	int ring2 = this->m_iNumStacks / 2;
	int index;

	HRESULT hr = 0;

	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	/**
	*  Create an empty mesh to receive this->m_iNumTriangles triangles
	*  and this->m_iNumVertices vertices
	*/
	hr = D3DXCreateMeshFVF( this->m_iNumTriangles, this->m_iNumVertices,
							D3DXMESH_32BIT | D3DXMESH_MANAGED,
							NSVertex3df::FVF, pd3dDevice, &this->m_pWorldMesh);

	if( FAILED(hr))
	{
		MessageBox(0, _T("CWorldSphere::GenerateSphericalMesh - D3DXCreateMeshFVF() for this->m_pWorldMesh - FAILED"), 0, 0);
		return false;
	}

	this->m_pSystemVertexBuffer = new NSVector3df[ this->m_iNumVertices ];

	// Fill in vertices of the this->m_pWorldMesh
	NSVertex3df * v = 0;
	this->m_pWorldMesh->LockVertexBuffer(0, (void**)&v);

	// North and South poles
	// The north and south poles use an entire row ( m_iNumSlices) of vertices
	for (int slice = 0; slice < this->m_iNumSlices; slice++)
	{
		v[slice] = NSVertex3df(0.0f, this->m_fRadius, 0.0f, 0.0f, 1.0f, 0.0f, (float)slice * (float)uInc, 0.0f);
  		index = this->m_iNumStacks * (this->m_iNumSlices + 1) + slice - 1;
		v[index] = NSVertex3df(0.0f, -this->m_fRadius, 0.0f, 0.0f, -1.0f, 0.0f, (float)slice * (float)uInc, 1.0f);

		// make system copies
		this->m_pSystemVertexBuffer[ slice ] = NSVector3df( v[ slice ].x, v[ slice ].y, v[ slice ].z );
		this->m_pSystemVertexBuffer[ index ] = NSVector3df( v[ index ].x, v[ index ].y, v[ index ].z );
	}

	NSVector3df vecHeight;
	NSVertex3df vertex;
	float height = 0.0f;
	float tu, tv;

	float random;

//	this->m_fMeshSpacingVert = rr * sin( (double)(ring2 - ring) * fDeltaRingAngle );;

	for (int ring = 1; ring < this->m_iNumStacks; ring++)
	{
		// First tried an equal height interval strategy, but this resulted
		// in too big an interval at the poles giving the poles a pointed
		// end. An equal angle approach overcame this problem
		//h = 2.0f*(double)(ring2 - ring)*rr/(double)this->m_iNumStacks;
		h = rr * sin( (double)(ring2 - ring) * fDeltaRingAngle );   // Equal angle intervals
		d = sqrt(rr * rr - h * h); //r*sinf(ring*fDeltaRingAngle);

		for ( int slice = 0; slice <= this->m_iNumSlices; slice++ )
		{
			// compute the correct index into the vertex buffer
			// based on where we are in the nested loop.
			//index = (ring-1)*(this->m_iNumSlices+1) + slice + 1;
			index = ring * (this->m_iNumSlices + 1) + slice - 1;
			dx = d * sin(slice % this->m_iNumSlices * fDelta);
			dz = -d * cos(slice % this->m_iNumSlices * fDelta);

			// apply height
			tu = (float)slice * (float)uInc;
			tv = (float)ring * (float)vInc;
			height = (float)this->GetVertexPointOnTexture( tu, tv );


			// if were not affecting the water.. add a random jitter to mix the heights better
			if (height > this->m_fMountainLevel - 5.0f)
			{
				// dont add any randomness on the seems
				if ( slice != 0  &&  slice != this->m_iNumSlices )
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
			}



			// take into account water levels
			if (height <= this->m_fSeaLevel  &&  height > this->m_fIceLevel)		// Sea
				height = 0.0f;
			else if ( height < this->m_fSeaLevel)									// Ice
			{
				height = (this->m_fMeshHeightScale * this->m_fIceScale) * (this->m_fIceLevel - height);
			}
			else																	// Land
				height = this->m_fMeshHeightScale * (height - this->m_fSeaLevel);


			vecHeight = NSVector3df( (float)dx, (float)h, (float)dz );
			vecHeight.SetLength( height + this->m_fRadius );
			vertex = vecHeight;
			vertex.nx = 0.0f;
			vertex.ny = 0.0f;
			vertex.nz = 0.0f;
			vertex.tu = tu;
			vertex.tv = tv;

			v[index] = vertex;

			// make system copy
			this->m_pSystemVertexBuffer[index] = NSVector3df( vertex.x, vertex.y, vertex.z );
		}
	}
	this->m_pWorldMesh->UnlockVertexBuffer();




	/////////////////////////////////
	// Create our indices


	// Define the triangles of the box
	DWORD * i = 0;
	this->m_pWorldMesh->LockIndexBuffer( 0, (void**)&i );


	// create system copy
	this->m_pSystemIndexBuffer = new DWORD [ this->m_iNumIndices ];


	int j, k, l, m, n, p;

  	// fill in the north pole index data
	j = 0;
	k = this->m_iNumSlices - 1;
	l = 0;
	p = -1;
	while (j < 3 * this->m_iNumSlices)
	{
		p++;
		k++;
		l = k + 1;
		i[j] = p;
		i[j + 1] = l;
		i[j + 2] = k;

		this->m_pSystemIndexBuffer[ j ]		= i[ j ];
		this->m_pSystemIndexBuffer[ j + 1 ]	= i[ j + 1];
		this->m_pSystemIndexBuffer[ j + 2 ]	= i[ j + 2];

		j = j + 3;
	}
  		// fill in the middle index data
	k = 0;
	l = 0;
	m = 1;
	while (m < this->m_iNumStacks - 1)
	{
		k = p + 1;
		n = 0;
		while (n < this->m_iNumSlices)
		{
			l = k + 1;
			i[ j ] = k + (m - 1) * ( this->m_iNumSlices + 1);
			i[ j + 1 ] = l +(m - 1) * (this->m_iNumSlices + 1);
			i[ j + 2 ] = m * (this->m_iNumSlices + 1) + k;

			this->m_pSystemIndexBuffer[ j ]		= i[ j ];
			this->m_pSystemIndexBuffer[ j + 1 ]	= i[ j + 1];
			this->m_pSystemIndexBuffer[ j + 2 ]	= i[ j + 2];

			j = j + 3;
			k++;

			i[ j ] = k + (m - 1) * (this->m_iNumSlices + 1);
			i[ j + 1 ] = m * (this->m_iNumSlices + 1) + k;
			i[ j + 2 ] = m * (this->m_iNumSlices + 1) + k - 1;

			this->m_pSystemIndexBuffer[ j ]		= i[ j ];
			this->m_pSystemIndexBuffer[ j + 1 ]	= i[ j + 1];
			this->m_pSystemIndexBuffer[ j + 2 ]	= i[ j + 2];

			j = j + 3;
			n++;
		}
		m++;  // Increment ring
	}
  		// fill in the south pole index data
	k = (this->m_iNumSlices + 1) * (this->m_iNumStacks - 2) + this->m_iNumSlices - 1;
	m = this->m_iNumStacks - 1;
	n = 0;
	p = (this->m_iNumSlices + 1) * (this->m_iNumStacks - 1) + this->m_iNumSlices - 1;
	while ( n < this->m_iNumSlices )
	{
		k++;
		l = k + 1;
		p++;
		i[ j ] = k;
		i[ j + 1 ] = l;
		i[ j + 2 ] = p;

		this->m_pSystemIndexBuffer[ j ]		= i[ j ];
		this->m_pSystemIndexBuffer[ j + 1 ]	= i[ j + 1];
		this->m_pSystemIndexBuffer[ j + 2 ]	= i[ j + 2];

		j = j + 3;
		n++;
	}

	this->m_pWorldMesh->UnlockIndexBuffer();

	//
	// Specify the subset each triangle belongs to, in this example
	// we will use three subsets, the first two faces of the this->m_pWorldMesh specified
	// will be in subset 0, the next two faces will be in subset 1 and
	// the the last two faces will be in subset 2.
	//
	DWORD* attributeBuffer = 0;
	this->m_pWorldMesh->LockAttributeBuffer(0, &attributeBuffer);

	for (int a = 0; a < (int)this->m_iNumVertices; a++)
		attributeBuffer[a] = 0;

	this->m_pWorldMesh->UnlockAttributeBuffer();

	// Optimize the mesh to generate an attribute table.
	std::vector<DWORD> adjacencyInfo( this->m_pWorldMesh->GetNumFaces() * 3 );
	this->m_pWorldMesh->GenerateAdjacency( 0.001f, &adjacencyInfo[0] );

	// Compute normals
	D3DXComputeNormals( this->m_pWorldMesh, &adjacencyInfo[0] );

	hr = this->m_pWorldMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
								&adjacencyInfo[0], 0, 0, 0);

	return true;
}




//void CWorldSphere::Render( LPDIRECT3DDEVICE9 device, NSMatrix4 &rotation, NSVector3df &translation )
void CWorldSphere::Render()
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
	device->SetTexture(0, this->m_pTexture);
	//device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	// render the mesh
	this->m_pWorldMesh->DrawSubset(0);
	
	// unset the texture
	device->SetTexture(0,0);

	// unset the blending mode
	if (this->m_bCapsMag)
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	if (this->m_bCapsMin)
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
}



void CWorldSphere::SmoothPoles( int GradientSize, int GradientMax )
{
	int addHeight = 0;
	int heightIncrement = GradientMax / GradientSize;
	int newHeight;
	int currHeight;
	float percentage;
	int x2 = 0;

	for ( int y = 0; y < GradientSize; y++)
	{
		// fractal size = size + 1
		for ( int x = 0; x <= this->m_iTexSize; x++)
		{
			// make the joins seamless
			if ( x2 == this->m_iTexSize)
				x2 = 0;
			else
				x2 = x;

			percentage = ( (float)y / (float)GradientSize );
			//percentage = (exp(  ( (float)y / (float)GradientSize ) / 21.0f  ) - 1.0f);// * 100.0f;

			// North Pole
			// smooth the ice cap with the surrounding terrain
			currHeight = this->m_kHeightMap->get_point(x2, y);
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(x, y, newHeight);

			// South Pole
			currHeight = this->m_kHeightMap->get_point(x2, this->m_iTexSize - y);
			newHeight = (int)(    ( (float)addHeight * (1.0f - percentage) ) + ( (float)currHeight * percentage )    );

			this->m_kHeightMap->set_point(x, this->m_iTexSize - y, newHeight);
		}
//		addHeight += heightIncrement;
	}
}


bool CWorldSphere::CreateDXTexture()
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

			r = g = b = 0;

			// work out colours using a modulus..
			// this way we get smooth gradients instead of
			// blocky crap.. and its oh so much neater!
			if (height <= this->m_fSeaLevel + this->m_fHeightOffset)
			{
				// white ice, blue sea
				r = 255 - ( 3 * (255 % height) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % height) );
				b = 255 - ( 255 % height );
//				if (r > 160)
//					r = 160;
			}
			else if	(height >= this->m_fMountainLevel + this->m_fHeightOffset  &&  height < this->m_fMountainPeakLevel + this->m_fHeightOffset)
			{
				// grey mountains

				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
				height += (int)random;

				tempHeight = 180 - (height - (int)this->m_fMountainLevel);

				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % tempHeight) );
				b = 255 - ( 2 * (255 % tempHeight) );
			}
			else if	(height >= this->m_fMountainPeakLevel + this->m_fHeightOffset)
			{
				// snowy mountain peaks

				// add random jitter to the texture
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
				height += (int)random;

				tempHeight = (int)( this->m_fMountainPeakScale * ( 170 - (int)( height - (int)this->m_fMountainPeakLevel ) )  );

				r = 255 - ( 2 * (255 % tempHeight) );//(int)pow(2.0f, fheight);
				g = 255 - ( 2 * (255 % tempHeight) );
				b = 255 - ( 2 * (255 % tempHeight) );
			}
			else if (height < this->m_fSandLevel + this->m_fHeightOffset)
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
				random = ( (float)rand() / (float)RAND_MAX ) * 5;
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
		MessageBox(0,_T("WorldSphere::CreateDXTexture - D3DXFilterTexture() Failed!"),0,0);
		return false;
	}


	return true;
}

void CWorldSphere::GetCellPlayerIsAbove( NSSphere &sp, int &x, int &y )
{
	x = 0;
	y = 0;

	// Get the collidee's position
	NSVector3df spos = sp.m_pPosition;
	float interval;
	float currVal;
	float theta;

	///////////////////////////////
	// Work out the Y of the cell

	// get the length of the hypotenuse of x, z
	float hyp = pow(sp.m_pPosition.v.x, 2.0f) + pow(sp.m_pPosition.v.z, 2.0f);
	hyp = sqrt(hyp);

	theta = atan(  ( sp.m_pPosition.v.y / hyp )  );

	// turn theta so that 0 is upward
	theta -= NS_HALF_PI;

	// make sure theta isnt -ve
	theta = abs(theta);

	// work out the theta interval of the stacks
	interval = NS_PI / ( (float)this->m_iNumStacks);

	currVal = 0.0f;
	for ( int i = 0; i < this->m_iNumStacks; i++ )
	{
		// if were in this stack
		if ( theta >= currVal  &&  theta < (currVal + interval) )
		{
			y = i;
			break;
		}

		currVal += interval;
	}




	///////////////////////////////
	// Work out the X of the cell

	float collX, collY;

	collX = sp.m_pPosition.v.x;
	collY = sp.m_pPosition.v.z;

	// get the angle the collidee is at
	theta = atan(  ( collY / collX )  );

	if ( collX < 0.0f )
		theta += NS_PI;

	// theta = 0 is pointing right, and as it increases it goes clock wise (because -ve z is where +ve y would be)
	// we need theta 0 to point up, and as it increases, to turn clock wise
	theta += NS_HALF_PI;

	// make sure theta isnt -ve
	if (theta < 0.0f)
		theta += NS_TWO_PI;


	// reuse our interval variable
	interval = NS_TWO_PI / ( (float)this->m_iNumSlices);

	currVal = 0.0f;
	for ( int i = 0; i < this->m_iNumSlices + 1; i++ )
	{
		// if were in this stack
		if ( theta >= currVal  &&  theta < (currVal + interval) )
		{
			x = i;
			break;
		}

		currVal += interval;
	}
}


int CWorldSphere::GetNumTilesToCheck( NSSphere &sp,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 )
{
	// because spheres cells vary in size, we cannot use the typical algorithm here

	// we only need the first 3 vertices
	// we find the length of the shortest side and divide the radius by that as in WorldFlat
	NSVector3df tv1, tv2;

	tv1 = v11 - v12;
	tv2 = v13 - v12;

	float length1, length2, smallest;
	length1 = tv1.GetLength();
	length2 = tv2.GetLength();

	if (length2 < length1)
		smallest = length2;
	else
		smallest = length1;

	float num = sp.m_fRadius / (float)smallest;
	// add 1 because .9 will be rounded to 0
	// and we always want to check neighbours incase were on an edge
	return (int)(num + 1.0f);
}


void CWorldSphere::GetVerticesToCheck( int x, int y,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 )
{
	int cell;

	// Triangular top
	if ( y < 1 )
	{
		cell = 3 * x;			//( (this->m_iNumSlices) * (3 * y) ) + (3 * x);

		v11 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell ]  ];
		v12 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 1 ]  ];
		v13 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 2 ]  ];

		v21 = v11;
		v22 = v12;
		v23 = v13;
		return;
	}
	// Triangular base
	else if ( y > this->m_iNumStacks - 2 )
	{
		cell = ( (this->m_iNumSlices) * (3) );
		cell += ( (this->m_iNumSlices) * ( (this->m_iNumStacks - 2) * 6));

		cell += 3 * x;

		v11 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell ]  ];
		v12 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 1 ]  ];
		v13 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 2 ]  ];

		v21 = v11;
		v22 = v12;
		v23 = v13;
		return;
	}

	// Normal

	// First Row
	cell = 3 * this->m_iNumSlices;
	// Body
	cell += ( (this->m_iNumSlices) * ( (y - 1) * 6)) + (x * 6);

	v11 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell ]  ];
	v12 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 1 ]  ];
	v13 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 2 ]  ];

	v21 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 3 ]  ];
	v22 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 4 ]  ];
	v23 = this->m_pSystemVertexBuffer[  this->m_pSystemIndexBuffer[ cell + 5 ]  ];

	return;

}



bool CWorldSphere::CheckCollisionWithCell( CTask * s, NSSphere &sp,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 )
{

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
							WORLDSPHERE_COLLISION_MINDAMAGE_LAND, WORLDSPHERE_COLLISION_DAMAGESCALE_LAND );

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


	// make sure the second triangle isnt the same as the first
	// this happens at the poles
	if ( v11 == v21  &&  v12 == v22  &&  v13 == v23 )
		return false;


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
							WORLDSPHERE_COLLISION_MINDAMAGE_LAND, WORLDSPHERE_COLLISION_DAMAGESCALE_LAND );

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




void CWorldSphere::ApplyDamageToShip( CTask * s,
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




void CWorldSphere::DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const
{
	float newvelocity = velocity->GetLength();
	newvelocity *= WORLDSPHERE_COLLISION_FRICTIONSCALE;
	velocity->SetLength( newvelocity );

	// limit the vertical velocity return
	NSVector3df planenorm = normal;
	float dotvalue = planenorm.Dot( *velocity );

	// make the normals length the same height as the velocity
	planenorm.Scale( dotvalue );
	planenorm.Scale(   ( 1.0f - WORLDSPHERE_COLLISION_VERTICALRETURNSCALE )   );

	*velocity -= planenorm;
}




bool CWorldSphere::CheckCollisionWithWater( CTask * s, NSSphere &sp )
{
	NSVector3df * velocity;

	// use a plane so that we can do sphere tests
	NSVector3df waterpos = sp.m_pPosition;
	NSVector3df waternorm = sp.m_pPosition;
	waternorm.Normalise();
	waterpos.SetLength( this->m_fActualSeaLevel );

	NSPlane waterplane = NSPlane( waterpos, waternorm );

	if ( IntersectSpherePlane( sp, waterplane) <= 1.0f )
	{
		// Water collision
		velocity = (NSVector3df*) s->PostMessage(MSGID_GET_VELOCITY, NULL);

		// apply damage
		// water will always do damage since we dont want to be able to land on it!
		this->ApplyDamageToShip( s, waterplane.m_pNormal, *velocity,
					WORLDSPHERE_COLLISION_MINDAMAGE_WATER, WORLDSPHERE_COLLISION_DAMAGESCALE_WATER );

		// use the ships own translation as the normal
		ReflectVector( waterplane.m_pNormal, *velocity );

		this->DampenVelocity( velocity, waterplane.m_pNormal );

		// tell the ship its colliding
		s->PostMessage( MSGID_SET_COLLIDING, NULL );

		return true;
	}

	return false;
}





void CWorldSphere::CheckCollisionVsSphere( CTask * s )
{
	NSVector3df spos;
	NSSphere sp;
	float sradius;

	spos = *( (NSNode*) s->PostMessage(MSGID_GET_NODE, NULL) )->GetWorldTranslation();

	// make sure were at a decent height
	if ( spos.GetLength() > this->m_fMinHeight )
		return;


	sradius = *(float*) s->PostMessage(MSGID_GET_BOUNDINGRADIUS, NULL);
	sp = NSSphere( spos, sradius );


	// Check collisions vs water
	if ( CheckCollisionWithWater( s, sp ) )
		return;


	// work out the tile which the sphere is above
	int x, y;

	// unlike the flat mesh, the player cannot be outside the map since it is a sphere
	// unless they're at the origin but the player can never get there
	// so we dont need a return value of true / false
	this->GetCellPlayerIsAbove( sp, x, y );


	NSVector3df v11, v12, v13;
	NSVector3df v21, v22, v23;

	// because the spheres cells vary in size,
	// we need to do this after we know which cell were above
	// work out how many tiles we need to check using the sphere's radius
	this->GetVerticesToCheck( x, y, v11, v12, v13, v21, v22, v23 );

	int numTilesToCheck = this->GetNumTilesToCheck( sp, v11, v12, v13, v21, v22, v23 );

	int currX, currY;

	for ( int i = x - numTilesToCheck; i < x + numTilesToCheck; i++ )
	{
		for ( int j = y - numTilesToCheck; j < y + numTilesToCheck; j++ )
		{
			currX = i;
			currY = j;

			// check if our X is out of bounds
			// if so, switch to the other side of the map
			if ( currX < 0 )
				currX += this->m_iNumSlices;
			else if ( currX >= this->m_iNumSlices )
				currX -= this->m_iNumSlices;

			// if were out of bounds on the Y axis
			// we want to check start counting down the other side with an inverse X value
			if ( currY < 0 )
			{
				// invert the Y value
				currY = abs(currY);

				// invert our x value
				currX = i + (this->m_iNumSlices / 2);

				if ( currX > this->m_iNumSlices )
					currX -= this->m_iNumSlices;
				else if ( currX < 0 )
					currX += this->m_iNumSlices;
			}
			else if ( currY > this->m_iNumStacks )
			{
				// invert the Y value
				currY = currY - abs(currY);

				// invert our x value
				currX = i + (this->m_iNumSlices / 2);

				if ( currX > this->m_iNumSlices )
					currX -= this->m_iNumSlices;
				else if ( currX < 0 )
					currX += this->m_iNumSlices;
			}


			// get the verticies for the current triangle
			this->GetVerticesToCheck( currX, currY, v11, v12, v13, v21, v22, v23 );
			
			// check for collision
			if ( this->CheckCollisionWithCell( s, sp, v11, v12, v13, v21, v22, v23 ) )
				return;
		}
	}


			//// get the verticies for the current triangle
			//this->GetVerticesToCheck( x, y, v11, v12, v13, v21, v22, v23 );
			//
			//// check for collision
			//if ( this->CheckCollisionWithCell( s, sp, v11, v12, v13, v21, v22, v23 ) )
			//	return;

	// no collision
	return;
}

