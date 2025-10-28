/******************************************************************************************
                                                                                         
                 Filename: Turret.h
                   Author: Adam Griffiths
              Description: An independent turret. Prefers cities over ships.

 ******************************************************************************************/

#ifndef UFO_TURRET_H
#define UFO_TURRET_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "Tracer.h"
#include "ParticleManager.h"

#include "NSVector3.h"
#include "NSNode.h"

using namespace neurosis;



#define SHIP_GUNS_MAXNUMTRACERS			50


class CTurret : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CTurret();
		virtual ~CTurret();

		class InitData
		{
			public:
				CTask *		parent;
				NSNode *	parentnode;
				NSNode *	bulletbranch;
				NSVector3df	trans;
				NSVector3df	facing;
				float		maxdotfiringangle;
				ALLIANCE	alliance;
		};

	protected:
		void	Init( void * Data );
		void	Destroy();
		void	Update( void * Data );

		void	SearchForTarget();

		void	NotifyOfShipDeath( CTask * ship );
		void	NotifyOfCityDeath( int arraypos );

		void	FireGuns();

		void	UpdateTracers();

		NSVector3df GetFiringVector();


		NSNode *	m_pNode;
		NSNode *	m_pBulletNode;
		NSNode *	m_pParentNode;
		NSNode *	m_pBulletParent;
		CTask *		m_pParent;

		CTask *		m_pTarget;
		bool		m_bTargetIsCity;

		float		m_fMaxDotFiringAngle;
		NSVector3df	m_kFacing;

		CTracer *	m_pTracers[SHIP_GUNS_MAXNUMTRACERS];
		int			m_iNumTracers;
		float		m_fGunTimer;

		ALLIANCE	m_eAlliance;

		float		m_fTimeDelta;

		float		m_fTargetSearchTimer;
		float		m_fMinTargetSearchTime;

		bool		m_bActive;


		static int	s_iNumTurrets;
		static const float	s_fTimeBetweenShots;
		static const float	s_fAvgTargetSearchTime;
		static float	s_fMaxTracerDistanceShip;
		static float	s_fMaxTracerDistanceCity;
		static float		s_fTracerSpeed;
};


#endif // #ifndef UFO_TURRET_H
