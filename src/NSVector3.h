/***************************************************************************
                          NSVector3d.h  -  description
                             -------------------
    begin                : Sun May 2 2004
    copyright            : (C) 2004 by Adam Griffiths
    email                : 
    description          : A based 3d vector class

         Please refer to the file `COPYING' for licensing information.
 ***************************************************************************/

#ifndef NSVECTOR3D_H
#define NSVECTOR3D_H

#include "NeurosisPrerequisites.h"
//#include "NSVertex3.h"
//#include "NSMatrix4.h"
#include <math.h>

#include <d3dx9math.h>


namespace neurosis {

//class NSMatrix4;


class NSVector3df
{
	// faster access
	friend class NSMatrix4;
	friend class NSVector3df;

	public:
		D3DXVECTOR3 v;


		NSVector3df()
				{ v.x = 0.0f; v.y = 0.0f; v.z = 0.0f; };
		NSVector3df( nsfloat x, nsfloat y, nsfloat z )
				{ v.x = x; v.y = y; v.z = z; };
		NSVector3df( D3DXVECTOR3 v2 )
				{ this->v = v2; };
		~NSVector3df()
				{};


		inline NSVector3df	CrossProduct( const NSVector3df &v2 ) const
								{
									NSVector3df vec;
									D3DXVec3Cross( &vec.v, &v, &v2.v );
									return vec;
								};
//		inline nsfloat		Dot( const NSVector3df &v2 ) const
//								{ float value = D3DXVec3Dot( &v, &v2.v ); return value; };

		inline nsfloat			Dot( const NSVector3df &v2 ) const
								{ return ( ( v.x * v2.v.x ) + (v.y * v2.v.y ) + (v.z * v2.v.z ) ); };

		inline void			Normalise()
								{ D3DXVec3Normalize( &v, &v ); };

		inline void			Invert()
								{ v.x *= -1.0f; v.y *= -1.0f; v.z *= -1.0f; };

		inline void			Scale( nsfloat scale )
								{ D3DXVec3Scale( &v, &v, scale ); };
		inline void			SetLength( nsfloat length )
								{ this->Normalise(); this->Scale( length ); };

		inline nsfloat		GetLength() const
								{ nsfloat val = D3DXVec3Length( &v ); return val; };
		inline nsfloat		GetLengthSqr() const
								{ nsfloat val = D3DXVec3LengthSq( &v ); return val; };
		inline bool			IsZeroLength() const
								{ return (this->GetLengthSqr() == 0.0f); };
		inline void			SetZero()
								{ this->Scale( 0.0f ); };

		inline void			SetTranslation( const NSVector3df &v2 )
								{ v.x = v2.v.x; v.y = v2.v.y; v.z = v2.v.z; };
		inline void			SetTranslation( nsfloat x, nsfloat y, nsfloat z )
								{ v.x = x; v.y = y; v.z = z; };

		inline void			Add( const NSVector3df &v2 )
								{ D3DXVec3Add( &v, &v, &v2.v ); };
		inline void			Subtract( const NSVector3df &v2 )
								{ D3DXVec3Subtract( &v, &v, &v2.v ); };

//		inline void			Multiply( const NSMatrix4 &m )
//								{ D3DXVec3TransformCoord( &v, &v, &m.m ); };



//		inline NSVector3df	operator * (const NSMatrix4 &m2) const
//			{	
//				NSVector3df nv;
//				D3DXVec3TransformCoord( &nv.v, &v, &m2.m );
//				return nv;
//			};

//		inline NSVector3df *	operator *= (const NSMatrix4 &m2)
//			{
//				D3DXVec3TransformCoord( &v, &v, &m2.m );
//				return this;
//			};

		//inline NSVector3df *	operator = (const NSVertex3df &v2)
		//	{ 
		//		this->v.x = v2.x;
		//		this->v.y = v2.y;
		//		this->v.z = v2.z;
		//		return this;
		//	};


		inline NSVector3df *	operator = (const NSVector3df &v2)
			{ 
				// FLA:DKFL:DKA M$ VECTORS DONT HAVE AN = OPERATOR!!
				// THEY HAVE EVERYTHING EXCEPT FOR AN = OPERATOR!!! WWHHTAWIYHAEKLE WHHYY?!!!!!
				this->v.x = v2.v.x;
				this->v.y = v2.v.y;
				this->v.z = v2.v.z;
				return this;
			};


		inline NSVector3df		operator + (const NSVector3df &v2) const
			{	
				NSVector3df vec;
				vec.v.x = this->v.x + v2.v.x;
				vec.v.y = this->v.y + v2.v.y;
				vec.v.z = this->v.z + v2.v.z;
				return vec;
			};

		inline NSVector3df *	operator += (const NSVector3df &v2)
			{
				this->v.x = this->v.x + v2.v.x;
				this->v.y = this->v.y + v2.v.y;
				this->v.z = this->v.z + v2.v.z;
				return this;
			};

