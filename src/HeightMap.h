/******************************************************************************************

                 Filename: HeightMap.h
                   Author: Adam Griffiths
              Description: Height map class, create and holds a heightmap for terrain
			               meshes to use.
						   This file makes use of the Perlin Noise algorithm, borrowed and
						   converted to C++ from:
						   http://freespace.virgin.net/hugo.elias/models/m_perlin.htm

 ******************************************************************************************/

#ifndef UFO_HEIGHTMAP_H
#define UFO_HEIGHTMAP_H

#include <stdlib.h>				// needed for rand()

#include "UFOPrerequisites.h"
#include "UFO.h"
#include "NSVector3.h"

using namespace neurosis;



class CHeightMap : public CTask
{
	public:
		virtual void *	PostMessage(int ID, void * Data);

		CHeightMap();
		~CHeightMap();

		class HeightMapSetup
		{
			public:
				int iWidth;
				int iHeight;
				int iOctaves;
				float fPersistance;
		};

		class HeightAt
		{
			public:
				int x;
				int y;
		};

		struct HEIGHT_DATA
		{
			// data stored in row1-0, row1-1 ... row1-width, row2-0 ... rowHeight-Width
			unsigned char * data;			// heightmap data
			int iWidth;						//the height size (must be a power of 2)
			int iHeight;
		};
	
	protected:

		void	Init( void * Data );
		void	Destroy();


		bool	CreateNewHeightMap();
		bool	CreateDXTexture();
		void	UnloadHeightMap();
		//void	LoadHeightMap();
		//void	SaveHeightMap();


		//void	GenerateRandoms();

		float	Noise( float x, float y );
		float	Interpolate(float y1, float y2, float dist);
		float	SmoothedNoise( int x, int y );
		float	InterpolatedNoise( float x, float y );
		float	CreatePerlinNoise( int x, int y );

		unsigned char		GetHeightAt( int x, int y );


		int		m_iOctaves;
		float	m_fPersistence;

		//float *	m_fRandoms;

		int		m_iSeaLevel;

		//char	m_cFilename[20];

		int		m_iWidth;
		int		m_iHeight;

		LPDIRECT3DTEXTURE9	m_pTexture;
		HEIGHT_DATA			m_pData;
};



#endif // #ifndef UFO_HEIGHTMAP_H
