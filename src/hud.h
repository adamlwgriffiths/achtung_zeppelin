/******************************************************************************************

                 Filename: HUD.h
                   Author: Adam Griffiths
              Description: HUD class, handles rendering of values sent to it during update.

 ******************************************************************************************/

#ifndef UFO_HUD_H
#define UFO_HUD_H

#include "UFO.h"
//#include "Task.h"
#include <stdio.h>
#include "UFOPrerequisites.h"

//#include "d3dapp/d3dfont.h"




//////////////////////////////
//      Start #defines

//       End #defines
//////////////////////////////



class CHUD : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CHUD();
		~CHUD();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CHUD * pHUD;


		enum GAME_STATE
		{
			RUNNING,
			FRIENDLY_NOPLANES,
			ENEMY_NOPLANES,
			FRIENDLY_NOCITIES,
			ENEMY_NOCITIES,
		};


		class PlayerData
		{
			public:
				int		health;
				float	throttle;
				float	posx;
				float	posy;
				float	posz;
				float	speed;
		};

		class ShipData
		{
			public:
				int		numfriendlyships;
				int		numfriendlyzeppelins;
				int		numenemyships;
				int		numenemyzeppelins;

				float	respawnTime;
		};

		class CityData
		{
			public:
				int		numfriendlycities;
				int		numenemycities;
		};

		class CameraData
		{
			public:
				bool	externalcamera;
				char *	currentDescription;

		};

	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);
		void			Render();

		void			UpdatePlayerStats( CHUD::PlayerData * data );
		void			UpdateShipStats( CHUD::ShipData * data );
		void			UpdateCityStats( CHUD::CityData * data );
		void			UpdateCameratats( CHUD::CameraData * data );


		CD3DFont *		m_pFont;

		GAME_STATE		m_eGameState;

		bool			m_bPlayerStatsVisible;

		bool			m_bRespawnTimeVisible;

		int				m_iHealth;
		float			m_fThrottle;
		float			m_fPosX;
		float			m_fPosY;
		float			m_fPosZ;
		float			m_fSpeed;

		int				m_iNumFriendlyShips;
		int				m_iNumFriendlyZeppelins;
		int				m_iNumEnemyShips;
		int				m_iNumEnemyZeppelins;
		float			m_fRespawnTime;

		int				m_iNumFriendlyCities;
		int				m_iNumEnemyCities;

		char *			m_pStateDescription;

		bool			m_bExternalCamera;


		// strings
		char			m_pBuff[80];

};

#endif // #ifndef UFO_HUD_H

