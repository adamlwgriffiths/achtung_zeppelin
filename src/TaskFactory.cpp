#include "TaskFactory.h"



// CURRENTLY EXCLUDED FROM BUILD



void CTaskFactory::AddObjectType(int ID, UNITCREATECALLBACK * Callback)
{
	CreateFuncMap[ID] = Callback;
}

void CTaskFactory::RegisterTasks()
{
	// Setup any static objects we need
	CGameController::Create();
}