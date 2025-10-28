/******************************************************************************************

                 Filename: WorldFlat.h
                   Author: Adam Griffiths
              Description: Base world class

 ******************************************************************************************/

#ifndef UFO_WORLD_H
#define UFO_WORLD_H

#include <time.h>

#include "UFOPrerequisites.h"
//#include "UFO.h"
#include "fractalsamp.h"
#include "NSNode.h"
#include "ShipController.h"

using namespace neurosis;




#define WORLD_COLLISION_MINDAMAGE_LAND				15
#define WORLD_COLLISION_MINDAMAGE_WATER				0

#define WORLD_COLLISION_DAMAGESCALE_WATER			2.0f
#define WORLD_COLLISION_DAMAGESCALE_LAND			1.0f

#define WORLD_COLLISION_RETURNSPEEDSCALE			0.75f


class CUFOMain;



class CWorld : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CWorld() {};
		virtual ~CWorld() {};

		class WorldSetup
		{
			public:
				LPDIRECT3DDEVICE9 pd3dDevice;
				float	fMeshHeightScale;
				int		iTexSize;			// texture is square
				int		iAmplitude;
				float	fRoughness;
		};


	protected:

		void		Init( void * Data );
		void		Render();
		void		Destroy();


		virtual bool	CreateDXTexture( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;

		void		ApplyDamageToShip( CTask * s,
										NSVector3df &collisionNormal,
										NSVector3df &collisionVelocity,
										int lowestDamage, float damageScale );

		void		DampenVelocity( NSVector3df * velocity, const NSVector3df &normal ) const;



		float		m_fMeshHeightScale;
		int			m_iTexSize;
		int			m_iAmplitude;
		float		m_fRoughness;


		float		m_fHeightOffset;
		float		m_fSeaLevel;							// height at which sea is
		float		m_fIceLevel;							// height at which ice is



		int			m_iNumVertices;
		int			m_iNumTriangles;
		int			m_iNumIndices;

		NSVector3df *	m_pSystemVertexBuffer;
		DWORD *			m_pSystemIndexBuffer;

		LPDIRECT3DTEXTURE9			m_pTexture;

		Fractal	*	m_kHeightMap;
};



#endif // #ifndef UFO_WORLD_H
