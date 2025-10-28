/******************************************************************************************

                 Filename: UFO.h
                   Author: Adam Griffiths
              Description: Includes all external and game headers and provides a central
                           point for all #defines that provide major behavior changes.
						   Also prevents having to #include <windows.h> multiple times to
						   get windows typedefs!

 ******************************************************************************************/

#ifndef UFO_H
#define UFO_H


#define WIN32_LEAN_AND_MEAN									// get rid of crappy MFC


////////////////////////////////////
//          #includes
////////////////////////////////////
#include <windows.h>										// include the cursed windows(R)(TM)(C)(whatever) header files
#include <commctrl.h>
#include <stdio.h>

#include <math.h>
#include <tchar.h>
#include <assert.h>

#include <D3DX9.h>											// DirectX includes
#include "d3dapp/dxutil.h"
#include "d3dapp/d3denumeration.h"
#include "d3dapp/d3dsettings.h"
#include "d3dapp/d3dfile.h"
#include "d3dapp/d3dfont.h"
#include "d3dapp/d3dutil.h"
//#include "d3dapp/d3dres.h"		// these are defined in resource.h
#include "d3dapp/d3dapp.h"
#include "d3dapp/dsutil.h"

#include <basetsd.h>
//#include <commdlg.h>
#include <mmreg.h>
#include <dxerr9.h>
#include <dsound.h>											// Direct Sound



////////////////////////////////////
//       Library includes
////////////////////////////////////
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dxof.lib")
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dxerr9.lib")
//dxerr9.lib d3dx9dt.lib odbc32.lib odbccp32.lib

////////////////////////////////////
//        Game #defines
////////////////////////////////////



////////////////////////////////
//  Initial Screen Dimentions
#define UFOMAIN_INITIAL_SCREENWIDTH		800					// Initial screen width
#define UFOMAIN_INITIAL_SCREENHEIGHT	600					// Initial screen height


////////////////////////////////
// Direct Play VS Windows Events
//#define INPUT_WINEVENT										// COMMENT OUT TO USE DIRECT PLAY

#ifndef INPUT_WINEVENT										// Windows Event input VS DirectPlay
#	define INPUT_DIRECTINPUT
#	undef INPUT_WINEVENT
#endif // #ifndef INPUT_WINEVENT

#ifdef INPUT_DIRECTINPUT
#	define DIRECTINPUT_VERSION 0x0800

#	include <basetsd.h>
#	include <dinput.h>

#	pragma comment (lib, "dinput8.lib")

#endif // #ifndef INPUT_WINEVENT


/////////////////////////////////
// Indicies
//#define INDEX_32BIT											// COMMENT OUT FOR NON-32bit INDICES



/////////////////////////////////
//        Frame Rate
#define DISPLAY_FRAMERATE									// COMMENT OUT TO HIDE THE FRAME RATE

#ifdef DISPLAY_FRAMERATE

#endif // #ifdef DISPLAY_FRAMERATE


/////////////////////////////////
//       View Distance
#define VIEW_DISTANCE						10000.0f


/////////////////////////////////
//          Direct-X

// causes compile problems.. so well leave it out
//#define D3DX_ALIGN16 __declspec(align(16))					// 16 bit alignment performs faster, however, it may not compile on < .Net

//#define NEUROSIS_DIRECTX									// tell neurosis were using Direct-X
//
//#ifndef NEUROSIS_DIRECTX
//#	define NEUROSIS_OPENGL
//#	undef NEUROSIS_DIRECTX
//#endif // #ifndef NEUROSIS_DIRECTX









////////////////////////////////////
//  Neurosis 3D Engine #includes
////////////////////////////////////
//#include "Neurosis.h"



////////////////////////////////////
//        Game #includes
////////////////////////////////////
#include "../resource.h"

#include "Task.h"
#include "UFOMain.h"
#include "Camera.h"
#include "GameController.h"
#include "InputController.h"
#include "MeshManager.h"
#include "Ship.h"
#include "Zeppelin.h"
#include "ShipController.h"
//#include "fractalsamp.h"
//#include "HeightMap.h"
//#include "World.h"
#include "WorldFlat.h"
#include "WorldSphere.h"
#include "WorldManager.h"
#include "CityManager.h"
#include "ParticleManager.h"
#include "HUD.h"
#include "SoundController.h"
//#include "SoundLayer.h"




#endif // #ifndef UFO_H
