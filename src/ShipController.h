/******************************************************************************************
                                                                                         
                 Filename: ShipManager.h
                   Author: Adam Griffiths
              Description: Handles the initialisation, access to, destruction and updating
			               of Ships in the game.

 ******************************************************************************************/

#ifndef UFO_SHIPCONTROLLER_H
#define UFO_SHIPCONTROLLER_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "NSVector3.h"
#include "NSCollision.h"

#define SHIPS_MAX_NUM		30
#define ZEPPELINS_MAX_NUM	4

#define SHIP_COLLISION_VSSHIPDAMAGESCALE	0.7f
#define SHIP_COLLISION_VSGROUNDDAMAGESCALE	1.0f
#define SHIP_COLLISION_RETURNSPEEDSCALE		0.75f

#define SHIP_COLLISION_VSZEPPELINDAMAGESCALE	0.006f


class CShipController : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CShipController();
		~CShipController();

		class ShipControllerInitData
		{
			public:
				NSNode *	shipBranch;
				NSNode *	bulletBranch;
				NSNode *	zeppelinBranch;
				NSNode *	zeppelinBulletBranch;
		};



		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CShipController * pShipController;

		enum CAMERA_TARGET
		{
			CAMTARG_PLAYER,
			CAMTARG_FRIENDLY,
			CAMTARG_ENEMY,
			CAMTARG_ZEPFRIENDLY,
			CAMTARG_ZEPENEMY,
		};

	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);

		CTask *			CreateShip(void * Data);				// create plane // uses CShip::InitData
		void			DestroyShip( CTask * ship );			// remove plane
		void			InvalidateShip( CTask * ship );			// mark plane as dead

		CTask *			CreateZeppelin( void * Data );			// create zeppelin
		void			DestroyZeppelin( CTask * zeppelin );	// remove zeppelin
		void			InvalidateZeppelin( CTask * zeppelin );	// make zeppelin as dead

		void			SpawnShips();
		void			SpawnZeppelins();

		void			CheckCameraCycle();
		void			CameraCycleNext();
		void			CameraCyclePrevious();


		void			CheckCollisionsShipVsGround();

		void			CheckCollisionsShipVsShip();
		void				CheckCollisionShip( CTask * s1, CTask * s2 ); // called by ShipVsShip

		void			CheckCollisionShipVsBullet( CTask * bullet );
		void				CheckCollisionBullet( CTask * bullet, CTask * s );

		void			CheckCollisionZeppelinVsBullet( CTask * bullet );
		void				CheckCollisionZeppBullet( CTask * bullet, CTask * z );

		void			CheckCollisionsShipVsCity( CTask * Data );

		void			CheckCollisionsVsZeppelin();
		void				CheckCollisionShipZeppelin( CTask * ship, CTask * zeppelin );
		void				CheckCollisionZeppelinZeppelin( CTask * z1, CTask * z2 );


		NSNode *		m_pShipBranch;								// the root scene node
		NSNode *		m_pBulletBranch;

		NSNode *		m_pZeppelinBranch;
		NSNode *		m_pZeppelinBulletBranch;

		CTask *			m_pPlayerShip;
		bool			m_bPlayer;

		CAMERA_TARGET	m_eCameraTarget;
		int				m_iCameraTargetPos;

		CTask *			m_pFriendlyShips[ SHIPS_MAX_NUM ];
		int				m_iNumFriendlyShips;

		CTask *			m_pEnemyShips[ SHIPS_MAX_NUM ];
		int				m_iNumEnemyShips;

		CTask *			m_pShipsPendingDeletion[ 2 * SHIPS_MAX_NUM + 1 ];	// 2 teams + 1 player
		int				m_iNumShipsPendingDeletion;

		CTask *			m_pFriendlyZeppelins[ ZEPPELINS_MAX_NUM ];
		int				m_iNumFriendlyZeppelins;

		CTask *			m_pEnemyZeppelins[ ZEPPELINS_MAX_NUM ];
		int				m_iNumEnemyZeppelins;

		CTask *			m_pZeppelinsPendingDeletion[ 2 * ZEPPELINS_MAX_NUM ];
		int				m_iNumZeppelinsPendingDeletion;



		float			m_fFriendlySpawnTimer;
		float			m_fEnemySpawnTimer;

		static const float	s_fBulletAllowanceRadius;

		static const float	s_fFriendlySpawnTime;
		static const float	s_fEnemySpawnTime;

		static const float	s_fShipVerticalSpawnOffset;
		static const float	s_fShipHorizontalSpawnOffset;

		static const float	s_fZeppelinHeight;
};

#endif // #ifndef UFO_SHIPCONTROLLER_H
