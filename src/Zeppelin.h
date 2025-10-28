/******************************************************************************************
                                                                                         
                 Filename: Zeppelin.h
                   Author: Adam Griffiths
              Description: A Zeppelin air ship. Mainstay of the fleet.
			               Class controls everything related to the zeppelin ships.

 ******************************************************************************************/

#ifndef UFO_ZEPPELIN_H
#define UFO_ZEPPELIN_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "Tracer.h"
#include "ParticleManager.h"
#include "Turret.h"

#include "NSVector3.h"
#include "NSNode.h"

using namespace neurosis;




//////////////////////////////
//      Start #defines


///////////////////////
// Damage
#define ZEPPELIN_DAMAGEABLE

//       End #defines
//////////////////////////////

class CTracer;

class CTurret;

class PSystem;
class CityExplosionPSystem;



class CZeppelin : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CZeppelin();
		virtual ~CZeppelin();

		// class used to send initialisation data to the ship
		class InitData
		{
			public:
				NSVector3df	trans;
				NSMatrix4	mat;
				ALLIANCE	alliance;
				CTask *		shipcontroller;
		};

		enum STATE
		{
			FLYING,
			FALLING,
			DEAD,
		};

		enum ENEMY_LOCATION
		{
			LEFT			= 1,
			RIGHT			= 2,
			ABOVE			= 4,
			BELOW			= 8,
		};

	protected:
		void	Init( void * Data );
		void	Destroy();
		void	Update( void * Data );

		void	CreateTurrets();

		void	AIGoToTarget();
		void	AIFalling();

		void	ApplyDamage( int amount );

		void	NotifyOfShipDeath( CTask * ship );
		void	NotifyOfCityDeath( int arraypos );

		void	SearchForTarget();


		CZeppelin::STATE	m_eState;

		int				m_iHealth;
		ALLIANCE		m_eAlliance;
		SHIP_TYPE		m_eShipType;

		bool			m_bAlive;

		NSNode *		m_pNode;							// the ships node in the scene
		NSNode *		m_pBulletNode;
		NSVector3df		m_kVelocity;

		CTask *			m_pShipController;

		int				m_iArrayPosition;

		bool			m_bColliding;

		float			m_fTimeDelta;

		CTask *			m_pTarget;
		float			m_fTargetSearchTimer;

		bool			m_bIsFiring;

		char			m_pStateDescription[40];


		static const int	m_iNumTurrets = 4;
		CTurret *		m_pTurrets[ m_iNumTurrets ];


		static const float	s_fTurnRate;
		static const float	s_fGravityPullStrength;
		static const float	s_fDiveRate;

		static const int	s_iMaxHealth;
		static const int	s_iMinHealth;

		static int		s_iNumZeppelins;

		static float	s_fMass;
		static const float	s_fSpeed;

		static const float	s_fTargetSearchTime;

		static float		s_fBBRadius;
		static NSMatrix4	s_kBBMatrix;
};


#endif // #ifndef UFO_ZEPPELIN_H