/******************************************************************************************

                 Filename: WorldFlat.h
                   Author: Adam Griffiths
              Description: Creates a flat height mapped mesh.

 ******************************************************************************************/

#ifndef UFO_WORLDFLAT_H
#define UFO_WORLDFLAT_H

#include <time.h>

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "fractalsamp.h"
#include "NSVertex3.h"
#include "NSVector3.h"
#include "NSMatrix4.h"
#include "NSSphere.h"
#include "NSPlane.h"
#include "NSCollision.h"

using namespace neurosis;


#define WORLDFLAT_COLLISION_MINDAMAGE_LAND		15
#define WORLDFLAT_COLLISION_MINDAMAGE_WATER		0

#define WORLDFLAT_COLLISION_DAMAGESCALE_WATER	0.01f//2.0f
#define WORLDFLAT_COLLISION_DAMAGESCALE_LAND	0.05f//1.0f

#define WORLDFLAT_COLLISION_FRICTIONSCALE		0.75f
#define WORLDFLAT_COLLISION_VERTICALRETURNSCALE	0.75f


class CWorldFlat : public CTask// : public CWorld
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CWorldFlat();
		~CWorldFlat();

		class WorldFlatSetup
		{
			public:
				int		iMeshWidth;
				int		iMeshLength;
				int		iMeshSpacing;
				float	fMeshHeightScale;
				int		iTexSize;			// texture is square
				int		iAmplitude;
				float	fRoughness;
		};

		class WorldGetHeightAt
		{
			public:
				int x, y;
		};

		class WorldGetHeightAtFloat
		{
			public:
				float x, z;
		};

	protected:

		void		Init( void * Data );
		void		Render();
		void		Destroy();

		bool		GenerateFlatMesh();

		void		SmoothEdges( int GradientSize, int GradientMax );
		bool		CreateDXTexture();

		int			GetVertexPointOnTexture( int x, int y );

		void		CheckCollisionVsSphere( CTask * s );

		int			GetNumTilesToCheck( float radius );
		bool		GetCellPlayerIsAbove( float playerX, float playerZ, int &x, int &z );
		bool		CheckCollisionWithWater( CTask * s, NSSphere &sp );
		bool		CheckCollisionWithCell( CTask * s, NSSphere &sp, int x, int z );
		void		ApplyDamageToShip( CTask * s,
										NSVector3df &collisionNormal,
										NSVector3df &collisionVelocity,
										int lowestDamage, float damageScale );

		void		DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const;



		float		GetHighestPointAtCell( float x, float z );


		int			m_iMeshWidth;
		int			m_iMeshLength;
		int			m_iTexSize;
		int			m_iMeshSpacing;
		float		m_fMeshHeightScale;
		int			m_iAmplitude;
		float		m_fRoughness;

		float		m_fMinCollisionDistance;
		float		m_fMinHeight;

		int			m_iNumVertices;
		int			m_iNumTriangles;
		int			m_iNumIndices;

		int			m_iStartX;
		int			m_iEndX;
		int			m_iStartZ;
		int			m_iEndZ;

		int			m_iWidth;
		int			m_iNumVertsPerRow;
		int			m_iNumVertsPerCol;
		int			m_iNumCellsPerRow;
		int			m_iNumCellsPerCol;

//		float		m_fHeightOffset;			// value to raise the terrain by
//		float		m_fSeaLevel;
//		float		m_fIceLevel;
//		float		m_fIceOffset;
		float		m_fIceOffset;
		float		m_fHeightOffset;			// value to raise the terrain by
		float		m_fSeaLevel;							// height at which sea is
		float		m_fIceLevel;							// height at which ice is
		float		m_fIceScale;
		float		m_fMountainPeakLevel;
		float		m_fMountainPeakScale;
		float		m_fMountainLevel;
		float		m_fSandLevel;

		LPDIRECT3DVERTEXBUFFER9		m_pVertexBuffer;        // Buffers to hold land
		LPDIRECT3DINDEXBUFFER9		m_pIndexBuffer;
		LPDIRECT3DTEXTURE9			m_pTexture;

		NSVector3df *	m_pSystemVertexBuffer;
		DWORD *			m_pSystemIndexBuffer;

		Fractal	*	m_kHeightMap;

		bool		m_bCapsMag;
		bool		m_bCapsMin;
};



#endif // #ifndef UFO_WORLDFLAT_H
