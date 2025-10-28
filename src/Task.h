/******************************************************************************************
 *                                                                                        *
 *                Filename: Task.h                                                        *
 *                  Author: Adam Griffiths                                                *
 *             Description: Task class which all main objects inherit from to allow for   *
 *                          message communication.                                        *
 *                                                                                        *
 ******************************************************************************************/

#ifndef UFO_TASK_H
#define UFO_TASK_H

#include "Msgids.h"


class CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data) = 0;
};

#endif // #ifndef UFO_TASK_H
