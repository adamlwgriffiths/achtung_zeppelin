/******************************************************************************************

                 Filename: City.h
                   Author: Adam Griffiths
              Description: A city located on the terrain

 ******************************************************************************************/

#ifndef UFO_CITY_H
#define UFO_CITY_H

//#include "UFO.h"
//#include "Task.h"
#include <stdio.h>
#include "UFOPrerequisites.h"
#include "NSNode.h"
#include "MeshManager.h"
#include "ParticleManager.h"

using namespace neurosis;


//////////////////////////////
//      Start #defines


///////////////////////
// Damage
#define CITY_HEALTH_MAX					100
#define CITY_HEALTH_MIN					0

#define CITY_DAMAGEABLE									// comment out to disable city damage


///////////////////////
// Size

// temp radius values
#define CITY_BOUNDINGRADIUS_FRIENDLY	40.0f
#define CITY_BOUNDINGRADIUS_ENEMY		40.0f

// actual box values, not used... yet
#define CITY_BOUNDINGWIDTH_FRIENDLY		1.0f
#define CITY_BOUNDINGLENGTH_FRIENDLY	1.0f
#define CITY_BOUNDINGHEIGHT_FRIENDLY	1.0f

#define CITY_BOUNDINGWIDTH_ENEMY		1.0f
#define CITY_BOUNDINGLENGTH_ENEMY		1.0f
#define CITY_BOUNDINGHEIGHT_ENEMY		1.0f


//       End #defines
//////////////////////////////


class PSystem;
class CityExplosionPSystem;

class CCity : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CCity();
		~CCity();

		class CitySetupData
		{
			public:
				NSNode *		cityBranch;
				float			x, y, z;
				ALLIANCE		alliance;
		};

	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);

		void			ApplyDamage( int amount );


		int				m_iArrayPosition;

		NSNode *		m_pCityBranch;
		NSNode *		m_pNode;
		ALLIANCE		m_eAlliance;

		int				m_iHealth;
		float			m_fRadius;

		bool			m_bAlive;
		bool			m_bFinished;

		bool			m_bFlatWorld;

		CityExplosionPSystem *	m_pParticleSystem;

		NSVector3df		m_kOriginalPosition;		// used during the falling animation

		static const float	s_fCityFallSpeed;
		static const float	s_fCityFallMaxJitter;

		static float	s_fMass;

		static int		m_iNumCities;
};

#endif // #ifndef UFO_CITY_H

