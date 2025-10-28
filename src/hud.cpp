#include "HUD.h"

CHUD *		CHUD::pHUD;	


void * CHUD::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch (ID)
	{
		case MSGID_INIT:
			this->Init(Data);							// intialise game structures
			break;
		case MSGID_UPDATE:
			this->Update(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;
		case MSGID_RENDER:
			this->Render();
			break;

		//case MSGID_HIDE_PLAYERSTATS:
		case MSGID_SHOW_SPAWNTIMER:
//			this->m_bPlayerStatsVisible = false;
			this->m_bRespawnTimeVisible = true;
			break;
//		case MSGID_SHOW_PLAYERSTATS:
		case MSGID_HIDE_SPAWNTIMER:
//			this->m_bPlayerStatsVisible = true;
			this->m_bRespawnTimeVisible = false;
			break;

		case MSGID_UPDATE_PLAYERSTATS:
			this->UpdatePlayerStats( (CHUD::PlayerData*)Data );
			break;

		case MSGID_UPDATE_SHIPSTATS:
			this->UpdateShipStats( (CHUD::ShipData*)Data );
			break;

		case MSGID_UPDATE_CITYSTATS:
			this->UpdateCityStats( (CHUD::CityData*)Data );
			break;

		case MSGID_UPDATE_CAMERASTATS:
			this->UpdateCameratats( (CHUD::CameraData*)Data );
			break;

		case MSGID_GAMEOVER_FRIENDLYNOSHIPS:
			this->m_eGameState = CHUD::FRIENDLY_NOPLANES;
			break;

		case MSGID_GAMEOVER_ENEMYNOSHIPS:
			this->m_eGameState = CHUD::ENEMY_NOPLANES;
			break;

		case MSGID_GAMEOVER_FRIENDLYNOCITIES:
			this->m_eGameState = CHUD::FRIENDLY_NOCITIES;
			break;

		case MSGID_GAMEOVER_ENEMYNOCITIES:
			this->m_eGameState = CHUD::ENEMY_NOCITIES;
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CHUD::CHUD()
{
}

CHUD::~CHUD()
{
}





/////////////////////////////////////////////
// Opens an instance of the singleton.<P>
// If its already open, throw an assertion so we can track down
// any bad logic in our program.
void CHUD::Open()
{
	assert(!pHUD);			// shouldnt get here either!

	pHUD = new CHUD();
}

/////////////////////////////////////////////
// Kill our singleton
void CHUD::Close()
{
	assert(pHUD);

	delete pHUD;
	pHUD = NULL;
}

/////////////////////////////////////////////
// Get the singleton instance of the class
CTask * CHUD::Get()
{
	if (!pHUD)
		CHUD::Open();

	return (CTask*)pHUD;
}






void CHUD::Init(void * Data)
{
	// create our font
	this->m_pFont = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

	// get our device
	LPDIRECT3DDEVICE9 pd3dDevice = ( (LPDIRECT3DDEVICE9)CUFOMain::Get()->PostMessage(MSGID_GET_D3DDEVICE, NULL) );
	this->m_pFont->InitDeviceObjects( pd3dDevice );
	this->m_pFont->RestoreDeviceObjects();

	this->m_bPlayerStatsVisible = true;
	this->m_bRespawnTimeVisible = false;

	this->m_eGameState = CHUD::RUNNING;

	this->m_iHealth = 100;
	this->m_fThrottle = 0.0f;
	this->m_fPosX = 0.0f;
	this->m_fPosY = 0.0f;
	this->m_fPosZ = 0.0f;
	this->m_fSpeed = 0.0f;

	this->m_iNumFriendlyShips = 0;
	this->m_iNumFriendlyZeppelins = 0;
	this->m_iNumEnemyShips = 0;
	this->m_iNumEnemyZeppelins = 0;

	this->m_bExternalCamera = true;
}

void CHUD::Destroy()
{
	this->m_pFont->DeleteDeviceObjects();

	if ( this->m_pFont )
		delete this->m_pFont;
	this->m_pFont = NULL;
}

void CHUD::Update( void * Data )
{

}

void CHUD::Render()
{
	int screenwidth = *(int*)CUFOMain::Get()->PostMessage(MSGID_GET_SCREENWIDTH, NULL);
	int screenheight = *(int*)CUFOMain::Get()->PostMessage(MSGID_GET_SCREENHEIGHT, NULL);

	// render a crosshair
	if ( ! this->m_bExternalCamera )
	{
		sprintf( this->m_pBuff, "O" );
		m_pFont->DrawText( (float)( (screenwidth / 2) - 4.0f ), (float)( (screenheight / 2) - 6.0f ),
									D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
	}

	if ( m_bPlayerStatsVisible )
	{
		// Health
		sprintf( this->m_pBuff, "Health: %3i", this->m_iHealth );
		m_pFont->DrawText( 10.0f, 20.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );

		// throttle
		sprintf( this->m_pBuff, "Throttle: %3.0f%%", this->m_fThrottle );
		m_pFont->DrawText( 10.0f, 40.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
	}

	CInputController::Keys keys = *(CInputController::Keys*)CInputController::Get()->PostMessage(MSGID_GET_KEYS, NULL);

	// render our respawn timer
	if ( this->m_bRespawnTimeVisible )
	{
		sprintf( this->m_pBuff, "Time Till Respawn - %5.2f",
				this->m_fRespawnTime );
		m_pFont->DrawText( (float)(screenwidth / 2) - 85.0f, (float)(screenheight / 2) + 90.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
	}

	// Render our extra hud info
	if ( keys.m_bExtraInfo )
	{
		if ( this->m_bPlayerStatsVisible )
		{
			// position
			sprintf( this->m_pBuff, "Translation - X: %5.2f, Y: %5.2f, Z: %5.2f",
					this->m_fPosX, this->m_fPosY, this->m_fPosZ );
			m_pFont->DrawText( 10.0f, 60.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );

			// speed
			sprintf( this->m_pBuff, "Speed: %5.2f km/h", this->m_fSpeed );
			m_pFont->DrawText( 10.0f, 80.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
		}

		// state description
		sprintf( this->m_pBuff, "Camera Target State: %s", this->m_pStateDescription );
		m_pFont->DrawText( 10.0f, 100.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );

		// number of ships
		// friendly forces
		sprintf( this->m_pBuff, "Friendly Forces - Planes: %i, Zeppelins: %i",
					this->m_iNumFriendlyShips, this->m_iNumFriendlyZeppelins );
		m_pFont->DrawText( 10.0f, (float)(screenheight - 60), D3DCOLOR_ARGB(255, 64, 64, 255), _T(this->m_pBuff) );

		// enemy forces
		sprintf( this->m_pBuff, "Enemy Forces - Planes: %i, Zeppelins: %i",
					this->m_iNumEnemyShips, this->m_iNumEnemyZeppelins );
		m_pFont->DrawText( 10.0f, (float)(screenheight - 40), D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );


		// number of cities
		// friendly forces
		sprintf( this->m_pBuff, "Friendly Cities: %i", this->m_iNumFriendlyCities );
		m_pFont->DrawText( 400.0f, (float)(screenheight - 60), D3DCOLOR_ARGB(255, 64, 64, 255), _T(this->m_pBuff) );

		// enemy forces
		sprintf( this->m_pBuff, "Enemy Cities: %i", this->m_iNumEnemyCities );
		m_pFont->DrawText( 400.0f, (float)(screenheight - 40), D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
	}


	// Draw our gameover screens if any
	bool gameover = false;
	switch ( this->m_eGameState )
	{
		case CHUD::FRIENDLY_NOPLANES:
			sprintf( this->m_pBuff, "No More Friendly Zeppelins" );
			m_pFont->DrawText( (float)(screenwidth / 2 - 105), 75.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
			gameover = true;
			break;
		case CHUD::ENEMY_NOPLANES:
			sprintf( this->m_pBuff, "No More Enemy Zeppelins" );
			m_pFont->DrawText( (float)(screenwidth / 2 - 100), 75.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
			gameover = true;
			break;
		case CHUD::FRIENDLY_NOCITIES:
			sprintf( this->m_pBuff, "No More Friendly Cities" );
			m_pFont->DrawText( (float)(screenwidth / 2 - 90), 75.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
			gameover = true;
			break;
		case CHUD::ENEMY_NOCITIES:
			sprintf( this->m_pBuff, "No More Enemy Cities" );
			m_pFont->DrawText( (float)(screenwidth / 2 - 85), 75.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
			gameover = true;
			break;
	}
	if (gameover)
	{
		sprintf( this->m_pBuff, "Game Over" );
		m_pFont->DrawText( (float)(screenwidth / 2 - 45), 95.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );

		sprintf( this->m_pBuff, "Press F2 to Restart" );
		m_pFont->DrawText( (float)(screenwidth / 2 - 75), 115.0f, D3DCOLOR_ARGB(255, 255, 64, 64), _T(this->m_pBuff) );
	}
}


void CHUD::UpdatePlayerStats( CHUD::PlayerData * data )
{
	this->m_iHealth = data->health;
	if ( this->m_iHealth < 0 )
		this->m_iHealth = 0;

	this->m_fThrottle = data->throttle;

	this->m_fPosX = data->posx;
	this->m_fPosY = data->posy;
	this->m_fPosZ = data->posz;

	this->m_fSpeed = data->speed;
}

void CHUD::UpdateShipStats( CHUD::ShipData * data )
{
	this->m_iNumEnemyShips = data->numenemyships;
	this->m_iNumEnemyZeppelins = data->numenemyzeppelins;
	this->m_iNumFriendlyShips = data->numfriendlyships;
	this->m_iNumFriendlyZeppelins = data->numfriendlyzeppelins;
	this->m_fRespawnTime = data->respawnTime;
}


void CHUD::UpdateCityStats( CHUD::CityData * data )
{
	this->m_iNumEnemyCities = data->numenemycities;
	this->m_iNumFriendlyCities = data->numfriendlycities;
}

void CHUD::UpdateCameratats( CHUD::CameraData * data )
{
	this->m_bExternalCamera = data->externalcamera;
	this->m_pStateDescription = data->currentDescription;
}


