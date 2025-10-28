/******************************************************************************************
                                                                                         
                 Filename: ShipPlayer.h
                   Author: Adam Griffiths
              Description: Player based ship class, uses input from InputController.

 ******************************************************************************************/

#ifndef UFO_SHIPPLAYER_H
#define UFO_SHIPPLAYER_H

#include "UFO.h"
//#include "Ship.h"

class CShip;

class CShipPlayer : public CShip
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CShipPlayer();
		~CShipPlayer();

	protected:
		void			UpdateInput( float fTimeDelta );	// handle player input
};

#endif // #ifndef UFO_SHIPPLAYER_H
