/***************************************************************************
                          NSLight.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A scene light. Inherits from NSNode.

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

//#ifdef NSNODE_H
//#	error NSNODE_H cross reference lock!
//#endif

#ifndef NSLIGHT_H
#define NSLIGHT_H

#include "NeurosisPrerequisites.h"

#include "NSNode.h"
#include "NSColour.h"


namespace neurosis {



class NSLight : public NSNode
{
	public:

		enum NSLIGHT_TYPE
		{
			NSLIGHT_AMBIENT,
			NSLIGHT_OMNI,
			NSLIGHT_DIRECTIONAL,
			NSLIGHT_SPOT,
		};

		NSLight( LPDIRECT3DDEVICE9 device, char * name, nsfloat tx, nsfloat ty, nsfloat tz, const NSColourf &colour, nsfloat range, nsfloat attenuation );
		NSLight( LPDIRECT3DDEVICE9 device, char * name, nsfloat tx, nsfloat ty, nsfloat tz, LIGHT_TYPE type, const NSColourf &colour, nsfloat range, nsfloat attenuation );
		~NSLight();

		void			SetColour( const NSColourf &colour );
		NSColourf &		GetColour();

		void			SetType( LPDIRECT3DDEVICE9 device, NSLight::LIGHT_TYPE type );
		NSLIGHT_TYPE	GetType();

		void			SetDirection( NSVector3df & dir );
		NSVector3df &	GetDirection();

		void			Enable( LPDIRECT3DDEVICE9 device );
		void			Disable( LPDIRECT3DDEVICE9 device );

		/** Start Internal Use Only **/
		virtual void	_Update( NSMatrix4 &rotation, NSVector3df &translation );
		/** End Internal Use Only **/

	protected:
		D3DLIGHT9		m_kLight;

		NSLIGHT_TYPE	m_eLightType;
		NSVector3df		m_kDirection;
		int				m_iLightNum;

		static int		s_iNumLights;
};

int NSLight::s_iNumLights = 0;



}; // namespace neurosis


#endif // #ifndef NSLIGHT_H
