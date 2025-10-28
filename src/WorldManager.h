/******************************************************************************************

                 Filename: WorldManager.h
                   Author: Adam Griffiths
              Description: Handles the different terrain meshes.
			               Much code borrowed from tutorials by Ian Foley.
						   Original code from Trent Polack (trent@voxelsoft.com)

 ******************************************************************************************/

#ifndef UFO_WORLDMANAGER_H
#define UFO_WORLDMANAGER_H

#include "UFOPrerequisites.h"
#include "UFO.h"


using namespace neurosis;



class CWorldManager : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CWorldManager();
		~CWorldManager();

		class WorldSetup
		{
			public:
//				NSNode *	pPlanetBranch;
				CTask *		pWorld;
				bool		bIsFlatWorld;
		};


		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CWorldManager * pWorldManager;


	private:
		void			Init( void * Data );
		void			Render();
		void			Destroy();

		CTask *			m_pWorld;
		NSVector3df		m_kGravity;

		bool			m_bFlatWorld;
};



#endif // #ifndef UFO_WORLDMANAGER_H
