/******************************************************************************************
                                                                                         
                 Filename: Camera.h
                   Author: Adam Griffiths
              Description: Camera class that wraps the NSOrbitCamera class and takes user
			               input.

 ******************************************************************************************/

#ifndef UFO_CAMERA_H
#define UFO_CAMERA_H

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "NSOrbitCamera.h"

using namespace neurosis;

////////////////////////
//   Start #defines

//////////////////////
// Orbit Mode
#define CAMERA_ORBIT_SCALE				0.025f
#define CAMERA_ZOOM_SCALE				0.3f//1.5f

#define CAMERA_ZOOM_MIN					4.0f	// bounding sphere + view plane at 1.0f
#define CAMERA_ZOOM_MAX					75.0f

//////////////////////
// Cockpit Mode
//#define CAMERA_COCKPIT_ZOOM				4.3f
#define CAMERA_COCKPIT_ELEVATION		0.0f
#define CAMERA_COCKPIT_ROTATION			0.0f //NS_PI	// our nodes are temporarily facing the wrong way!
//#define CAMERA_COCKPIT_LOOKATOFFSET		CAMERA_COCKPIT_ZOOM + 1.0f

//    End #defines
////////////////////////


class CCamera : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CCamera();
		~CCamera();

	protected:
		void		Init( void * Data );
		void		Update( void * Data );
		void		Destroy();


		bool			m_bOrbitMode;
		NSOrbitCamera *	m_pCamera;

		float			m_fPreviousZoom;
		float			m_fPreviousElevation;
		float			m_fPreviousRotation;

		CTask *			m_pCTaskTarget;
		bool			m_bShowDescription;

		static const float	s_fCockpitZoom;
		static const float	s_fCockpitLookAtOffset;
};


#endif // #ifndef UFO_CAMERA_H
