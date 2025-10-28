/***************************************************************************
                          NSMesh.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A basic Mesh class.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSMESH_H
#define NSMESH_H

#include "NeurosisPrerequisites.h"
#include "NSElement.h"
//#include "NSVector3.h"
//#include "NSMatrix4.h"
#include "NSColour.h"

namespace neurosis {


class NSMesh : public NSElement
{
	friend class NSMesh;

	public:
		NSMesh();
		~NSMesh();

		bool			Load( LPDIRECT3DDEVICE9 device, char * filename );
		void			Unload();

		void			Render( LPDIRECT3DDEVICE9 device );

		//void			SetDiffuse( NSColourf &colour );
		//void			UnsetDiffuse();
		void			SetEmissive( NSColourf &colour );
		void			UnsetEmissive();

	protected:
		LPD3DXMESH				m_pMesh;			// Our mesh object
		D3DMATERIAL9 *			m_pMeshMaterials;	// Materials for our mesh
		LPDIRECT3DTEXTURE9 *	m_pMeshTextures;	// Textures for our mesh
		DWORD					m_iNumMaterials;	// Number of mesh materials

		//NSColourf				m_kDiffuse;
		//bool					m_bDiffuse;
		NSColourf				m_kEmissive;
		bool					m_bEmissive;
};


}; // namespace neurosis

#endif // #ifndef NSMESH_H
