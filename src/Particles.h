/******************************************************************************************

                 Filename: Particles.h
                   Author: Adam Griffiths
              Description: Particle and ParticleSystem classes.

 ******************************************************************************************/

#ifndef UFO_PARTICLES_H
#define UFO_PARTICLES_H

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "NSNode.h"
#include "NSVector3.h"
#include "MeshManager.h"

using namespace neurosis;


class CMeshManager;

class Particle
{
	public:
		Particle();
		Particle( NSNode * node );
		virtual ~Particle();

		NSNode *	m_pNode;
		float		m_fCurrLifeSpan;
		float		m_fMaxLifeSpan;
};



class PSystem
{
	public:
		PSystem( NSNode * particleBranch, NSVector3df &trans );
		virtual ~PSystem();

		bool			IsFinished();

		virtual void	Update( float fTimeDelta ) = 0;

		NSNode *		m_pNode;

	protected:
		Particle **		m_pParticles;
		NSNode *		m_pParticleBranch;

		int				m_iNumParticles;

		bool			m_bIsFinished;

		static int		m_iNumPSystems;
};




class PlaneSmoke : public PSystem
{
	public:
		PlaneSmoke( NSNode * particleBranch, NSNode *planeNode );
		virtual ~PlaneSmoke();

		void			Update( float fTimeDelta );

		void			Stop();

	private:
		NSNode *		m_pPlaneNode;
		unsigned int	m_iNumParticles;

		bool			m_bStopped;

		static const float	s_fMinLifeSpan;
		static const float	s_fMaxLifeSpan;

		static const unsigned int	s_iMaxNumParticles;
};


class PlaneShrapnel : public PSystem
{
	public:
		PlaneShrapnel( NSNode * particleBranch, NSVector3df &trans );
		virtual ~PlaneShrapnel();

		void			Update( float fTimeDelta );

	private:
		NSVector3df		m_kVelocity;

		float			m_fRotationX;
		float			m_fRotationY;
		float			m_fRotationZ;

		
		static const float	s_fGravAccel;

		static const float	s_fMinLifeSpan;
		static const float	s_fMaxLifeSpan;
		static const float	s_fMaxRotationSpeed;
};


class CityExplosionPSystem : public PSystem
{
	public:
		CityExplosionPSystem( NSNode * particleBranch, NSVector3df &trans, float fRadius, NSVector3df &velocity = NSVector3df(0.0f, 0.0f, 0.0f) );
		virtual ~CityExplosionPSystem();

		void			Update( float fTimeDelta );

		float *			m_fRotationX;
		float *			m_fRotationY;
		float *			m_fRotationZ;

		NSVector3df *	m_kVelocity;
		NSVector3df		m_kPSystemVelocity;

		Particle ***	m_pSmokeTrails;
		int *			m_iCurrSmokeTrail;
		float *			m_fSmokeTrailTimer;
		NSNode **		m_pSmokeNodes;

		static const int	s_iMaxNumParticles;
		static const int	s_iMinNumParticles;
		static const float	s_fMaxRotationSpeed;
		static const float	s_fMinVelocity;
		static const float	s_fMaxVelocity;
		static const float	s_fMinLifeSpan;
		static const float	s_fMaxLifeSpan;
		static const float	s_fVelocityDampening;
		static const float	s_fGravityStrength;

		static const int	s_iNumSmokePerParticle;
		static const float	s_fSmokeMaxLifeSpan;
		static const float	s_fSmokeMinLifeSpan;
		static const float	s_fSmokeTrailCreationSpeed;

//		static const float	s_fRadialSpread;
};



#endif // #ifndef UFO_PARTICLES_H

