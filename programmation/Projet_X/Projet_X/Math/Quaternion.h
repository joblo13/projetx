/****************************************
 * Quaternion class
 * By Will Perone
 * Original: 12-09-2003  
 * Revised:  27-09-2003
 *           22-11-2003
 *           10-12-2003
 *           15-01-2004
 *           16-04-2004
 *           29-07-2011   added corrections from website,
 *           22-12-2011   added correction to *= operator, thanks Steve Rogers
 * 
 * Notes:  
 * if |q|=1 then q is a unit quaternion
 * if q=(0,v) then q is a pure quaternion 
 * if |q|=1 then q conjugate = q inverse
 * if |q|=1 then q= [cos(angle), u*sin(angle)] where u is a unit vector 
 * q and -q represent the same rotation 
 * q*q.conjugate = (q.length_squared, 0) 
 * ln(cos(theta),sin(theta)*v)= ln(e^(theta*v))= (0, theta*v)
 ****************************************/

#ifndef QUATERNION_H
#define QUATERNION_H

#include "../Math/Matrix.h" 
#include "Vector.h" 
#include "assert.h" 


struct Quaternion
{
	union 
	{
		struct 
		{
			float    m_real; //!< the real component
			Vector m_imaginary; //!< the imaginary components
		};
		struct 
		{
			float m_element[4]; 
		}; //! the raw elements of the quaternion
	};


	//! constructors
	Quaternion();
	Quaternion(float real, float x, float y, float z);
	Quaternion(float real, const Vector &i);

	//! from 3 euler angles
	Quaternion(float theta_z, float theta_y, float theta_x);
	
	//! from 3 euler angles 
	Quaternion(const Vector &angles);
		
	//! basic operations
	Quaternion &operator =(const Quaternion &q)		;

	const Quaternion operator +(const Quaternion &q) const	;

	const Quaternion operator -(const Quaternion &q) const	;

	const Quaternion operator *(const Quaternion &q) const	;

	const Quaternion operator /(const Quaternion &q) const	;

	const Quaternion operator *(float scale) const;

	const Quaternion operator /(float scale) const;

	const Quaternion operator -() const;
	
	const Quaternion &operator +=(const Quaternion &q);

	const Quaternion &operator -=(const Quaternion &q);

	const Quaternion &operator *=(const Quaternion &q);
	
	const Quaternion &operator *= (float scale);

	const Quaternion &operator /= (float scale);
	

	//! gets the length of this quaternion
	float length() const;

	//! gets the squared length of this quaternion
	float length_squared() const;

	//! normalizes this quaternion
	void normalize();

	//! returns the normalized version of this quaternion
	Quaternion normalized() const;

	//! computes the conjugate of this quaternion
	void conjugate();

	//! inverts this quaternion
	void invert();
	
	//! returns the logarithm of a quaternion = v*a where q = [cos(a),v*sin(a)]
	Quaternion log() const;

	//! returns e^quaternion = exp(v*a) = [cos(a),vsin(a)]
	Quaternion exp() const;

	////! casting to a 4x4 isomorphic matrix for right multiplication with vector
	//operator Matrix4() const;
	//
	////! casting to 3x3 rotation matrix
	//operator Matrix3x3() const;

	//! computes the dot product of 2 quaternions
	static inline float dot(const Quaternion &q1, const Quaternion &q2) ;

	//! linear quaternion interpolation
	static Quaternion lerp(const Quaternion &q1, const Quaternion &q2, float t) ;

	//! spherical linear interpolation
	static Quaternion slerp(const Quaternion &q1, const Quaternion &q2, float t) ;

	//! This version of slerp, used by squad, does not check for theta > 90.
	static Quaternion slerpNoInvert(const Quaternion &q1, const Quaternion &q2, float t) ;

	//! spherical cubic interpolation
	static Quaternion squad(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t);

	//! Shoemake-Bezier interpolation using De Castlejau algorithm
	static Quaternion bezier(const Quaternion &q1,const Quaternion &q2,const Quaternion &a,const Quaternion &b,float t);

	//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
	static Quaternion spline(const Quaternion &qnm1,const Quaternion &qn,const Quaternion &qnp1);

	//! converts from a normalized axis - angle pair rotation to a quaternion
	static inline Quaternion from_axis_angle(const Vector &axis, float angle);

	//! returns the axis and angle of this unit quaternion
	void to_axis_angle(Vector &axis, float &angle) const;

	//! rotates v by this quaternion (quaternion must be unit)
	Vector rotate(const Vector &v);

	//! returns the euler angles from a rotation quaternion
	Vector euler_angles(bool homogenous=true) const;
};

#endif //QUATERNION_H