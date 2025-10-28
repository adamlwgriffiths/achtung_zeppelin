/***************************************************************************
                          UFOPrerequisites.h  -  description
                             -------------------
    begin                : Sun September 12 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : Basic UFO typedefs and pre-definitions for
                           pointers to use.
						   Helps prevent circular dependencies.
						   Helps GREATLY.
						   thank god for OGRE! (www.ogre3d.org)

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef UFO_PREREQUISITES_H
#define UFO_PREREQUISITES_H


////////////////////////
// Typedef's for Primitive Types
// provides a central place for alliegences
enum ALLIANCE
{
	FRIENDLY,
	ENEMY
};

enum SHIP_TYPE
{
	PLANE,
	ZEPPELIN,
};

//#include "Task.h"

//class CHeightMap;
class CCamera;


#endif // #ifndef UFO_PREREQUISITES_H
