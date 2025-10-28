#include "ShipPlayer.h"

void * CShipPlayer::PostMessage(int ID, void * Data)
{
	void * result = 0;

	switch(ID)
	{
		case MSGID_INIT:
			result = (void*)this->Init(Data);
			break;
		case MSGID_DESTROY:
			this->Destroy();
			break;

		default:
			CShip::PostMessage(ID, Data);		// if we dont understand it, try our parent
			break;
	}

	return result;
}

void CShipPlayer::UpdateInput( float fTimeDelta )
{
	// handle player input
}
