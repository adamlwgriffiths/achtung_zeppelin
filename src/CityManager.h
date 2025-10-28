/******************************************************************************************

                 Filename: CityManager.h
                   Author: Adam Griffiths
              Description: Controls the creation, destruction, updating and general
			               management of cities.

 ******************************************************************************************/

#ifndef UFO_CITYMANAGER_H
#define UFO_CITYMANAGER_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "City.h"
#include "NSNode.h"

using namespace neurosis;

class CCity;

class CCityManager : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CCityManager();
		~CCityManager();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CCityManager * pCityManager;


	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);

		void			CheckCollisionVsShip( CTask * ship );
		void				CheckCollisionShip( CTask * c, CTask * s );	// called by VsShip
		void			CheckCollisionVsBullet( CTask * bullet );
		void				CheckCollisionBullet( CTask * c, CTask * bullet );

		void			NotifyShipsOfRemovedCity( int arraypos, ALLIANCE alliance );


		NSNode *		m_pCityBranch;

		CCity **		m_pFriendlyCities;
		int				m_iNumFriendlyCities;

		CCity **		m_pEnemyCities;
		int				m_iNumEnemyCities;
};

#endif // #ifndef UFO_CITYMANAGER_H

