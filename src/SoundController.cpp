#include "SoundController.h"

CSoundController *		CSoundController::pSoundController;	

const float				CSoundController::s_fMaxSoundDistance = 400.0f;
float					CSoundController::s_fVolScale = 1.0f;



void * CSoundController::PostMessage(int ID, void * Data)
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


		case MSGID_UPDATE_PLAYERENGINE:
			this->UpdatePlayerEngine( (ShipData*)Data );
			break;

		//case MSGID_DISABLE_PLAYERENGINE:
		//	m_bEnableEngine = false;
		//	break;

		//case MSGID_ENABLE_PLAYERENGINE:
		//	m_bEnableEngine = true;
		//	break;

		case MSGID_PLAY_SOUND:
			this->PlaySound( (CSoundController::SoundData*) Data );
			break;

		default:
			assert(0);									// should never get here
	}

	return result;
}


CSoundController::CSoundController()
{
}

CSoundController::~CSoundController()
{
}





/////////////////////////////////////////////
// Opens an instance of the singleton.<P>
// If its already open, throw an assertion so we can track down
// any bad logic in our program.
void CSoundController::Open()
{
	assert(!pSoundController);			// shouldnt get here either!

	pSoundController = new CSoundController();
}

/////////////////////////////////////////////
// Kill our singleton
void CSoundController::Close()
{
	assert(pSoundController);

	delete pSoundController;
	pSoundController = NULL;
}

/////////////////////////////////////////////
// Get the singleton instance of the class
CTask * CSoundController::Get()
{
	if (!pSoundController)
		CSoundController::Open();

	return (CTask*)pSoundController;
}






void CSoundController::Init(void * Data)
{
	this->m_pSoundManager = NULL;

	// work out our volume scale
	s_fVolScale = (float)pow(  s_fMaxSoundDistance, 3 ) + 1.0f;
	s_fVolScale = 100.0f / s_fVolScale;

	s_fVolScale = s_fVolScale / 100.0f;


	///////////////////////
	// load the sound manager

	this->m_pSoundManager = new CSoundManager();
	if( this->m_pSoundManager == NULL )
		return;

	HWND hWnd = (HWND)CUFOMain::Get()->PostMessage(MSGID_GET_HWND, NULL);

	if(   FAILED(  this->m_pSoundManager->Initialize( hWnd, DSSCL_PRIORITY )  )   )
		return;

    if(   FAILED(  this->m_pSoundManager->SetPrimaryBufferFormat( 2, 22050, 16 )  )   )
        return;


	///////////////////////
	// load our sounds

	// Player Engine
	this->m_sEngineSound.m_pSound = NULL;
	this->m_sEngineSound.m_bPaused = false;
	this->m_sEngineSound.m_lVolume = 0;
	this->m_sEngineSound.m_lFrequency = -1;
	this->m_sEngineSound.m_bLooped = false;

	char buff[40];
	sprintf(buff, "sounds/propeller.wav");
	this->LoadSound( this->m_sEngineSound, buff, DSBCAPS_CTRLFREQUENCY );



	// Zeppelin Engine
	this->m_sZeppelinSound.m_pSound = NULL;
	this->m_sZeppelinSound.m_bPaused = false;
	this->m_sZeppelinSound.m_lVolume = 0;
	this->m_sZeppelinSound.m_lFrequency = -1;
	this->m_sZeppelinSound.m_bLooped = false;

	sprintf(buff, "sounds/zeppelin.wav");
	this->LoadSound( this->m_sZeppelinSound, buff, DSBCAPS_CTRLFREQUENCY );


	// Tracer Sound
	this->m_sTracerSound.m_pSound = NULL;
	this->m_sTracerSound.m_bPaused = false;
	this->m_sTracerSound.m_lVolume = 0;
	this->m_sTracerSound.m_lFrequency = -1;
	this->m_sTracerSound.m_bLooped = false;

	sprintf(buff, "sounds/gun.wav");
	this->LoadSound( this->m_sTracerSound, buff, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, 3 );



	// Zeppelin Tracer Sound
	this->m_sZeppelinGunSound.m_pSound = NULL;
	this->m_sZeppelinGunSound.m_bPaused = false;
	this->m_sZeppelinGunSound.m_lVolume = 0;
	this->m_sZeppelinGunSound.m_lFrequency = -1;
	this->m_sZeppelinGunSound.m_bLooped = false;

	sprintf(buff, "sounds/zeppgun.wav");
	this->LoadSound( this->m_sZeppelinGunSound, buff, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, 3 );


	// Ship Explosion
	this->m_sShipExplosionSound.m_pSound = NULL;
	this->m_sShipExplosionSound.m_bPaused = false;
	this->m_sShipExplosionSound.m_lVolume = 0;
	this->m_sShipExplosionSound.m_lFrequency = -1;
	this->m_sShipExplosionSound.m_bLooped = false;

	sprintf(buff, "sounds/bang.wav");
	this->LoadSound( this->m_sShipExplosionSound, buff, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, 2 );

	// Ship Hit
	this->m_sShipHitSound.m_pSound = NULL;
	this->m_sShipHitSound.m_bPaused = false;
	this->m_sShipHitSound.m_lVolume = 0;
	this->m_sShipHitSound.m_lFrequency = -1;
	this->m_sShipHitSound.m_bLooped = false;

	sprintf(buff, "sounds/hit.wav");
	this->LoadSound( this->m_sShipHitSound, buff, DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, 2 );
}

