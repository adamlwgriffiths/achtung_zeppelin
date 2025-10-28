/******************************************************************************************
                                                                                         
                 Filename: MeshManager.h
                   Author: Adam Griffiths
              Description: Handles the initialisation, storage, retrieval and
                           destruction of model buffers.

 ******************************************************************************************/

#ifndef UFO_MESHMANAGER_H
#define UFO_MESHMANAGER_H

#include "UFO.h"
//#include "NSElement.h"
#include "NSMesh.h"
#include "NSBillboard.h"


class CMeshManager : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CMeshManager();
		~CMeshManager();

		///////////////////////////
		// Singleton functions
		static void		Open();
		static void		Close();
		static CTask *	Get();

		static CMeshManager * meshmanager;


		enum MESHES
		{
			CITY_FRIEND_1,
			CITY_FRIEND_2,
			CITY_FRIEND_3,

			CITY_ENEMY_1,
			CITY_ENEMY_2,
			CITY_ENEMY_3,

			SHIP_FRIEND,
			SHIP_ENEMY,

			ZEPPELIN_FRIEND,
			ZEPPELIN_ENEMY,

			TRACER_FRIEND,
			TRACER_ENEMY,

			SHIP_PROPELLER,

			PLANE_SHRAPNEL_1,
			CITY_SHRAPNEL_1,
			SMOKE_BLACK,

			SKY_BOX,
		};

	private:
		void			Init(void * Data);
		void			Destroy();

		NSElement *		GetMesh( MESHES meshid );			// returns an instance of the mesh


		NSElement *		m_pShipFriend;
		NSElement *		m_pShipEnemy;

		NSElement *		m_pZeppelinFriend;
		NSElement *		m_pZeppelinEnemy;

		NSElement *		m_pCityFriend1;
		NSElement *		m_pCityFriend2;
		NSElement *		m_pCityFriend3;

		NSElement *		m_pCityEnemy1;
		NSElement *		m_pCityEnemy2;
		NSElement *		m_pCityEnemy3;

		NSElement *		m_pPlaneShrapnel1;
		NSElement *		m_pCityShrapnel1;

		NSElement *		m_pTracerFriend;
		NSElement *		m_pTracerEnemy;

		NSElement *		m_pPropeller;

		NSElement *		m_pSmokeBlack;

		NSElement *		m_pSkyBox;

};

#endif // #ifndef UFO_MESHMANAGER_H