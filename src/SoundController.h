/******************************************************************************************

                 Filename: SoundController.h
                   Author: Adam Griffiths
              Description: Handles the playing and updating of sounds

 ******************************************************************************************/

#ifndef UFO_SOUNDCONTROLLER_H
#define UFO_SOUNDCONTROLLER_H

#include "UFO.h"
//#include "Task.h"
#include <stdio.h>
#include "UFOPrerequisites.h"

//#include "d3dapp/d3dfont.h"




//////////////////////////////
//      Start #defines

//       End #defines
//////////////////////////////



class CSoundController : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CSoundController();
		~CSoundController();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CSoundController * pSoundController;


		enum SOUNDS
		{
			SHIP_ENGINE,
			ZEPPELIN_ENGINE,
			TRACER,
			ZEPPELIN_GUN,
			SHIP_EXPLOSION,
			SHIP_HIT,
		};

		struct SSound
		{
			CSound *	m_pSound;
			bool		m_bPaused;
			long		m_lVolume;
			long		m_lFrequency;
			bool		m_bLooped;
		};

		class ShipData
		{
			public:
				float	throttle;
				SHIP_TYPE	type;
		};

		class SoundData
		{
			public:
				CSoundController::SOUNDS	sound;
				float						distance;
				long						frequency;
				long						volume;
		};


	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);

		void			PlaySound( CSoundController::SoundData * data );

		void			PlaySound( SSound &sound );

		void			LoadSound( SSound &sound, char * filename, DWORD flags = 0, int numBuffers = 1 );

		void			UpdatePlayerEngine( ShipData * data );


		CSoundManager *	m_pSoundManager;

		SSound			m_sEngineSound;
//		bool			m_bEnableEngine;
		SHIP_TYPE		m_eEngineType;

		SSound			m_sZeppelinSound;


		SSound			m_sTracerSound;
		SSound			m_sZeppelinGunSound;

		SSound			m_sShipExplosionSound;

		SSound			m_sShipHitSound;

		static const float	s_fMaxSoundDistance;
		static float		s_fVolScale;
};

#endif // #ifndef UFO_SOUNDCONTROLLER_H


