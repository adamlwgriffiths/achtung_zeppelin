/******************************************************************************************

                 Filename: GameController.h
                   Author: Adam Griffiths
              Description: Central game logic controller, handles the initialisation,
                           running and destruction of the core game objects.

 ******************************************************************************************/

#ifndef UFO_GAMECONTROLLER_H
#define UFO_GAMECONTROLLER_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "WaterPlane.h"
#include "NSSkyBox.h"

using namespace neurosis;



class CGameController : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CGameController();
		~CGameController();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CGameController * pGameController;


	private:
		// DX functions
		HRESULT			InitDeviceObjects();		// Initialise objects that are dependent on the device, ie, textures, vertex buffers etc.
		HRESULT			DeleteDeviceObjects();		// Called to delete anything created by InitDeviceObjects

		HRESULT			RestoreDeviceObjects();		// Called if the device is destroyed and we need to re-initialise our device dependent objects.
		HRESULT			InvalidateDeviceObjects();	// Called to delete anything created by RestoreDeviceObjects

		void			SetupWorldViewMatrix( LPDIRECT3DDEVICE9 pd3dDevice );
		void			SetupCameraViewMatrix( LPDIRECT3DDEVICE9 pd3dDevice );

		void			Init(void * Data);
		void			Update(void * Data);
		void			Render();
		void			Destroy();

		void			HandleInput();

		void			SetupFog();
		void			SetupLights();


		float			m_fTimeDelta;

		D3DLIGHT9		dirlight;
		CCamera *		m_pCamera;
		NSNode *		m_pScene;
		NSNode *		m_pShipBranch;
		//NSNode *		m_pPlanetBranch;
		NSNode *		m_pCityBranch;
		NSNode *		m_pBulletBranch;
		NSNode *		m_pParticleBranch;
		NSNode *		m_pSkyBox;

		NSNode *		m_pZeppelinBranch;
		NSNode *		m_pZeppelinBulletBranch;

		CTask *			m_pWaterPlane;
};

#endif // #ifndef UFO_GAMECONTROLLER_H
