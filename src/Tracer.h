/******************************************************************************************

                 Filename: Tracer.h
                   Author: Adam Griffiths
              Description: A bullet

 ******************************************************************************************/

#ifndef UFO_TRACER_H
#define UFO_TRACER_H

#include <stdio.h>
#include "UFOPrerequisites.h"
#include "NSNode.h"
#include "MeshManager.h"

using namespace neurosis;


//////////////////////////////
//      Start #defines

#define TRACER_ROTATION_SPEED			25.0f
#define TRACER_SHIPVELOCITY_INHERITANCE	0.2f


//       End #defines
//////////////////////////////


class CTracer : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CTracer();
		~CTracer();

		class TracerSetupData
		{
			public:
				ALLIANCE		alliance;
				NSNode *		bulletBranch;
				bool			flatworld;
				CTask *			owner;
				NSVector3df		pos;
				NSVector3df		dir;
				NSVector3df		shipvelo;
		};

		static float	s_fSpeed;

	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);

		NSNode *		m_pNode;
		NSNode *		m_pBulletBranch;
		ALLIANCE		m_eAlliance;				// use if friendly fire is off
		CTask *			m_pOwner;

		NSVector3df		m_kVelocity;
		NSVector3df		m_kShipVelocity;

		//float			m_fRotationX;
		//float			m_fRotationY;
		//float			m_fRotationZ;

		float			m_fTimeDelta;

		float			m_fLifetime;

		bool			m_bFlatWorld;

		static int		s_iNumTracers;
		static float	s_fRadius;
		static int		s_iDamage;
		static float	s_fGravityPullStrength;
		static float	s_fLifetime;
};

#endif // #ifndef UFO_TRACER_H

