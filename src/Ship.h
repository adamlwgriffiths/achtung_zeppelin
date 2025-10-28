/******************************************************************************************
                                                                                         
                 Filename: Ship.h
                   Author: Adam Griffiths
              Description: A basic ship class, handles movement and rendering.<P>
			               Also contains the ShipPlayer and ShipAI classes to prevent
						   circular dependency hell.

 ******************************************************************************************/

#ifndef UFO_SHIP_H
#define UFO_SHIP_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "Tracer.h"
#include "ParticleManager.h"

#include "NSVector3.h"
#include "NSNode.h"
#include "NSPlane.h"

using namespace neurosis;



//////////////////////////////
//      Start #defines


///////////////////////
// Damage
#define SHIP_HEALTH_MAX					100
#define SHIP_HEALTH_MIN					0

#define SHIP_MASS_FRIENDLY				500.0f
#define SHIP_MASS_ENEMY					500.0f

#define SHIP_CRITICAL_DEATH				-40		// amount required to cause an explosion

#define	SHIP_DAMAGEABLE							// comment out to disable damage

///////////////////////
// Size
#define SHIP_BOUNDINGRADIUS_FRIENDLY	1.0f
#define SHIP_BOUNDINGRADIUS_ENEMY		1.0f

///////////////////////
// Movement
//#define SHIP_INPUT_ACCELSPEED			75.0f
//#define SHIP_INPUT_DECELSPEED			75.0f
//#define SHIP_INPUT_STRAFESPEED			75.0f		// 100.0f = full speed for analogue input
//#define SHIP_INPUT_PITCHSPEED			75.0f
//#define SHIP_INPUT_YAWSPEED				75.0f
//#define SHIP_INPUT_ROLLSPEED			75.0f

#define SHIP_INPUT_ACCELSCALE			0.01f
#define SHIP_INPUT_DECELSCALE			0.01f

#define SHIP_THROTTLE_MAXIMUM			100.0f
#define SHIP_THROTTLE_MINIMUM			0.0f


///////////////////////
// Physics

// #defines are EVVVIIILLL!! these have been made into static variables

//#	define SHIP_MOVEMENT_ROLLSCALE			0.003f//0.0004f
//#	define SHIP_MOVEMENT_YAWSCALE			0.0003f//0.0005f
//#	define SHIP_MOVEMENT_PITCHSCALE			0.0016f//0.0017f
//
//
//#	define SHIP_ACCELERATION_THROTTLESCALE	0.005f
//#	define SHIP_ENGINE_SPEEDINCREASE		10.0f
//#	define SHIP_ENGINE_SPEEDDECREASE		6.0f
//
//#	define SHIP_VELOCITY_FRICTIONSCALE		0.06f
//
//#	define SHIP_GRAVITY_PULLSTRENGTH		0.2f//0.002f
//#	define SHIP_GRAVITY_ROTATIONSTRENGTH	0.1f
//#	define SHIP_LIFT_PUSHSTRENGTH			0.030f//0.035f//0.0015f
//#	define SHIP_LIFT_ROTATIONSTRENGTH		0.4f//15.4f

//#	define SHIP_VELOCITY_SCALE				100.0f//3.0f



///////////////////////
// Weapons

//#define SHIP_LIMITEDAMMO						// comment out to have unlimited ammo

#define SHIP_GUNS_TIMEBETWEENSHOTS		0.2f
#define SHIP_GUNS_AMMOMAX				500
#define SHIP_GUNS_MAXNUMTRACERS			50

#define SHIP_GUNS_BARRELDISTANCE		0.6f
#define SHIP_GUNS_FORWARDDISTANCE		0.7f

//       End #defines
//////////////////////////////


class CTracer;

class PSystem;
class CityExplosionPSystem;



class CShip : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CShip();
		virtual ~CShip();

		// class used to send initialisation data to the ship
		class InitData
		{
			public:
				NSVector3df	trans;
				NSVector3df	up;
				ALLIANCE	alliance;
				bool		isPlayer;
				CTask *		shipcontroller;
				NSMatrix4	mat;
		};

		enum STATE
		{
			TAKEOFF,
			FLYING,
			EXPLODING,
			FALLINGAPART,
			DEAD,
		};

	protected:
		NSNode *		Init( void * Data );
		void			Destroy();
		void			Update( void * Data );

		// our main state updates
		void			UpdateTakeOff();
		void			UpdateFlying();
		void			UpdateExploding();
		void			UpdateFallingApart();

		virtual void	UpdateInput() = 0;	// override this in child classes
		virtual void	PostUpdate() {};

		void			UpdateEngine();

		void			ApplyInput();
//#ifdef SHIP_USE_HOMEMADE_PHYSICS
		void			ApplyPhysics();
//#endif // #ifdef SHIP_USE_HOMEMADE_PHYSICS

		void			ApplyDamage( int amount );
		virtual void	Explode();
		virtual void	FallApart();

		void			FireGuns();
		void			UpdateTracers();


		void			Accelerate( float amount );
		void			Decelerate( float amount );

		void			StrafeLeft( float amount );
		void			StrafeRight( float amount );
		void			StrafeUp( float amount );
		void			StrafeDown( float amount );

		void			RollLeft( float amount );
		void			RollRight( float amount );

		void			TurnLeft( float amount );
		void			TurnRight( float amount );
		void			TurnUp( float amount );
		void			TurnDown( float amount );


		CShip::STATE	m_eState;

		ALLIANCE		m_eAlliance;
		SHIP_TYPE		m_eShipType;

		NSNode *		m_pNode;							// the ships node in the scene
		NSVector3df		m_kVelocity;						// the ships current velocity vector
