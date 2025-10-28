/******************************************************************************************

                 Filename: UFOMain.h
                   Author: Adam Griffiths
              Description: Main entry point and base functions for UFO game and wrapper
                           around stupid D3D Application function names!

 ******************************************************************************************/

#ifndef UFO_MAIN_H
#define UFO_MAIN_H

#include "UFO.h"
#include "NSColour.h"

using namespace neurosis;


#define UFO_MAX_TIMEDELTA		0.1f


//////////////////////////////////////
// Default Game Values

#define UFO_WORLDDATA_FLATWORLD				true
#define UFO_WORLDDATA_TEXTURESIZE			512
#define UFO_WORLDDATA_MESHHEIGHTSCALE		5.0f
#define UFO_WORLDDATA_FRACTALAMPLITUDE		120
#define UFO_WORLDDATA_FRACTALROUGHNESS		2.0f

		// Flat
#define UFO_WORLDDATA_MESHWIDTH				128
#define UFO_WORLDDATA_MESHLENGTH			128
#define UFO_WORLDDATA_MESHSPACING			60

		// Spherical
#define UFO_WORLDDATA_RADIUS				1280.0f
#define UFO_WORLDDATA_NUMSLICES				70
#define UFO_WORLDDATA_NUMSTACKS				70

#define UFO_GAME_FRIENDLYFIRE				true
#define UFO_GAME_DEMOMODE					false

#define UFO_WORLDDATA_NUMINITIALCITIES		20

#define UFO_GAME_NUMSHIPS					10
#define UFO_GAME_NUMZEPPELINS				2

//////////////////////////////////////


class CUFOMain : public CTask, public CD3DApplication
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CUFOMain();
		~CUFOMain();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CUFOMain * ufomain;

		LRESULT				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


		static bool		m_bFlatWorld;
		static float	m_fMeshHeightScale;
		static int		m_iTexSize;
		static int		m_iAmplitude;
		static float	m_fRoughness;

		// Flat World
		static int		m_iMeshWidth;		// number of rows
		static int		m_iMeshLength;		// number of columns
		static int		m_iMeshSpacing;		// spacing between cells

		// Spherical World
		static float	m_fRadius;
		static int		m_iNumSlices;
		static int		m_iNumStacks;


		// Cities
		static bool		m_bFriendlyFire;
		static bool		m_bDemoMode;

		static int		m_iNumCities;
		static int		m_iMaxNumShips;
		static int		m_iNumZeppelins;


	private:
// From MSDN: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dndrive/html/directx09172001.asp
// - Init/Delete are called when you are creating/destroying a device completely.
// - Restore/Invalidate are called before/after calling Reset on a device,
//   which happens when you change some aspects of the device or have to deal with a lost device.

		HRESULT			ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT,D3DFORMAT);
		HRESULT			OneTimeSceneInit();

		HRESULT			InitDeviceObjects();		// Initialise objects that are dependent on the device, ie, textures, vertex buffers etc.
		HRESULT			DeleteDeviceObjects();		// Called to delete anything created by InitDeviceObjects

		HRESULT			RestoreDeviceObjects();		// Called if the device is destroyed and we need to re-initialise our device dependent objects.
		HRESULT			InvalidateDeviceObjects();	// Called to delete anything created by RestoreDeviceObjects

		HRESULT			Update();					// Renamed from FrameMove, Called each tick to update the scene
		HRESULT			Render();					// Called each tick to render the scene

		void			Restart();

		HRESULT			FinalCleanup();				// Called just before app exit, used to clear any remaining objects.





		HINSTANCE		hInst;						// the window instance

		NSColouri		m_kClearColour;
};

LRESULT CALLBACK AboutProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK HelpProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK SettingsProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT );	// Main entry point


#endif // #ifndef UFO_MAIN_H
