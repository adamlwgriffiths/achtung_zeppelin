/***************************************************************************
                          NSMatrix4.h  -  description
                             -------------------
    begin                : Mon May 3 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A 4x4 Matrix for us with the translation,
                           rotation and scaling of NSSceneNodes

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSMATRIX4_H
#define NSMATRIX4_H

#include "NeurosisPrerequisites.h"
#include "NSVector3.h"

#include <d3dx9math.h>

namespace neurosis {

class NSVector3df;

class NSMatrix4
{
	// faster access
	friend class NSVector3df;
	friend class NSMatrix4;

	public:
		D3DXMATRIX	m;


		NSMatrix4()
				{};
		NSMatrix4( D3DXMATRIX m2 )
				{ this->m = m2; };
		~NSMatrix4()
				{};

		inline void		SetIdentity()
							{ D3DXMatrixIdentity( &m ); };

		inline void		YawPitchRoll( nsfloat yaw, nsfloat pitch, nsfloat roll )
							{ D3DXMatrixRotationYawPitchRoll( &m, yaw, pitch, roll ); };

		inline void		SetRotationX( nsfloat angle )
							{ D3DXMatrixRotationX( &m, angle ); };
		inline void		SetRotationY( nsfloat angle )
							{ D3DXMatrixRotationY( &m, angle ); };
		inline void		SetRotationZ( nsfloat angle )
							{ D3DXMatrixRotationZ( &m, angle ); };

		inline void		SetRotationAboutAxis( const NSVector3df v, nsfloat angle )
							{ D3DXMatrixRotationAxis( &m, &v.v, angle ); };

		inline void		SetTranslation( const NSVector3df &v )
							{ m._41 = v.v.x; m._42 = v.v.y; m._43 = v.v.z; };
		inline void		SetTranslation( nsfloat x, nsfloat y, nsfloat z )
							{ m._41 = x; m._42 = y; m._43 = z; };
		inline NSVector3df	GetTranslation()
							{ return NSVector3df( m._41, m._42, m._43 ); };

		inline void		SetScale( nsfloat x, nsfloat y, nsfloat z)
							{ D3DXMatrixScaling( &m, x, y, z ); };
		inline void		SetScale( nsfloat scale )
							{ D3DXMatrixScaling( &m, scale, scale, scale ); };
		inline void		SetScale( const NSVector3df &v )
							{ this->SetScale( v.v.x, v.v.y, v.v.z ); };

		inline void		Multiply( const NSMatrix4 &m2 )
							{ D3DXMatrixMultiply( &m, &m, &m2.m ); };

		inline nsfloat	GetDeterminant() const
							{ nsfloat val = D3DXMatrixDeterminant( &m ); return val; };

		inline void		Transpose()
							{ D3DXMatrixTranspose( &m, &m ); };

		inline D3DXMATRIX * GetDXMatrix()
						{ return &m; };


		void		SetLookAt( const NSVector3df &eye, const NSVector3df &target, const NSVector3df &up )
						{ D3DXMatrixLookAtLH( &m, &eye.v, &target.v, &up.v ); };

		void		SetLookAt( const NSVector3df &target, const NSVector3df &up )
						{ D3DXMatrixLookAtLH( &m, &this->GetTranslation().v, &target.v, &up.v ); };



		///////////////////////
		// Vector Operators
		NSVector3df operator * (const NSVector3df &v) const
        {
            NSVector3df nv;
			D3DXVec3TransformCoord( &nv.v, &v.v, &m );
            return nv;
        }

		///////////////////////
		// Matrix Operators
		NSMatrix4 *	operator = (const NSMatrix4 &m2)
			{ 
				this->m._11 = m2.m._11;
				this->m._12 = m2.m._12;
				this->m._13 = m2.m._13;
				this->m._14 = m2.m._14;

				this->m._21 = m2.m._21;
				this->m._22 = m2.m._22;
				this->m._23 = m2.m._23;
				this->m._24 = m2.m._24;

				this->m._31 = m2.m._31;
				this->m._32 = m2.m._32;
				this->m._33 = m2.m._33;
				this->m._34 = m2.m._34;

				this->m._41 = m2.m._41;
				this->m._42 = m2.m._42;
				this->m._43 = m2.m._43;
				this->m._44 = m2.m._44;
				return this;
			};


		NSMatrix4	operator * (const NSMatrix4 &m2) const
			{
				NSMatrix4 mat;
				D3DXMatrixMultiply( &mat.m, &m, &m2.m );
				return mat;
			};

		NSMatrix4 *	operator *= (const NSMatrix4 &m2)
			{
				this->m *= m2.m;
				return this;
			};

		NSMatrix4	operator + (const NSMatrix4 &m2) const
			{	
				NSMatrix4 mat;
				// hurray for not having an = operator... ffs
				mat.m._11 = this->m._11 + m2.m._11;
				mat.m._12 = this->m._12 + m2.m._12;
				mat.m._13 = this->m._13 + m2.m._13;
				mat.m._14 = this->m._14 + m2.m._14;

				mat.m._21 = this->m._21 + m2.m._21;
				mat.m._22 = this->m._22 + m2.m._22;
				mat.m._23 = this->m._23 + m2.m._23;
				mat.m._24 = this->m._24 + m2.m._24;

				mat.m._31 = this->m._31 + m2.m._31;
				mat.m._32 = this->m._32 + m2.m._32;
				mat.m._33 = this->m._33 + m2.m._33;
				mat.m._34 = this->m._34 + m2.m._34;

				mat.m._41 = this->m._41 + m2.m._41;
				mat.m._42 = this->m._42 + m2.m._42;
				mat.m._43 = this->m._43 + m2.m._43;
				mat.m._44 = this->m._44 + m2.m._44;
				return mat;
			};

		NSMatrix4 *	operator += (const NSMatrix4 &m2)
			{
				this->m += m2.m;
				return this;
			};

		NSMatrix4	operator - (const NSMatrix4 &m2) const
			{
				NSMatrix4 mat;
				mat.m._11 = this->m._11 - m2.m._11;
				mat.m._12 = this->m._12 - m2.m._12;
				mat.m._13 = this->m._13 - m2.m._13;
				mat.m._14 = this->m._14 - m2.m._14;

				mat.m._21 = this->m._21 - m2.m._21;
				mat.m._22 = this->m._22 - m2.m._22;
				mat.m._23 = this->m._23 - m2.m._23;
				mat.m._24 = this->m._24 - m2.m._24;

				mat.m._31 = this->m._31 - m2.m._31;
				mat.m._32 = this->m._32 - m2.m._32;
				mat.m._33 = this->m._33 - m2.m._33;
				mat.m._34 = this->m._34 - m2.m._34;

				mat.m._41 = this->m._41 - m2.m._41;
				mat.m._42 = this->m._42 - m2.m._42;
				mat.m._43 = this->m._43 - m2.m._43;
				mat.m._44 = this->m._44 - m2.m._44;
				return mat;
			}

		NSMatrix4 *	operator -= (const NSMatrix4 &m2)
			{
				this->m -= m2.m;
				return this;
			};

		NSMatrix4	operator / (nsfloat val) const
			{
				NSMatrix4 mat;
				mat.m._11 = this->m._11 / val;
				mat.m._12 = this->m._12 / val;
				mat.m._13 = this->m._13 / val;
				mat.m._14 = this->m._14 / val;

				mat.m._21 = this->m._21 / val;
				mat.m._22 = this->m._22 / val;
				mat.m._23 = this->m._23 / val;
				mat.m._24 = this->m._24 / val;

				mat.m._31 = this->m._31 / val;
				mat.m._32 = this->m._32 / val;
				mat.m._33 = this->m._33 / val;
				mat.m._34 = this->m._34 / val;

				mat.m._41 = this->m._41 / val;
				mat.m._42 = this->m._42 / val;
				mat.m._43 = this->m._43 / val;
				mat.m._44 = this->m._44 / val;
				return mat;
			}

		NSMatrix4 *	operator /= (nsfloat val)
			{
				this->m /=  val;
				return this;
			};

		bool			operator == (const NSMatrix4 &m2) const
			{
				if (this->m == m2.m)
					return true;
				return false;
			};

		bool		operator != (const NSMatrix4 &m2) const
			{
				if (this->m != m2.m)
					return true;
				return false;
			};
};

/*
class NSMatrix4
{
		// allows for direct access to matrix variable
		friend class NSMatrix4;

	protected:

		//		col
		//row	0  1  2  3
		//		4  5  6  7
		//		8  9  10 11
		//		12 13 14 15
		nsfloat		matrix[16];


	public:
		inline void			SetIdentity()
			{	this->matrix[0] = 1.0f; this->matrix[1] = 0.0f; this->matrix[2] = 0.0f; this->matrix[3] = 0.0f;
				this->matrix[4] = 0.0f; this->matrix[5] = 1.0f; this->matrix[6] = 0.0f; this->matrix[7] = 0.0f;
				this->matrix[8] = 0.0f; this->matrix[9] = 0.0f; this->matrix[10] = 1.0f; this->matrix[11] = 0.0f;
				this->matrix[12] = 0.0f; this->matrix[13] = 0.0f; this->matrix[14] = 0.0f; this->matrix[15] = 1.0f;
			};
		inline void			Reset()
			{ for (u32 i = 0; i < 16; i++) this->matrix[i] = 0.0f; };

		// [ 1,  0,  0, 0]
		// [ 0,  1,  0, 0]
		// [ 0,  0,  1, 0]
		// [tx, ty, tz, 1]
		inline void			SetTranslation( const NSVector3df &translation )
			{	this->matrix[12] = translation.x;
				this->matrix[13] = translation.y;
				this->matrix[14] = translation.z; };

		inline void			SetTranslation( nsfloat x, nsfloat y, nsfloat z )
			{	this->matrix[12] = x;
				this->matrix[13] = y;
				this->matrix[14] = z; };

		inline NSVector3df		GetTranslation()
			{	return NSVector3df( this->matrix[12], this->matrix[13], this->matrix[14] ); };

		// [ 1,         0,          0, 0]
		// [ 0, cos(xrot),  sin(xrot), 0]
		// [ 0,-sin(xrot),  cos(xrot), 0]
		// [ 0,         0,          0, 1]
		inline void			SetRotationX( nsfloat xrot )
			{	this->matrix[5] = cos(xrot);	this->matrix[6] = sin(xrot);
				this->matrix[9] = -sin(xrot);	this->matrix[10] = cos(xrot); };


		// [ cos(yrot), 0,-sin(yrot), 0]
		// [         0, 1,         0, 0]
		// [ sin(yrot), 0, cos(yrot), 0]
		// [         0, 0,         0, 1]
		inline void			SetRotationY( nsfloat yrot )
			{	this->matrix[0] = cos(yrot);	this->matrix[2] = -sin(yrot);
				this->matrix[8] = sin(yrot);	this->matrix[10] = cos(yrot); };

		// [ cos(zrot),  sin(zrot), 0, 0]
		// [-sin(zrot),  cos(zrot), 0, 0]
		// [         0,          0, 1, 0]
		// [         0,          0, 0, 1]
		inline void			SetRotationZ( nsfloat zrot )
			{	this->matrix[0] = cos(zrot);	this->matrix[1] = sin(zrot);
				this->matrix[4] = -sin(zrot);	this->matrix[5] = cos(zrot); };

		// [ sx,  0,  0, 0]
		// [  0, sy,  0, 0]
		// [  0,  0, sz, 0]
		// [  0,  0,  0, 1]
		inline void			SetScale( const NSVector3df scale )
			{	this->matrix[0] = scale.x;
				this->matrix[5] = scale.y;
				this->matrix[10] = scale.z; };

		inline void			SetScale( const nsfloat scalex, const nsfloat scaley, const nsfloat scalez )
			{	this->matrix[0] = scalex;
				this->matrix[5] = scaley;
				this->matrix[10] = scalez; };

		inline void			SetScale( nsfloat scale )
			{	this->matrix[0] = scale;
				this->matrix[5] = scale;
				this->matrix[10] = scale; };

		inline NSVector3df	GetScale()
			{	return NSVector3df ( this->matrix[0], this->matrix[5], this->matrix[10] ); };


        inline NSMatrix4	Transpose() const
			{	return NSMatrix4 (	matrix[0], matrix[4], matrix[8], matrix[12],
									matrix[1], matrix[5], matrix[9], matrix[13],
									matrix[2], matrix[6], matrix[10], matrix[14],
									matrix[3], matrix[7], matrix[11], matrix[15] ); };



		inline NSMatrix4	Multiply( const NSMatrix4 &m ) const
			{
				// gloriously liberated from OGRE's OgreMatrix4.h
				// www.ogre3d.org
				NSMatrix4 m2;
				m2.matrix[0] = matrix[0] * m.matrix[0] + matrix[1] * m.matrix[4] + matrix[2] * m.matrix[8] + matrix[3] * m.matrix[12];
				m2.matrix[1] = matrix[0] * m.matrix[1] + matrix[1] * m.matrix[5] + matrix[2] * m.matrix[9] + matrix[3] * m.matrix[13];
				m2.matrix[2] = matrix[0] * m.matrix[2] + matrix[1] * m.matrix[6] + matrix[2] * m.matrix[10] + matrix[3] * m.matrix[14];
				m2.matrix[3] = matrix[0] * m.matrix[3] + matrix[1] * m.matrix[7] + matrix[2] * m.matrix[11] + matrix[3] * m.matrix[15];

				m2.matrix[4] = matrix[4] * m.matrix[0] + matrix[5] * m.matrix[4] + matrix[6] * m.matrix[8] + matrix[7] * m.matrix[12];
				m2.matrix[5] = matrix[4] * m.matrix[1] + matrix[5] * m.matrix[5] + matrix[6] * m.matrix[9] + matrix[7] * m.matrix[13];
				m2.matrix[6] = matrix[4] * m.matrix[2] + matrix[5] * m.matrix[6] + matrix[6] * m.matrix[10] + matrix[7] * m.matrix[14];
				m2.matrix[7] = matrix[4] * m.matrix[3] + matrix[5] * m.matrix[7] + matrix[6] * m.matrix[11] + matrix[7] * m.matrix[15];

				m2.matrix[8] = matrix[8] * m.matrix[0] + matrix[9] * m.matrix[4] + matrix[10] * m.matrix[8] + matrix[11] * m.matrix[12];
				m2.matrix[9] = matrix[8] * m.matrix[1] + matrix[9] * m.matrix[5] + matrix[10] * m.matrix[9] + matrix[11] * m.matrix[13];
				m2.matrix[10] = matrix[8] * m.matrix[2] + matrix[9] * m.matrix[6] + matrix[10] * m.matrix[10] + matrix[11] * m.matrix[14];
				m2.matrix[11] = matrix[8] * m.matrix[3] + matrix[9] * m.matrix[7] + matrix[10] * m.matrix[11] + matrix[11] * m.matrix[15];

				m2.matrix[12] = matrix[12] * m.matrix[0] + matrix[13] * m.matrix[4] + matrix[14] * m.matrix[8] + matrix[15] * m.matrix[12];
				m2.matrix[13] = matrix[12] * m.matrix[1] + matrix[13] * m.matrix[5] + matrix[14] * m.matrix[9] + matrix[15] * m.matrix[13];
				m2.matrix[14] = matrix[12] * m.matrix[2] + matrix[13] * m.matrix[6] + matrix[14] * m.matrix[10] + matrix[15] * m.matrix[14];
				m2.matrix[15] = matrix[12] * m.matrix[3] + matrix[13] * m.matrix[7] + matrix[14] * m.matrix[11] + matrix[15] * m.matrix[15];

				return m2;
			}


// QND hack for DX
#ifdef NEUROSIS_DIRECTX
		inline D3DXMATRIXA16		ToDXMatrix()
			{
				D3DXMATRIXA16 mat;
				mat.m[0][0] = this->matrix[0];
				mat.m[0][1] = this->matrix[1];
				mat.m[0][2] = this->matrix[2];
				mat.m[0][3] = this->matrix[3];

				mat.m[1][0] = this->matrix[4];
				mat.m[1][1] = this->matrix[5];
				mat.m[1][2] = this->matrix[6];
				mat.m[1][3] = this->matrix[7];

				mat.m[2][0] = this->matrix[8];
				mat.m[2][1] = this->matrix[9];
				mat.m[2][2] = this->matrix[10];
				mat.m[2][3] = this->matrix[11];

				mat.m[3][0] = this->matrix[12];
				mat.m[3][1] = this->matrix[13];
				mat.m[3][2] = this->matrix[14];
				mat.m[3][3] = this->matrix[15];

				return mat;
			};
#endif // #ifdef NEUROSIS_DIRECTX



		inline NSMatrix4()
			{ this->SetIdentity(); };

		inline NSMatrix4( NSMatrix4 & m )
			{
				this->matrix[0] = m.matrix[0]; this->matrix[1] = m.matrix[1]; this->matrix[2] = m.matrix[2]; this->matrix[3] = m.matrix[3];
				this->matrix[4] = m.matrix[4]; this->matrix[5] = m.matrix[5]; this->matrix[6] = m.matrix[6]; this->matrix[7] = m.matrix[7];
				this->matrix[8] = m.matrix[8]; this->matrix[9] = m.matrix[9]; this->matrix[10] = m.matrix[10]; this->matrix[11] = m.matrix[11];
				this->matrix[12] = m.matrix[12]; this->matrix[13] = m.matrix[13]; this->matrix[14] = m.matrix[14]; this->matrix[15] = m.matrix[15];
			};

		inline NSMatrix4(	nsfloat m0, nsfloat m1, nsfloat m2, nsfloat m3,
							nsfloat m4, nsfloat m5, nsfloat m6, nsfloat m7,
							nsfloat m8, nsfloat m9, nsfloat m10, nsfloat m11,
							nsfloat m12, nsfloat m13, nsfloat m14, nsfloat m15 )
			{
				this->matrix[0] = m0; this->matrix[1] = m1; this->matrix[2] = m2; this->matrix[3] = m3;
				this->matrix[4] = m4; this->matrix[5] = m5; this->matrix[6] = m6; this->matrix[7] = m7;
				this->matrix[8] = m8; this->matrix[9] = m9; this->matrix[10] = m10; this->matrix[11] = m11;
				this->matrix[12] = m12; this->matrix[13] = m13; this->matrix[14] = m14; this->matrix[15] = m15;
			};

		inline ~NSMatrix4() {};





		// Operator that lets you access the matrix by way of `matrix(row, col)'
		// valid values for `row' and `col' are 0 - 3, ~~not~~ 1 - 4!!
		inline nsfloat &			operator () (nsint row, nsint col) { return this->matrix[ (row * 4) + col ]; };
		inline const nsfloat &		operator () (nsint row, nsint col) const { return matrix[ (row * 4) + col ]; };
		inline nsfloat &			operator [] (nsint val) { return this->matrix[val]; };


		///////////////////////
		// Vector Operators
		inline NSVector3df operator * (const NSVector3df &v) const
        {
            NSVector3df nv;
            nsfloat scale = 1.0f / ( matrix[3] * v.x + matrix[7] * v.y + matrix[11] * v.z + matrix[15] );

            nv.x = ( matrix[0] * v.x + matrix[4] * v.y + matrix[8] * v.z + matrix[12] ) * scale;
            nv.y = ( matrix[1] * v.x + matrix[5] * v.y + matrix[9] * v.z + matrix[13] ) * scale;
            nv.z = ( matrix[2] * v.x + matrix[6] * v.y + matrix[10] * v.z + matrix[14] ) * scale;

            return nv;
        }

		///////////////////////
		// Matrix Operators
		inline NSMatrix4 *	operator = (const NSMatrix4 &m)
			{ 
				this->matrix[0] = m.matrix[0];
				this->matrix[1] = m.matrix[1];
				this->matrix[2] = m.matrix[2];
				this->matrix[3] = m.matrix[3];

				this->matrix[4] = m.matrix[4];
				this->matrix[5] = m.matrix[5];
				this->matrix[6] = m.matrix[6];
				this->matrix[7] = m.matrix[7];

				this->matrix[8] = m.matrix[8];
				this->matrix[9] = m.matrix[9];
				this->matrix[10] = m.matrix[10];
				this->matrix[11] = m.matrix[11];

				this->matrix[12] = m.matrix[12];
				this->matrix[13] = m.matrix[13];
				this->matrix[14] = m.matrix[14];
				this->matrix[15] = m.matrix[15];
				return this;
			};
		inline NSMatrix4	operator + (const NSMatrix4 &m) const
			{
				NSMatrix4 mat;

				mat.matrix[0] = this->matrix[0] + m.matrix[0];
				mat.matrix[1] = this->matrix[1] + m.matrix[1];
				mat.matrix[2] = this->matrix[2] + m.matrix[2];
				mat.matrix[3] = this->matrix[3] + m.matrix[3];

				mat.matrix[4] = this->matrix[4] + m.matrix[4];
				mat.matrix[5] = this->matrix[5] + m.matrix[5];
				mat.matrix[6] = this->matrix[6] + m.matrix[6];
				mat.matrix[7] = this->matrix[7] + m.matrix[7];

				mat.matrix[8] = this->matrix[8] + m.matrix[8];
				mat.matrix[9] = this->matrix[9] + m.matrix[9];
				mat.matrix[10] = this->matrix[10] + m.matrix[10];
				mat.matrix[11] = this->matrix[11] + m.matrix[11];

				mat.matrix[12] = this->matrix[12] + m.matrix[12];
				mat.matrix[13] = this->matrix[13] + m.matrix[13];
				mat.matrix[14] = this->matrix[14] + m.matrix[14];
				mat.matrix[15] = this->matrix[15] + m.matrix[15];

				return mat;
			};

		inline NSMatrix4 *	operator += (const NSMatrix4 &m)
			{
				this->matrix[0] += m.matrix[0];
				this->matrix[1] += m.matrix[1];
				this->matrix[2] += m.matrix[2];
				this->matrix[3] += m.matrix[3];

				this->matrix[4] += m.matrix[4];
				this->matrix[5] += m.matrix[5];
				this->matrix[6] += m.matrix[6];
				this->matrix[7] += m.matrix[7];

				this->matrix[8] += m.matrix[8];
				this->matrix[9] += m.matrix[9];
				this->matrix[10] += m.matrix[10];
				this->matrix[11] += m.matrix[11];

				this->matrix[12] += m.matrix[12];
				this->matrix[13] += m.matrix[13];
				this->matrix[14] += m.matrix[14];
				this->matrix[15] += m.matrix[15];
				return this;
			};

		inline NSMatrix4	operator - (const NSMatrix4 &m) const
			{
				NSMatrix4 mat;

				mat.matrix[0] = this->matrix[0] - m.matrix[0];
				mat.matrix[1] = this->matrix[1] - m.matrix[1];
				mat.matrix[2] = this->matrix[2] - m.matrix[2];
				mat.matrix[3] = this->matrix[3] - m.matrix[3];

				mat.matrix[4] = this->matrix[4] - m.matrix[4];
				mat.matrix[5] = this->matrix[5] - m.matrix[5];
				mat.matrix[6] = this->matrix[6] - m.matrix[6];
				mat.matrix[7] = this->matrix[7] - m.matrix[7];

				mat.matrix[8] = this->matrix[8] - m.matrix[8];
				mat.matrix[9] = this->matrix[9] - m.matrix[9];
				mat.matrix[10] = this->matrix[10] - m.matrix[10];
				mat.matrix[11] = this->matrix[11] - m.matrix[11];

				mat.matrix[12] = this->matrix[12] - m.matrix[12];
				mat.matrix[13] = this->matrix[13] - m.matrix[13];
				mat.matrix[14] = this->matrix[14] - m.matrix[14];
				mat.matrix[15] = this->matrix[15] - m.matrix[15];

				return mat;
			};

		inline NSMatrix4 *	operator -= (const NSMatrix4 &m)
			{
				this->matrix[0] -= m.matrix[0];
				this->matrix[1] -= m.matrix[1];
				this->matrix[2] -= m.matrix[2];
				this->matrix[3] -= m.matrix[3];

				this->matrix[4] -= m.matrix[4];
				this->matrix[5] -= m.matrix[5];
				this->matrix[6] -= m.matrix[6];
				this->matrix[7] -= m.matrix[7];

				this->matrix[8] -= m.matrix[8];
				this->matrix[9] -= m.matrix[9];
				this->matrix[10] -= m.matrix[10];
				this->matrix[11] -= m.matrix[11];

				this->matrix[12] -= m.matrix[12];
				this->matrix[13] -= m.matrix[13];
				this->matrix[14] -= m.matrix[14];
				this->matrix[15] -= m.matrix[15];
				return this;
			};
		
		inline NSMatrix4	operator * (const NSMatrix4 &m) const
			{
				return this->Multiply(m);
			};

		inline NSMatrix4 *	operator *= (const NSMatrix4 &m)
			{
				*this = this->Multiply(m);
				return this;
			};

		inline NSMatrix4	operator / (const NSMatrix4 &m) const
			{
				NSMatrix4 mat;

				mat.matrix[0] = this->matrix[0] / m.matrix[0];
				mat.matrix[1] = this->matrix[1] / m.matrix[1];
				mat.matrix[2] = this->matrix[2] / m.matrix[2];
				mat.matrix[3] = this->matrix[3] / m.matrix[3];

				mat.matrix[4] = this->matrix[4] / m.matrix[4];
				mat.matrix[5] = this->matrix[5] / m.matrix[5];
				mat.matrix[6] = this->matrix[6] / m.matrix[6];
				mat.matrix[7] = this->matrix[7] / m.matrix[7];

				mat.matrix[8] = this->matrix[8] / m.matrix[8];
				mat.matrix[9] = this->matrix[9] / m.matrix[9];
				mat.matrix[10] = this->matrix[10] / m.matrix[10];
				mat.matrix[11] = this->matrix[11] / m.matrix[11];

				mat.matrix[12] = this->matrix[12] / m.matrix[12];
				mat.matrix[13] = this->matrix[13] / m.matrix[13];
				mat.matrix[14] = this->matrix[14] / m.matrix[14];
				mat.matrix[15] = this->matrix[15] / m.matrix[15];

				return mat;
			};

		inline NSMatrix4 *	operator /= (const NSMatrix4 &m)
			{
				this->matrix[0] /= m.matrix[0];
				this->matrix[1] /= m.matrix[1];
				this->matrix[2] /= m.matrix[2];
				this->matrix[3] /= m.matrix[3];

				this->matrix[4] /= m.matrix[4];
				this->matrix[5] /= m.matrix[5];
				this->matrix[6] /= m.matrix[6];
				this->matrix[7] /= m.matrix[7];

				this->matrix[8] /= m.matrix[8];
				this->matrix[9] /= m.matrix[9];
				this->matrix[10] /= m.matrix[10];
				this->matrix[11] /= m.matrix[11];

				this->matrix[12] /= m.matrix[12];
				this->matrix[13] /= m.matrix[13];
				this->matrix[14] /= m.matrix[14];
				this->matrix[15] /= m.matrix[15];
				return this;
			};
};
*/

}; // namespace neurosis

#endif // ifndef NSMATRIX_H
