#include "NSMesh.h"

namespace neurosis
{

NSMesh::NSMesh()
{
	this->m_pMesh = NULL;							// Our mesh object in sysmem
	this->m_pMeshMaterials = NULL;					// Materials for our mesh
	this->m_pMeshTextures = NULL;					// Textures for our mesh
	this->m_iNumMaterials = NULL;

//	this->m_bDiffuse = false;
	this->m_bEmissive = false;
}

NSMesh::~NSMesh()
{
//	this->Unload();		// dont unload manually
}

bool NSMesh::Load( LPDIRECT3DDEVICE9 device, char * filename )
{
	//////////////////////////////////////
	// The following code was `borrowed'
	// from the DX sample programs
	//////////////////////////////////////

	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	if(   FAILED(  D3DXLoadMeshFromX( filename, D3DXMESH_MANAGED, 
								device, NULL,
								&pD3DXMtrlBuffer, NULL, &m_iNumMaterials, &m_pMesh )  )   )
	{
		char msg[50] = "Could not find model: ";
		strcat(msg, filename);
		MessageBox(NULL, msg, "NSMesh::LoadMesh", MB_OK);
		return false;
	}

	// We need to extract the material properties and texture names from the
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	m_pMeshMaterials = new D3DMATERIAL9[m_iNumMaterials];
	m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_iNumMaterials];

	for( DWORD i = 0; i < m_iNumMaterials; i++ )
	{
		// Copy the material
		m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

		m_pMeshTextures[i] = NULL;
		if( d3dxMaterials[i].pTextureFilename != NULL && 
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			// Create the texture
			if( FAILED( D3DXCreateTextureFromFile( device,
												d3dxMaterials[i].pTextureFilename, 
												&m_pMeshTextures[i] ) ) )
			{
				// If texture is not in current folder, try parent folder
				const TCHAR* strPrefix = TEXT("models/");
				const int lenPrefix = lstrlen( strPrefix );
				TCHAR strTexture[MAX_PATH];
				lstrcpyn( strTexture, strPrefix, MAX_PATH );
				lstrcpyn( strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix );
				// If texture is not in current folder, try parent folder
				if( FAILED( D3DXCreateTextureFromFile( device,
													strTexture,
													&m_pMeshTextures[i] ) ) )
				{
					char msg[50] = "Could not find texture map: ";
					strcat(msg, filename);
					MessageBox(NULL, msg, "NSMesh::LoadMesh", MB_OK);
					// dont return because we need to free the vertex buffer
				}
			}
		}
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	// compute our vertex normals
	D3DXComputeNormals( this->m_pMesh, NULL );

	return true;
}


void NSMesh::Unload()
{
	///////////////////////////////
	// Code taken from DX Samples
	///////////////////////////////

	// delete the mesh
    if( this->m_pMeshMaterials != NULL ) 
        delete [] this->m_pMeshMaterials;

    if( this->m_pMeshTextures )
    {
        for( DWORD i = 0; i < this->m_iNumMaterials; i++ )
        {
            if( this->m_pMeshTextures[i] )
                this->m_pMeshTextures[i]->Release();
        }
        delete [] this->m_pMeshTextures;
    }

    if( this->m_pMesh != NULL )
        this->m_pMesh->Release();

	this->m_pMeshMaterials = NULL;
	this->m_pMeshTextures = NULL;
	this->m_pMesh = NULL;
}



void NSMesh::Render( LPDIRECT3DDEVICE9 device )
{
	////////////////////////////////
	// Code taken from DX samples
	////////////////////////////////

	D3DMATERIAL9 mat;

	for( DWORD i = 0; i < m_iNumMaterials; i++ )
    {
		mat = m_pMeshMaterials[i];

		// override the material if weve set up a new diffuse value
		// bad coding ahoy
//		if ( this->m_bDiffuse )
//		{
//			mat.Diffuse.r = this->m_kDiffuse.r;
//			mat.Diffuse.g = this->m_kDiffuse.g;
//			mat.Diffuse.b = this->m_kDiffuse.b;
//		}
		if ( this->m_bEmissive )
		{
			mat.Emissive.r = this->m_kEmissive.r;
			mat.Emissive.g = this->m_kEmissive.g;
			mat.Emissive.b = this->m_kEmissive.b;
		}

		// Set the material and texture for this subset
        device->SetMaterial( &mat );
		device->SetTexture( 0, m_pMeshTextures[i] );
    
        // Draw the mesh subset
        m_pMesh->DrawSubset( i );
    }

	// unset the material
	mat.Ambient.r = 1.0f; mat.Ambient.g = 1.0f; mat.Ambient.b = 1.0f;
	mat.Diffuse.r = 1.0f; mat.Diffuse.g = 1.0f; mat.Diffuse.b = 1.0f;
	mat.Specular.r = 0.0f; mat.Specular.g = 0.0f; mat.Specular.b = 0.0f;
	mat.Emissive.r = 0.0f; mat.Emissive.g = 0.0f; mat.Emissive.b = 0.0f;
	device->SetMaterial( &mat );

	// unset the texture
	device->SetTexture( 0, 0 );
}


//void NSMesh::SetDiffuse( NSColourf &colour )
//{
//	this->m_bDiffuse = true;
//	this->m_kDiffuse = colour;
//}
//
//void NSMesh::UnsetDiffuse()
//{
//	this->m_bDiffuse = false;
//}
//
//
void NSMesh::SetEmissive( NSColourf &colour )
{
	this->m_bEmissive = true;
	this->m_kEmissive = colour;
}

void NSMesh::UnsetEmissive()
{
	this->m_bEmissive = false;
}



}; // namespace neurosis
