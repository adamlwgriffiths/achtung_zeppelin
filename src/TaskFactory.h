/******************************************************************************************
 *                                                                                        *
 *                Filename: TaskFactory.h                                                 *
 *                  Author: Adam Griffiths                                                *
 *             Description: Handles the initialising of objects and calling their Create  *
 *                          function to initialise any static objects they require.       *
 *                                                                                        *
 ******************************************************************************************/


// CURRENTLY EXCLUDED FROM BUILD


#ifndef UFO_TASKFACTORY_H
#define UFO_TASKFACTORY_H

#include "UFO.h"
#include "Task.h"

// include all game headers here
#include "GameController.h"



typedef CTask* (UNITCREATECALLBACK)(void);

class CTaskFactory : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		enum OBJID
		{
			TASKID_GAMECONTROLLER,
			NUM_TASKS
		};

		void RegisterTasks();
		void AddObjectType(int ID, UNITCREATECALLBACK * Callback);

	private:
		UNITCREATECALLBACK * CreateFuncMap[ NUM_TASKS ];
};

#endif // #ifndef UFO_TASKFACTORY_H