		inline NSVector3df		operator - (const NSVector3df &v2) const
			{
				NSVector3df vec;
				vec.v.x = this->v.x - v2.v.x;
				vec.v.y = this->v.y - v2.v.y;
				vec.v.z = this->v.z - v2.v.z;
				return vec;
			}

		inline NSVector3df *	operator -= (const NSVector3df &v2)
			{
				this->v.x = this->v.x - v2.v.x;
				this->v.y = this->v.y - v2.v.y;
				this->v.z = this->v.z - v2.v.z;
				return this;
			};

		inline bool				operator == (const NSVector3df &v2) const
			{
				if (this->v == v2.v)
					return true;
				return false;
			};

		inline bool				operator != (const NSVector3df &v2) const
			{
				if (this->v != v2.v)
					return true;
				return false;
			};


		inline NSVector3df operator * ( nsfloat s ) const
			{	
				NSVector3df vec;
				vec.v.x = this->v.x * s;
				vec.v.y = this->v.y * s;
				vec.v.z = this->v.z * s;
				return vec;
			};

		inline NSVector3df * operator *= ( nsfloat s )
			{
				this->v.x = this->v.x * s;
				this->v.y = this->v.y * s;
				this->v.z = this->v.z * s;
				return this;
			};

		inline NSVector3df operator / ( nsfloat s ) const
			{
				NSVector3df vec;
				vec.v.x = this->v.x / s;
				vec.v.y = this->v.y / s;
				vec.v.z = this->v.z / s;
				return vec;
			};

