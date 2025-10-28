/***************************************************************************
                          NSPrerequisites.h  -  description
                             -------------------
    begin                : Sun May 2 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : Basic Neurosis typedefs and pre-definitions for
                           pointers to use.
						   Helps prevent circular dependencies.
						   Helps GREATLY.
						   thank god for OGRE! (www.ogre3d.org)

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSPREREQUISITES_H
#define NSPREREQUISITES_H


#ifndef NULL
#	define NULL		0
#endif // #ifndef NULL

#include <tchar.h>
#include <assert.h>

// No point using this since it would be abstracted through OO totally if i had enough time.
// who wants #defines in their code?!
//#ifdef NEUROSIS_DIRECTX
#	include <D3DX9.h>		// BAH!
//#endif // #ifdef NEUROSIS_DIRECTX

////////////////////////
// Typedef's for Primitive Types
#include "NSTypes.h"
#include "NSMaths.h"


////////////////////////
// Predeclare Classes
// Allows use of pointers in header files without including individual .h
// so decreases dependencies between files
namespace neurosis {

//class NSVertex3df;
//class NSVector3df;
class NSNode;
class NSElement;
//class NSLight;
class NSCamera;
class NSOrbitCamera;

class NSPlane;
class NSSphere;

}; // neurosis

#endif // #ifndef NSPREREQUISITES_H