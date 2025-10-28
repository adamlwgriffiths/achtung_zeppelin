#include "WaterPlane.h"

const float	CWaterPlane::s_fSize			= VIEW_DISTANCE;// / 2.0f;//10000.0f;

void * CWaterPlane::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init(Data);							// intialise game structures
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		case MSGID_RENDER:
			this->Render();
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CWaterPlane::CWaterPlane()
{}

CWaterPlane::~CWaterPlane()
{}


void CWaterPlane::Init( void * Data )
{
	this->m_pTexture = NULL;

	HRESULT hr = 0;

	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );


	hr = D3DXCreateMeshFVF( 2, 4,
							D3DXMESH_32BIT | D3DXMESH_MANAGED,
							NSVertex3df::FVF, pd3dDevice, &this->m_pMesh);

	if( FAILED(hr))
	{
		MessageBox(0, _T("CWaterPlane::Init - D3DXCreateMeshFVF() for this->m_pMesh - FAILED"), 0, 0);
		return;
	}

	NSVertex3df * v = 0;
	this->m_pMesh->LockVertexBuffer(0, (void**)&v);

	// 0 -- 1
	// | /  |
	// 2 -- 3

	v[0] = NSVertex3df( -this->s_fSize, 0.0f, -this->s_fSize,		// co-ordinates
										0.0f, 1.0f, 0.0f,			// normal
										0.0f, 0.0f );				// texture co-ords
	v[1] = NSVertex3df( +this->s_fSize, 0.0f, -this->s_fSize,
										0.0f, 1.0f, 0.0f,
										1.0f, 0.0f );
	v[2] = NSVertex3df( -this->s_fSize, 0.0f, +this->s_fSize,
										0.0f, 1.0f, 0.0f,
										0.0f, 1.0f );
	v[3] = NSVertex3df( +this->s_fSize, 0.0f, +this->s_fSize,
										0.0f, 1.0f, 0.0f,
										1.0f, 1.0f );

	this->m_pMesh->UnlockVertexBuffer();
	v = NULL;



	/////////////////////////////////
	// Create our indices


	// Define the triangles of the box
	DWORD * indices = 0;
	this->m_pMesh->LockIndexBuffer( 0, (void**)&indices );

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 1;

	this->m_pMesh->UnlockIndexBuffer();
	indices = NULL;



	// Optimize the mesh to generate an attribute table.
	std::vector<DWORD> adjacencyInfo( this->m_pMesh->GetNumFaces() * 3 );
	this->m_pMesh->GenerateAdjacency( 0.001f, &adjacencyInfo[0] );

	// Compute normals
	D3DXComputeNormals( this->m_pMesh, &adjacencyInfo[0] );

//	hr = this->m_pMesh->OptimizeInplace( D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE,
//								&adjacencyInfo[0], 0, 0, 0);






	// create our texture
	int size = 2;
	hr = D3DXCreateTexture( pd3dDevice, size, size, 0, 0,
						D3DFMT_X8R8G8B8, // 32 bit XRGB format
						D3DPOOL_MANAGED, &this->m_pTexture );

	if ( FAILED(hr) )
	{
		this->m_pTexture = NULL;
		return;
	}

	D3DSURFACE_DESC textureDesc;
	this->m_pTexture->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
	{
		this->m_pTexture->Release();
		this->m_pTexture = NULL;
		return;
	}

	D3DLOCKED_RECT lockedRect;
	this->m_pTexture->LockRect(0/*lock top surface*/, &lockedRect,
								0 /* lock entire tex*/, 0/*flags*/);
	DWORD * imageData = (DWORD*)lockedRect.pBits;

	// taken from WorldFlat createdxtexture
	//float fHeightOffset = 80.0f;
	//float fIceLevel = 85.0f;

	//int height = (int)( fIceLevel - fHeightOffset + 1.0f);
	//height = (int)(  ( (float)height * 5.0f) + 80.0f  );
	int r, g, b;

	//r = 255 - ( 3 * (255 % height) );
	//g = 255 - ( 2 * (255 % height) );
	//b = 255 - ( 255 % height );
	r = 6;
	g = 69;//69;//89;
	b = 147;//152;//172;

	D3DXCOLOR colour = D3DCOLOR_XRGB(r,g,b);

	// fill locked data, note we divide the pitch by four because the
	// pitch is given in bytes and there are 4 bytes per DWORD.
	for ( int i = 0; i < size; i++ )
	{
		for ( int j = 0; j < size; j++ )
		{
			int pos = (j * lockedRect.Pitch / 4) + i;
			imageData[ pos ] = (D3DCOLOR)colour;
		}
	}

	this->m_pTexture->UnlockRect(0);

	hr = D3DXFilterTexture( this->m_pTexture, 0, 0, D3DX_DEFAULT );

	if (FAILED(hr))
	{
		MessageBox(0,_T("WorldFlat::CreateDXTexture - D3DXFilterTexture() Failed!"),0,0);
		return;
	}
}


void CWaterPlane::Update( void * Data )
{
//	NSVector3df camtrans = *(NSVector3df*)Data;

}

void CWaterPlane::Destroy()
{
	if (this->m_pTexture != NULL)
		this->m_pTexture->Release();
	this->m_pTexture = NULL;

	if (this->m_pMesh != NULL)
		this->m_pMesh->Release();
	this->m_pMesh = NULL;
}

void CWaterPlane::Render()
{
	LPDIRECT3DDEVICE9 device = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );

	CTask * camera = (CTask*)CGameController::Get()->PostMessage(MSGID_GET_CAMERA, NULL);
	NSNode * camnode = (NSNode*)camera->PostMessage(MSGID_GET_NODE, NULL);
	NSVector3df camtrans = *camnode->GetWorldTranslation();

	NSMatrix4 tempmat = NSMatrix4();
	tempmat.SetIdentity();
	tempmat.SetTranslation( camtrans.v.x, -5.0f, camtrans.v.z );

	D3DXMATRIXA16 mat;
	mat = *tempmat.GetDXMatrix();


	// set the translation
	device->SetTransform( D3DTS_WORLD, &mat );

	// render the world mesh
	device->SetRenderState( D3DRS_LIGHTING, FALSE );

	// set the texture
	device->SetTexture(0, this->m_pTexture);

//	device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );

	// render the mesh

	this->m_pMesh->DrawSubset(0);
	
	// unset the texture
	device->SetTexture(0, 0);

	// undo our texture stage functions
//	device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );

	device->SetRenderState( D3DRS_LIGHTING, TRUE );
}