void CSoundController::Destroy()
{
	///////////////////////
	// Kill our sounds
	if ( this->m_sEngineSound.m_pSound )
		delete this->m_sEngineSound.m_pSound;

	if ( this->m_sZeppelinSound.m_pSound )
		delete this->m_sZeppelinSound.m_pSound;

	if ( this->m_sTracerSound.m_pSound )
		delete this->m_sTracerSound.m_pSound;

	if ( this->m_sZeppelinGunSound.m_pSound )
		delete this->m_sZeppelinGunSound.m_pSound;

	if ( this->m_sShipExplosionSound.m_pSound )
		delete this->m_sShipExplosionSound.m_pSound;

	if ( this->m_sShipHitSound.m_pSound )
		delete this->m_sShipHitSound.m_pSound;


	///////////////////////
	// Kill our sound manager
	if ( this->m_pSoundManager != NULL )
		delete this->m_pSoundManager;
	this->m_pSoundManager = NULL;
}

void CSoundController::Update( void * Data )
{
	// Update our player's engine
//	if ( ! this->m_bEnableEngine )
//	{
//		if ( this->m_sEngineSound.m_pSound->IsSoundPlaying() )
//			this->m_sEngineSound.m_pSound->Stop();
//	}
//	else if ( !this->m_sEngineSound.m_pSound->IsSoundPlaying() )
//		this->PlaySound( this->m_sEngineSound );

	if ( this->m_eEngineType == PLANE )
	{
		if ( !this->m_sEngineSound.m_pSound->IsSoundPlaying() )
			this->PlaySound( this->m_sEngineSound );
	}
	else if ( this->m_eEngineType == ZEPPELIN )
	{
		if ( !this->m_sZeppelinSound.m_pSound->IsSoundPlaying() )
			this->PlaySound( this->m_sZeppelinSound );
	}
}






void CSoundController::PlaySound( CSoundController::SoundData * data )
{
	// volume is 100 / distance^3
	if ( data->distance > s_fMaxSoundDistance )
		return;

	float fdistancecube = (float)pow( (float)(data->distance / 300.0f), 3 ) + 1.0f;
	float fvolume = 100.0f / fdistancecube;
	long volume = (long)fvolume;//(  100.0f / (  (float)pow( (float)(data->distance / 10.0f), 3 ) + 1.0f  )  );
	long freq = data->frequency;
//	volume *= data->volume;

	volume = (long)( (float)volume * s_fVolScale );

	volume = 100 - volume;

	switch ( data->sound )
	{
		case CSoundController::TRACER:
			this->m_sTracerSound.m_lVolume = volume;
			this->m_sTracerSound.m_lFrequency = volume;//freq;
			this->m_sTracerSound.m_pSound->GetFreeBuffer();
			this->PlaySound( this->m_sTracerSound );
			break;

		case CSoundController::ZEPPELIN_GUN:
			this->m_sZeppelinGunSound.m_lVolume = volume;
			this->m_sZeppelinGunSound.m_lFrequency = volume;//freq;
			this->PlaySound( this->m_sZeppelinGunSound );
			break;

		case CSoundController::SHIP_EXPLOSION:
			this->m_sShipExplosionSound.m_lVolume = volume;
			this->m_sShipExplosionSound.m_lFrequency = volume;//freq;
			this->PlaySound( this->m_sShipExplosionSound );
			break;

		case CSoundController::SHIP_HIT:
			this->m_sShipHitSound.m_lVolume = volume;
			this->m_sShipHitSound.m_lFrequency = volume;//freq;
			this->PlaySound( this->m_sShipHitSound );
			break;

		default:
			assert(0);
	}
}