// ive decided to leave propellers off as they would just get in the way of the player
//		NSNode *		m_pPropellerNode;

		int				m_iHealth;
		bool			m_bAlive;
		bool			m_bColliding;

		float			m_fThrottle;
		float			m_fEngineSpeed;
		float			m_fPitchInput;
		float			m_fYawInput;
		float			m_fRollInput;

		float			m_fRadius;							// used for basic collision detection
		float			m_fMass;

		float			m_fTimeDelta;

		float			m_fTakeOffTimer;

		float			m_fGunTimer;
		int				m_iGunAmmo;
		bool			m_bGunLeftBarrel;
		bool			m_bIsFiring;

		CTracer *		m_pTracers[SHIP_GUNS_MAXNUMTRACERS];
		int				m_iNumTracers;

		NSNode *		m_pBulletNode;

		NSNode *		m_pPropellerNode;

		bool			m_bFlatWorld;

		CTask *			m_pShipController;

//		PSystem *		m_pParticleSystem;
		PSystem *		m_pSmokeSystem;

		int				m_iArrayPosition;

		char			m_pStateDescription[40];

		float			m_fDeathTimer;

		static const float	s_fMaxDeadTime;

		static float	s_fRollScale;
		static float	s_fYawScale;
		static float	s_fPitchScale;

		static float	s_fThrustScale;
		static float	s_fEngineIncreaseScale;
		static float	s_fEngineDecreaseScale;

		static float	s_fDragScale;
		static float	s_fDragAOAScale;

		static float	s_fGravPull;
		static float	s_fGravRotation;
		static float	s_fGravMinLift;
		static float	s_fLiftPush;
		static float	s_fLiftRotation;

		static float	s_fVelocityScale;

		static int		m_iNumShips;

		static const float	s_fTakeOffTime;

//		static const float	s_fPropellerScale;
};





// this is here to stop circular dependency hell from occuring and causing my brain to explode
// i hate single pass compilers!

class CShipPlayer : public CShip
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CShipPlayer();
		~CShipPlayer();

	protected:
		NSNode *		Init( void * Data );

		virtual void	Explode();
		virtual void	FallApart();

		void			UpdateInput();	// handle player input
		virtual void	PostUpdate();
};




// this is here to stop circular dependency hell from occuring and causing my brain to explode
// i hate single pass compilers!
// i also hate the way ive structured this program!!!

class CShipAI : public CShip
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CShipAI();
		~CShipAI();

		enum ORIENTATION_STATES
		{
			INVERTED		= 1,
			ROLLED_LEFT		= 2,
			ROLLED_RIGHT	= 4,
			PITCHED_DOWN	= 8,
			PITCHED_UP		= 16,
		};

		enum REACTION
		{
			PITCH_UP		= 1,
			PITCH_DOWN		= 2,
			ROLL_LEFT		= 4,
			ROLL_RIGHT		= 8,
			YAW_LEFT		= 16,
			YAW_RIGHT		= 32,
			FIRE_GUNS		= 64,
		};

		enum ENEMY_LOCATION
		{
			LEFT			= 1,
			RIGHT			= 2,
			ABOVE			= 4,
			BELOW			= 8,
			INFRONT			= 16,
			BEHIND			= 32,
		};

		enum TURNING_DIRECTION
		{
			TURN_LEFT		= 1,
			TURN_RIGHT		= 2,
		};

	protected:
		NSNode *		Init( void * Data );
		void			UpdateInput();	// handle player input
		void			NotifyOfDeath( CTask * ship );

		int				AINoTarget();
		int				AIWithTarget();

		int				AICheckFireGuns( NSVector3df &targtrans, int targpos );
		bool			AICheckGroundLevel();
		int				AIAvoidGround();
		NSVector3df		AIGetFiringVector();
		int				AIGetOrientation();

		void			AISearchForTarget();

		void			ApplyResponse( int response );


		CTask *			m_pTarget;
		float			m_fTargetSearchTimer;

		bool			m_fTurningDirectionSelected;
		TURNING_DIRECTION	m_eTurningDirection;
		float			m_fBehindTimer;

		bool			m_bAvoidingGround;

		bool			m_bWithinDotAllowance;
		static const float	s_fAIDotDistanceAllowance;
		static const float	s_fAILargeDotDeviationAllowance;
		static const float	s_fAISmallDotDeviationAllowance;

		static const float	s_fAIGroundCheckBuffer;

		static const float	s_fMaxBehindTime;

		static const float	s_fAIGroundCheckSpeedMultiplier;
		static float		s_fAIGroundCheckMinimumSafeHeight;
		static const float	s_fAIFireGunsMinDot;
		static const float	s_fAIFireGunsMinDistance;
		static const float	s_fAIFireGunsMinTrackingDistance;
		static float		s_fAITracerSpeed;
		static const float	s_fAITargetHeightAllowance;

		static const float	s_fAITargetSearchTime;

		static const float	s_fAIEvadeDistance;
};


#endif // #ifndef UFO_SHIP_H
