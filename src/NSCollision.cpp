#include "NSCollision.h"

namespace neurosis {


nsfloat	IntersectSpherePlane( const NSSphere &s, const NSPlane &p )
{
	// get our planes normal and extrude it to the spheres radius
	NSVector3df planenorm = p.m_pNormal;
	planenorm.SetLength( s.m_fRadius );

	// translate the sphere and normal to be relative to the plane
	NSVector3df spherevec = s.m_pPosition - p.m_pPosition;

	// we have to do this since the Dots are returning stupid feking values!
	spherevec = spherevec - planenorm;

	planenorm.Normalise();

	// dot product the extruded planes position with the spheres position
	// if dist > 1.0, no collision
	// if dist == 1.0, spheres is touching
	// if dist < 1.0, sphere is behind
	float value = planenorm.Dot( spherevec );
	return value;
}

nsfloat	IntersectPointPlane( const NSVector3df &v, const NSPlane &p )
{
	// get our planes normal and extrude it to the spheres radius
	NSVector3df planenorm = p.m_pNormal;

	// translate the sphere and normal to be relative to the plane
	NSVector3df pointvec = v - p.m_pPosition;

	planenorm.Normalise();

	// dot product the extruded planes position with the spheres position
	// if dist > 0.0, no collision
	// if dist == 0.0, point is touching
	// if dist < 0.0, point is behind
	float value = planenorm.Dot( pointvec );
	return value;
}



nsfloat IntersectSphereSphere( const NSSphere &s1, const NSSphere &s2 )
{
	NSVector3df vec = s1.m_pPosition - s2.m_pPosition;
	
	nsfloat dist = vec.GetLength();
	dist = dist - s1.m_fRadius - s2.m_fRadius;

	// if dist > 0, no collision
	// if dist == 0, spheres are touching
	// if dist < 0, spheres are intersecting
	return dist;
}


nsfloat IntersectSphereMatrixSphere( const NSSphere &s1, const NSSphere &s2, const NSMatrix4 &s2mat )
{
	NSVector3df vec = s1.m_pPosition - s2.m_pPosition;
	
	nsfloat dist = vec.GetLength();

	// we now use this vector to get the radius of the deformed sphere
	vec.SetLength( s2.m_fRadius );
	vec = s2mat * vec;

	dist = dist - s1.m_fRadius - vec.GetLength();

	// if dist > 0, no collision
	// if dist == 0, spheres are touching
	// if dist < 0, spheres are intersecting
	return dist;
}

nsfloat IntersectMatrixSphereMatrixSphere(	const NSSphere &s1, const NSMatrix4 &s1mat,
											const NSSphere &s2, const NSMatrix4 &s2mat )
{
	NSVector3df vec = s1.m_pPosition - s2.m_pPosition;
	
	nsfloat dist = vec.GetLength();

	// we now use this vector to get the radius of the deformed sphere
	NSVector3df s2vec = vec;

	vec.SetLength( s2.m_fRadius );
	vec = s2mat * vec;

	s2vec.SetLength( s1.m_fRadius );
	s2vec = s1mat * s2vec;


	dist = dist - s2vec.GetLength() - vec.GetLength();

	// if dist > 0, no collision
	// if dist == 0, spheres are touching
	// if dist < 0, spheres are intersecting
	return dist;
}




void ReflectVector( const NSVector3df &normal, NSVector3df &vector )
{
	// Get the normal
	NSVector3df diff = normal;
	diff.Normalise();

	// Get the dot product between the normal and the vector
	nsfloat scale = diff.Dot( vector );

	// Scale the normal by the dot product * 2 to reflect the velocity
	diff.Scale(  abs( (scale * 2) )  );

	// Add the normal to the velocity to reflect it
	vector += diff;
	return;
}


}; // namespace neurosis
