/******************************************************************************************

                 Filename: ParticleManager.h
                   Author: Adam Griffiths
              Description: Manages the particle branch.

 ******************************************************************************************/

#ifndef UFO_PARTICLEMANAGER_H
#define UFO_PARTICLEMANAGER_H

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "Particles.h"
#include "NSNode.h"
#include "MeshManager.h"

using namespace neurosis;


class PSystem;
class CityExplosionPSystem;
class CMeshManager;

class CParticleManager : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CParticleManager();
		~CParticleManager();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CParticleManager * pParticleManager;


		enum PARTICLESYSTEMS
		{
			CITY_EXPLOSION,
			PLANE_SHRAPNEL,
			PLANE_SMOKE,
		};


		class NewSystemData
		{
			public:
				NSVector3df			trans;
				NSVector3df			velocity;
				CParticleManager::PARTICLESYSTEMS	type;
				float				radius;
				NSNode *			parent;
//				bool				smoke;
//				CMeshManager::MESHES meshtype;
		};

	protected:
		void		Init( void * Data );
		void		Update( void * Data );
		void		Destroy();

		PSystem *	CreatePSystem( CParticleManager::NewSystemData * data );
		void		DeletePSystem( PSystem * system );

		NSNode *	m_pPSystemsBranch;

		PSystem **	m_pPSystems;
		int			m_iNumPSystems;

		static const int	s_iMaxNumPSystems;
};


#endif // #ifndef UFO_PARTICLEMANAGER_H

