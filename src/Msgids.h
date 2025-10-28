/******************************************************************************************

                 Filename: Msgids.h
                   Author: Adam Griffiths
              Description: Defines all messages posted via the `PostMessageMessage'
                           method in true game programming style.

 ******************************************************************************************/

#ifndef UFO_MSGIDS_H
#define UFO_MSGIDS_H

// Core Messages
#define		MSGID_INIT						1
#define		MSGID_DESTROY					2
#define		MSGID_UPDATE					3
#define		MSGID_RENDER					4
//#define		MSGID_CREATE					5			// initialise any static objects needed by the class, easier than writing a task factory

// Scene Messages
#define		MSGID_INIT_DEVICE_OBJECTS		20			// create any objects that require the device
#define		MSGID_DELETE_DEVICE_OBJECTS		21			// destroy any objects that we used INIT_DEVICE_OBJECTS to create
#define		MSGID_RESTORE_DEVICE_OBJECTS	22			// create and objects that were temporarily destroyed
#define		MSGID_INVALIDATE_DEVICE_OBJECTS	23			// destory any objects before we temporarily destroy the device

// Input Handling Messages
#define		MSGID_INPUT_KEY_PRESSED			40			// a key was pressed, Data is a WPARAM
#define		MSGID_INPUT_KEY_RELEASED		41			// a key was released, Data is a WPARAM
#define		MSGID_GET_KEYS					42			// returns the key struct

// UFOMain Messages
#define		MSGID_GET_SCREENWIDTH			50			// get the width of the render device
#define		MSGID_GET_SCREENHEIGHT			51			// get the height of the render device
#define		MSGID_GET_TIMEDELTA				52			// get the time delta from the last frame till now
#define		MSGID_GET_TIME					53			// get the current time since the app has started
#define		MSGID_SET_HINST					54			// set the HINSTANCE
#define		MSGID_GET_D3DDEVICE				55			// returns the D3D device pointer
#define		MSGID_GET_HWND					56			// Get the window handle
#define		MSGID_RESTART					57			// Restart the game
#define		MSGID_GET_CAPS					58			// Get the card's capabilities
#define		MSGID_GET_CLEARCOLOUR			59			// gets the screen clear colour

// MeshManager Messages
#define		MSGID_GET_MESH					60			// returns an NSMesh

// ShipController Messages
#define		MSGID_CREATE_SHIP				70			// create and return a new ship
#define		MSGID_GET_VELOCITY				71			// get the ships velocity
#define		MSGID_CHECK_COLLISIONSSPHEREVSSHIP	72		// check for any collisions and update accordingly
#define		MSGID_CHECK_COLLISIONSSPHEREVSGROUND	73	// Perform a sphere vs plane check between the ships and the ground
#define		MSGID_CHECK_COLLISIONSSPHEREVSBULLET	74
#define		MSGID_CHECK_COLLISIONSHIPVSCITY	75
#define		MSGID_NOTIFY_OF_DEATH			76
//#define		MSGID_ASSIGN_TARGETS			77
#define		MSGID_REQUEST_DELETION			78
#define		MSGID_CREATE_ZEPPELIN			79
#define		MSGID_GET_NUMFRIENDLYSHIPS		220			// friendly
#define		MSGID_GET_FRIENDLYSHIPSARRAY	221
#define		MSGID_GET_NUMFRIENDLYCITIES		222
#define		MSGID_GET_FRIENDLYCITIESARRAY	223
#define		MSGID_GET_NUMENEMYSHIPS			224			// enemy
#define		MSGID_GET_ENEMYSHIPSARRAY		225
#define		MSGID_GET_NUMENEMYCITIES		256
#define		MSGID_GET_ENEMYCITIESARRAY		257
#define		MSGID_GET_NUMFRIENDLYZEPPELINS	258
#define		MSGID_GET_FRIENDLYZEPPELINSARRAY	259
#define		MSGID_GET_NUMENEMYZEPPELINS		260
#define		MSGID_GET_ENEMYZEPPELINSARRAY	261
#define		MSGID_GET_PLAYERSHIP			262
#define		MSGID_CHECK_COLLISIONSSPHEREVSZEPPELIN	263
#define		MSGID_CHECK_COLLISIONSZEPPELINVSBULLET	264
#define		MSGID_SPAWN_SHIPS				265
#define		MSGID_NOTIFY_OF_ZEPPELINDEATH	266
#define		MSGID_CYCLE_CAMERA				267
#define		MSGID_SPAWN_ZEPPELINS			268
#define		MSGID_GET_STATE_DESCRIPTION		269