		inline NSVector3df * operator /= ( nsfloat s )
			{
				this->v.x = this->v.x / s;
				this->v.y = this->v.y / s;
				this->v.z = this->v.z / s;
				return this;
			};



};


/*
class NSVector3df
{
	public:
	// Public variables
		nsfloat		x, y, z;

	// Standard constructors

		inline NSVector3df()									// Default constructor. Creates a vector with length of 0
		{ x = y = z = 0; };

		inline NSVector3df(nsfloat vx, nsfloat vy, nsfloat vz)	// Creates a simple vector from (0,0,0) to (x2,y2,z2)
		{ x = vx; y = vy; z = vz; };

		inline NSVector3df(const NSVector3df &v)				// Copy constructor
		{ x = v.x; y = v.y; z = v.z; };

	// Vertex3d Constructors
		inline NSVector3df(const NSVertex3df &v)				// Creates a vector from 0 to v
		{ x = v.x; y = v.y; z = v.z; };

		inline NSVector3df(const NSVertex3df &v1, const NSVertex3df &v2)	// Creates a vector from v1 to v2
		{ x = v1.x - v2.x; y = v1.y - v2.y; z = v1.z - v2.z; };

		inline NSVector3df(const NSVector3df &a, const NSVertex3df &b, const NSVertex3df &c)	// Creates a vector based on the cross product (normal) of 3 vertices
		{
			x = (a.y - b.y) * c.z - c.y * (a.z - b.z);
			y = (a.z - b.z) * c.x - (a.x - b.x) * c.z;
			z = (a.x - b.x) * c.y - c.x * (a.y - b.y);
		};

		~NSVector3df()									// Destructor
		{
		};




	// Methods
		inline NSVector3df *	Invert() { x *= -1.0f; y *= -1.0f; z *= -1.0f; return this; };

		inline nsfloat			DotProduct(const NSVector3df &v)
			{ return ( ( x * v.x ) + (y * v.y ) + (z * v.z ) ); };
		inline NSVector3df		CrossProduct(const NSVector3df &v)
			{ return NSVector3df(	y * v.z - z * v.y,
									z * v.x - x * v.z,
									x * v.y - y * v.x ); };

		inline void				Scale(nsfloat scale) { x *= scale; y *= scale; z *= scale; };

		inline nsfloat			GetLength() { return sqrt( (x * x) + (y * y) + (z * z) ); };
		inline nsfloat			GetLengthSqr() { return ( (x * x) + (y * y) + (z * z) ); };		// faster than getLength()

		inline bool				IsZeroLength() { return (this->GetLengthSqr() == 0.0f); };

		inline NSVector3df *	Normalise() { if ( this->IsZeroLength() ) return this; nsfloat inverse = 1.0f / this->GetLength(); x *= inverse; y *= inverse; z *= inverse; return this; };

		inline void				SetLength(nsfloat length) { this->Normalise(); this->Scale(length); };
		inline void				SetZero() { x = y = z = 0; };
		inline void				SetTranslation( nsfloat tx, nsfloat ty, nsfloat tz) { this->x = tx; this->y = ty; this->z = tz; };

//////////////////////
// Operators
		//////////////////////
		// Vector operators
		inline NSVector3df *	operator=(const NSVector3df &v) { x = v.x; y = v.y; z = v.z; return this; };
		inline NSVector3df		operator+(const NSVector3df &v) const { return NSVector3df(x + v.x, y + v.y, z + v.z); };
		inline NSVector3df *	operator+=(const NSVector3df &v) { x += v.x; y += v.y; z += v.z; return this; };
		inline NSVector3df		operator-(const NSVector3df &v) const { return NSVector3df(x - v.x, y - v.y, z - v.z); };
		inline NSVector3df *	operator-=(const NSVector3df &v) { x -= v.x; y -= v.y; z -= v.z; return this; };
		
		inline NSVector3df		operator*(const NSVector3df &v) const { return NSVector3df(x * v.x, y * v.y, z * v.z ); };
		inline NSVector3df *	operator*=(const NSVector3df &v) { x *= v.x; y *= v.y; z *= v.z; return this; };
		inline NSVector3df		operator/(const NSVector3df &v) const { return NSVector3df(x / v.x, y / v.y, z / v.z ); };
		inline NSVector3df *	operator/=(const NSVector3df &v) { x /= v.x; y /= v.y; z /= v.z; return this; };

		inline bool				operator==(const NSVector3df &v) const { return ( x == v.x && y == v.y && z == v.z ); };
		inline bool				operator!=(const NSVector3df &v) const { return ( x != v.x && y != v.y && z != v.z ); };

		inline bool				operator>(const NSVector3df &v) const { return ( x > v.x && y > v.y && z > v.z ); };
		inline bool				operator<(const NSVector3df &v) const { return ( x < v.x && y < v.y && z < v.z ); };
		inline bool				operator>=(const NSVector3df &v) const { return ( x >= v.x && y >= v.y && z >= v.z ); };
		inline bool				operator<=(const NSVector3df &v) const { return ( x <= v.x && y <= v.y && z <= v.z ); };


	//////////////////////
	// Vertex operators
		inline NSVector3df *	operator=(const NSVertex3df &v) { x = v.x; y = v.y; z = v.z; return this; };

		inline NSVector3df		operator+(const NSVertex3df &v) const { return NSVector3df(x + v.x, y + v.y, z + v.z); };
		inline NSVector3df *	operator+=(const NSVertex3df &v) { x += v.x; y += v.y; z += v.z; return this; };
		inline NSVector3df		operator-(const NSVertex3df &v) const { return NSVector3df(x - v.x, y - v.y, z - v.z); };
		inline NSVector3df *	operator-=(const NSVertex3df &v) { x -= v.x; y -= v.y; z -= v.z; return this; };
		
		inline NSVector3df		operator*(const NSVertex3df &v) const { return NSVector3df(x * v.x, y * v.y, z * v.z ); };
		inline NSVector3df *	operator*=(const NSVertex3df &v) { x *= v.x; y *= v.y; z *= v.z; return this; };
		inline NSVector3df		operator/(const NSVertex3df &v) const { return NSVector3df(x / v.x, y / v.y, z / v.z ); };
		inline NSVector3df *	operator/=(const NSVertex3df &v) { x /= v.x; y /= v.y; z /= v.z; return this; };

		inline bool				operator==(const NSVertex3df &v) const { return ( x == v.x && y == v.y && z == v.z ); };
		inline bool				operator!=(const NSVertex3df &v) const { return ( x != v.x && y != v.y && z != v.z ); };

		inline bool				operator>(const NSVertex3df &v) const { return ( x > v.x && y > v.y && z > v.z ); };
		inline bool				operator<(const NSVertex3df &v) const { return ( x < v.x && y < v.y && z < v.z ); };
		inline bool				operator>=(const NSVertex3df &v) const { return ( x >= v.x && y >= v.y && z >= v.z ); };
		inline bool				operator<=(const NSVertex3df &v) const { return ( x <= v.x && y <= v.y && z <= v.z ); };



		// float operators
//		inline NSVector3df		operator+(const nsfloat num) const { return NSVector3df(x + num, y + num, z + num); };
//		inline NSVector3df *	operator+=(const nsfloat num) { x += num; y += num; z += num; return this; };
//		inline NSVector3df		operator-(const nsfloat num) const { return NSVector3df(x - num, y - num, z - num); };
//		inline NSVector3df *	operator-=(const nsfloat num) { x -= num; y -= num; z -= num; return this; };

		inline NSVector3df		operator*(const nsfloat num) const { return NSVector3df(x * num, y * num, z * num); };
		inline NSVector3df *	operator*=(const nsfloat num) { x *= num; y *= num; z *= num; return this; };
		inline NSVector3df		operator/(const nsfloat num) const { return NSVector3df(x / num, y / num, z / num); };
		inline NSVector3df *	operator/=(const nsfloat num) { x /= num; y /= num; z /= num; return this; };

		inline bool				operator>(const nsfloat num) const { return ( num > x && num > y && num > z ); };
		inline bool				operator<(const nsfloat num) const { return ( num < x && num < y && num < z ); };
		inline bool				operator>=(const nsfloat num) const { return ( num >= x && num >= y && num >= z ); };
		inline bool				operator<=(const nsfloat num) const { return ( num <= x && num <= y && num <= z ); };


	protected:

};
*/

}; // namespace neurosis

#endif // #ifndef NSVECTOR3D_H
