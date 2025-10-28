/******************************************************************************************

                 Filename: WaterPlane.h
                   Author: Adam Griffiths
              Description: A city located on the terrain

 ******************************************************************************************/

#ifndef UFO_WATERPLANE_H
#define UFO_WATERPLANE_H

#include "UFO.h"
#include "UFOPrerequisites.h"
#include "NSVertex3.h"
#include "NSVector3.h"

using namespace neurosis;




class CWaterPlane : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CWaterPlane();
		~CWaterPlane();

	private:
		void			Init(void * Data);
		void			Destroy();
		void			Update(void * Data);
		void			Render();

		ID3DXMesh *					m_pMesh;
		LPDIRECT3DTEXTURE9			m_pTexture;

		static const float	s_fSize;
};

#endif // #ifndef UFO_WATERPLANE_H