// Ship Messages
#define		MSGID_GET_NODE					80			// get the node for this task
#define		MSGID_GET_BOUNDINGRADIUS		81			// Get the radius of the bounding sphere
#define		MSGID_APPLY_DAMAGE				82			// Apply a damage value to the ship
#define		MSGID_GET_MASS					83			// Get the ships mass value
#define		MSGID_SET_COLLIDING				84			// tell the ship it is colliding
#define		MSGID_GET_BULLETNODE			85
#define		MSGID_GET_ALLIANCE				86
#define		MSGID_GET_CAMERA				87
#define		MSGID_GET_TARGET				88
#define		MSGID_SET_ARRAYPOSITION			89
#define		MSGID_GET_ARRAYPOSITION			200
#define		MSGID_IS_PLAYERSHIP				201
#define		MSGID_GET_SHIPTYPE				202
#define		MSGID_NOTIFY_CITYDEATH			203
#define		MSGID_GET_BOUNDINGMATRIX		204
#define		MSGID_IS_ALIVE					205
#define		MSGID_GET_THROTTLE				206
#define		MSGID_IS_FIRING					207
#define		MSGID_NOTIFY_FIRING				208
#define		MSGID_GET_HEALTH				209

// Camera Messages
#define		MSGID_SET_TARGET				90			// set the cameras target
#define		MSGID_GET_UPAXIS				91			// Get the camera's up axis
#define		MSGID_GET_LOOKAT				92			// Get the camera's look at point
#define		MSGID_GET_TRANSLATION			93			// Get the camera's local translation
#define		MSGID_GET_WORLDTRANSLATION		94			// Get the camera's world translation
#define		MSGID_CAMERA_SWITCHVIEW			95			// Alternate way to switch camera view

// World Manager Messages
#define		MSGID_GET_WORLD					100			// Get a pointer to the current world
#define		MSGID_IS_FLATWORLD				101			// Returns true if the world is flat
#define		MSGID_GET_NUMROWS				102			// Returns the number of rows / stacks
#define		MSGID_GET_NUMCOLS				103			// Returns the number of columns / slices
#define		MSGID_GET_VERTEXATTOPLEFTOFCELL	104			// Returns a vertex at the top of a cell
#define		MSGID_GET_HEIGHTAT				105			// Gets the height at a specific X, Y point that corresponds to a vertex
#define		MSGID_GET_RADIUS				106			// Gets the radius of the spherical world
#define		MSGID_GET_MOUNTAINLEVEL			107
#define		MSGID_GET_HEIGHTAT_FLOAT		108
#define		MSGID_GET_MINHEIGHT				109

// Tracer Messages
#define		MSGID_IS_LIFETIMEEXPIRED		120
#define		MSGID_SHOW_TRACER				121
#define		MSGID_HIDE_TRACER				122
#define		MSGID_IS_VISIBLE				123
#define		MSGID_GET_DAMAGEAMOUNT			124
#define		MSGID_GET_OWNER					125
#define		MSGID_TRACER_HIT				126

// City Manager Messages
#define		MSGID_CHECK_COLLISIONSBOXVSBULLET	140

// Particle Manager Messages
#define		MSGID_CREATE_PSYSTEM			150
#define		MSGID_DELETE_PSYSTEM			151

// HUD
#define		MSGID_SHOW_SPAWNTIMER			300
#define		MSGID_HIDE_SPAWNTIMER			301
#define		MSGID_UPDATE_PLAYERSTATS		302
#define		MSGID_UPDATE_SHIPSTATS			303
#define		MSGID_UPDATE_CITYSTATS			304
#define		MSGID_UPDATE_CAMERASTATS		305

// Turret Messages
#define		MSGID_DISABLE					320
#define		MSGID_ENABLE					321

// Sound Controller
#define		MSGID_UPDATE_PLAYERENGINE		340
//#define		MSGID_DISABLE_PLAYERENGINE		341
//#define		MSGID_ENABLE_PLAYERENGINE		342
#define		MSGID_PLAY_SOUND				343
#define		MSGID_GAMEOVER_FRIENDLYNOSHIPS	344
#define		MSGID_GAMEOVER_ENEMYNOSHIPS		345
#define		MSGID_GAMEOVER_FRIENDLYNOCITIES	346
#define		MSGID_GAMEOVER_ENEMYNOCITIES	347


#endif // #ifndef UFO_MSGIDS_H
