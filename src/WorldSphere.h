/******************************************************************************************

                 Filename: WorldSphere.h
                   Author: Adam Griffiths
              Description: Creates a spherical height mapped mesh.
			               Algorithm borrowed from Ian Folley.
						   Heightmap created using Chris Bates Fractal algorithm.

 ******************************************************************************************/

#ifndef UFO_WORLDSPHERE_H
#define UFO_WORLDSPHERE_H

#include <time.h>
#include <vector>

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "fractalsamp.h"
#include "NSVertex3.h"
#include "NSVector3.h"
#include "NSMatrix4.h"
#include "NSMaths.h"

using namespace neurosis;



#define WORLDSPHERE_COLLISION_MINDAMAGE_LAND		15
#define WORLDSPHERE_COLLISION_MINDAMAGE_WATER		0

#define WORLDSPHERE_COLLISION_DAMAGESCALE_WATER		0.01f//2.0f
#define WORLDSPHERE_COLLISION_DAMAGESCALE_LAND		0.05f//1.0f

#define WORLDSPHERE_COLLISION_FRICTIONSCALE			0.75f
#define WORLDSPHERE_COLLISION_VERTICALRETURNSCALE	0.75f


class CWorldSphere : public CTask// : public CWorld
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CWorldSphere();
		~CWorldSphere();

		class WorldSphereSetup
		{
			public:
				float	fRadius;
				int		iNumSlices;
				int		iNumStacks;
				float	fMeshHeightScale;
				int		iTexSize;			// texture is square
				int		iAmplitude;
				float	fRoughness;
		};

	protected:

		void		Init( void * Data );
		void		Render();
		void		Destroy();

		bool		GenerateSphericalMesh();

		void		SmoothPoles( int GradientSize, int GradientMax );
		bool		CreateDXTexture();

		int			GetVertexPointOnTexture( float x, float y );

		void		CheckCollisionVsSphere( CTask * s );

		void		GetCellPlayerIsAbove( NSSphere &sp, int &x, int &y );

		bool		CheckCollisionWithWater( CTask * s, NSSphere &sp );

		bool		CheckCollisionWithCell( CTask * s, NSSphere &sp,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 );

		int			GetNumTilesToCheck( NSSphere &sp,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 );

		void		GetVerticesToCheck( int x, int y,
							NSVector3df &v11, NSVector3df &v12, NSVector3df &v13,
							NSVector3df &v21, NSVector3df &v22, NSVector3df &v23 );

		void		ApplyDamageToShip( CTask * s,
										NSVector3df &collisionNormal,
										NSVector3df &collisionVelocity,
										int lowestDamage, float damageScale );

		void		DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const;



		float		m_fRadius;
		int			m_iNumSlices;
		int			m_iNumStacks;
		float		m_fMeshHeightScale;
		int			m_iTexSize;
		int			m_iAmplitude;
		float		m_fRoughness;

		float		m_fHeightOffset;

		int			m_iNumVertices;
		int			m_iNumTriangles;
		int			m_iNumIndices;

		float		m_fActualSeaLevel;
		float		m_fMinHeight;

		float		m_fSeaLevel;							// height at which sea is
		float		m_fIceLevel;							// height at which ice is
		float		m_fIceScale;
		float		m_fMountainPeakLevel;
		float		m_fMountainPeakScale;
		float		m_fMountainLevel;
		float		m_fSandLevel;

		ID3DXMesh *					m_pWorldMesh;
		LPDIRECT3DTEXTURE9			m_pTexture;

		NSVector3df *	m_pSystemVertexBuffer;
		DWORD *			m_pSystemIndexBuffer;


		Fractal	*	m_kHeightMap;

		bool		m_bCapsMag;
		bool		m_bCapsMin;
};



#endif // #ifndef UFO_WORLDSPHERE_H