void CSoundController::UpdatePlayerEngine( CSoundController::ShipData * data )
{
	long freq;
	// hack ahoy
	if ( data->type == PLANE )
	{
		freq = ( (long)data->throttle + 5 ) * 250;
	}
	else
	{
		if ( data->throttle > 50 )
			freq = ( (long)data->throttle + 5 ) * 25;			// alive
		else
			freq = 50 * 25;										// dead
	}

	this->m_eEngineType = data->type;

	// if this is different, stop the sound and restart it with the new freq
	if ( this->m_eEngineType == PLANE )
	{
		if ( this->m_sEngineSound.m_lFrequency != freq  ||  data->type != this->m_eEngineType )
		{
//		if ( this->m_eEngineType == PLANE )
//		{
			this->m_sEngineSound.m_lFrequency = freq;
			this->PlaySound( this->m_sEngineSound );

			// stop the other sound
			this->m_sZeppelinSound.m_pSound->Stop();
		}
	}
	else
	{
	//	else
		if ( this->m_sZeppelinSound.m_lFrequency != freq  ||  data->type != this->m_eEngineType )
		{
			this->m_sZeppelinSound.m_lFrequency = freq;//2625;
			this->PlaySound( this->m_sZeppelinSound );

			// stop the other sound
			this->m_sEngineSound.m_pSound->Stop();
		}
	}
}






void CSoundController::PlaySound( CSoundController::SSound &sound )
{
	if( sound.m_bPaused )
    {
        // Play the buffer since it is currently paused
        DWORD dwFlags = 0;
		
		if ( sound.m_bLooped )
			dwFlags |= DSBPLAY_LOOPING;
		else
			dwFlags |= 0L;

		if( FAILED( sound.m_pSound->Play( 0, dwFlags, sound.m_lVolume, sound.m_lFrequency ) ) )
            return;

        // Update the UI controls to show the sound as playing
		sound.m_bPaused = false;
    }
    else
    {
		if( sound.m_pSound->IsSoundPlaying() )
        {
            // To pause, just stop the buffer, but don't reset the position
            if( sound.m_pSound )
                sound.m_pSound->Stop();

			sound.m_bPaused = true;
        }
        else
        {
            // The buffer is not playing, so play it again
            DWORD dwFlags = 0;

			if ( sound.m_bLooped )
				dwFlags |= DSBPLAY_LOOPING;
			else
				dwFlags |= 0L;

			if( FAILED( sound.m_pSound->Play( 0, dwFlags, sound.m_lVolume, sound.m_lFrequency ) ) )
                return;

            // Update the UI controls to show the sound as playing
			sound.m_bPaused = false;
        }
    }

    return;
}




void CSoundController::LoadSound( CSoundController::SSound &sound, char * filename, DWORD flags, int numBuffers )
{
    // Free any previous sound, and make a new one
   if ( sound.m_pSound != NULL )
	   delete sound.m_pSound;
   sound.m_pSound = NULL;

    // Verify the file is small
    HANDLE hFile = CreateFile( filename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( hFile != NULL )
    {
        // If you try to open a 100MB wav file, you could run out of system memory with this
        // sample cause it puts all of it into a large buffer.  If you need to do this, then 
        // see the "StreamData" sample to stream the data from the file into a sound buffer.
        DWORD dwFileSizeHigh = 0;
        DWORD dwFileSize = GetFileSize( hFile, &dwFileSizeHigh );
        CloseHandle( hFile );

        if( dwFileSizeHigh != 0 || dwFileSize > 1000000 )
        {
            OutputDebugString( _T("File too large.  You should stream large files.\n") );
            return;
        }
    }

    // Load the wave file into a DirectSound buffer
	if( FAILED( this->m_pSoundManager->Create( &sound.m_pSound, filename, flags, GUID_NULL, (DWORD)numBuffers ) ) )
    {
        // Not a critical failure, so just update the status
        OutputDebugString( _T("Could not create sound buffer.\n") );
        return; 
    }
}

