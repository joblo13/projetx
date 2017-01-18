#include "Quaternion.h"

Quaternion::Quaternion() 
{}
Quaternion::Quaternion(float real, float x, float y, float z)
: m_real(real), m_imaginary(x,y,z) 
{}
Quaternion::Quaternion(float real, const Vector &i)
: m_real(real), m_imaginary(i) 
{}

//! from 3 euler angles
Quaternion::Quaternion(float theta_z, float theta_y, float theta_x)
{
	float cos_z_2 = cosf(0.5*theta_z);
	float cos_y_2 = cosf(0.5*theta_y);
	float cos_x_2 = cosf(0.5*theta_x);

	float sin_z_2 = sinf(0.5*theta_z);
	float sin_y_2 = sinf(0.5*theta_y);
	float sin_x_2 = sinf(0.5*theta_x);

	// and now compute quaternion
	m_real   = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
	m_imaginary.x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
	m_imaginary.y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
	m_imaginary.z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;

}
	
//! from 3 euler angles 
Quaternion::Quaternion(const Vector &angles)
{	
	float cos_z_2 = cosf(0.5*angles.z);
	float cos_y_2 = cosf(0.5*angles.y);
	float cos_x_2 = cosf(0.5*angles.x);

	float sin_z_2 = sinf(0.5*angles.z);
	float sin_y_2 = sinf(0.5*angles.y);
	float sin_x_2 = sinf(0.5*angles.x);

	// and now compute quaternion
	m_real   = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
	m_imaginary.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
	m_imaginary.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
	m_imaginary.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;		
} 
		
//! basic operations
Quaternion& Quaternion::operator=(const Quaternion &q)		
{ 
	m_real = q.m_real; 
	m_imaginary = q.m_imaginary; 
	return *this; 
}

const Quaternion Quaternion::operator+(const Quaternion &q) const	
{ 
	return Quaternion(m_real+q.m_real, m_imaginary+q.m_imaginary); 
}

const Quaternion Quaternion::operator-(const Quaternion &q) const	
{ 
	return Quaternion(m_real-q.m_real, m_imaginary-q.m_imaginary); 
}

const Quaternion Quaternion::operator *(const Quaternion &q) const	
{	
	return Quaternion(m_real * q.m_real - m_imaginary * q.m_imaginary,
				m_imaginary.y * q.m_imaginary.z - m_imaginary.z * q.m_imaginary.y + m_real * q.m_imaginary.x + m_imaginary.x * q.m_real,
				m_imaginary.z * q.m_imaginary.x - m_imaginary.x * q.m_imaginary.z + m_real * q.m_imaginary.y + m_imaginary.y * q.m_real,
				m_imaginary.x * q.m_imaginary.y - m_imaginary.y * q.m_imaginary.x + m_real * q.m_imaginary.z + m_imaginary.z * q.m_real);
}

const Quaternion Quaternion::operator/(const Quaternion &q) const	
{
	Quaternion p(q); 
	p.invert(); 
	return *this * p;
}

const Quaternion Quaternion::operator*(float scale) const
{ 
	return Quaternion(m_real*scale, m_imaginary*scale); 
}

const Quaternion Quaternion::operator/(float scale) const
{ 
	return Quaternion(m_real/scale, m_imaginary/scale); 
}

const Quaternion Quaternion::operator-() const
{ 
	return Quaternion(-m_real, -m_imaginary); 
}
	
const Quaternion& Quaternion::operator+=(const Quaternion &q)		
{ 
	m_imaginary += q.m_imaginary; 
	m_real += q.m_real; 
	return *this; 
}

const Quaternion& Quaternion::operator-=(const Quaternion &q)		
{ 
	m_imaginary -= q.m_imaginary; 
	m_real -= q.m_real; 
	return *this; 
}

const Quaternion& Quaternion::operator*=(const Quaternion &q)		
{			
	float x = m_imaginary.x, 
		y = m_imaginary.y, 
		z = m_imaginary.z, 
		sn = m_real * q.m_real - m_imaginary * q.m_imaginary;

	m_imaginary.x= y * q.m_imaginary.z - z * q.m_imaginary.y + m_real * q.m_imaginary.x + x * q.m_real;
	m_imaginary.y= z * q.m_imaginary.x - x * q.m_imaginary.z + m_real * q.m_imaginary.y + y * q.m_real;
	m_imaginary.z= x * q.m_imaginary.y - y * q.m_imaginary.x + m_real * q.m_imaginary.z + z * q.m_real;
	m_real= sn;
	return *this;
}
	
const Quaternion& Quaternion::operator*=(float scale)			
{
	m_imaginary *= scale; 
	m_real *= scale; 
	return *this; 
}

const Quaternion& Quaternion::operator/=(float scale)			
{
	m_imaginary /= scale; 
	m_real /= scale; 
	return *this; 
}
	

//! gets the length of this quaternion
float Quaternion::length()const
{ 
	return (float)sqrt(m_real * m_real + m_imaginary * m_imaginary); 
}

//! gets the squared length of this quaternion
float Quaternion::length_squared()const
{ 
	return (float)(m_real * m_real + m_imaginary * m_imaginary); 
}

//! normalizes this quaternion
void Quaternion::normalize()
{ 
	*this/=length(); 
}

//! returns the normalized version of this quaternion
Quaternion Quaternion::normalized() const
{ 
	return  *this/length(); 
}

//! computes the conjugate of this quaternion
void Quaternion::conjugate()
{ 
	m_imaginary = -m_imaginary; 
}

//! inverts this quaternion
void Quaternion::invert()
{ 
	conjugate(); 
	*this /= length_squared(); 
}
	
//! returns the logarithm of a quaternion = v*a where q = [cos(a),v*sin(a)]
Quaternion Quaternion::log()const
{
	float a = (float)acos(m_real);
	float sina = (float)sin(a);
	Quaternion ret;

	ret.m_real = 0;
	if (sina > 0)
	{
		ret.m_imaginary.x = a*m_imaginary.x/sina;
		ret.m_imaginary.y = a*m_imaginary.y/sina;
		ret.m_imaginary.z = a*m_imaginary.z/sina;
	} 
	else 
	{
		ret.m_imaginary.x= ret.m_imaginary.y= ret.m_imaginary.z= 0;
	}
	return ret;
}

//! returns e^quaternion = exp(v*a) = [cos(a),vsin(a)]
Quaternion Quaternion::exp()const
{
	float a = (float)m_imaginary.length();
	float sina = (float)sin(a);
	float cosa = (float)cos(a);
	Quaternion ret;

	ret.m_real = cosa;
	if (a > 0)
	{
		ret.m_imaginary.x = sina * m_imaginary.x / a;
		ret.m_imaginary.y = sina * m_imaginary.y / a;
		ret.m_imaginary.z = sina * m_imaginary.z / a;
	} 
	else 
	{
		ret.m_imaginary.x = ret.m_imaginary.y = ret.m_imaginary.z = 0;
	}
	return ret;
}

////! casting to a 4x4 isomorphic matrix for right multiplication with vector
//operator Quaternion::Matrix()const
//{			
//	return Matrix(m_s,  -m_v.x, -m_v.y,-m_v.z,
//			m_v.m_x,  m_s,  -m_v.z, m_v.y,
//			m_v.m_y, m_v.m_z,    m_s,-m_v.x,
//			m_v.m_z,-m_v.m_y,  m_v.x,   m_s);
//}
//	
////! casting to 3x3 rotation matrix
//operator Quaternion::Matrix3x3()const
//{
//	Assert(length() > 0.9999 && length() < 1.0001, "quaternion is not normalized");		
//	return Mtrix3x3(1-2*(v.y*v.y+v.z*v.z), 2*(v.x*v.y-s*v.z),   2*(v.x*v.z+s*v.y),   
//			2*(v.x*v.y+s*v.z),   1-2*(v.x*v.x+v.z*v.z), 2*(v.y*v.z-s*v.x),   
//			2*(v.x*v.z-s*v.y),   2*(v.y*v.z+s*v.x),   1-2*(v.x*v.x+v.y*v.y));
//}

//! computes the dot product of 2 quaternions
inline float Quaternion::dot(const Quaternion &q1, const Quaternion &q2) 
{ 
	return q1.m_imaginary * q2.m_imaginary + q1.m_real * q2.m_real; 
}

//! linear quaternion interpolation
Quaternion Quaternion::lerp(const Quaternion &q1, const Quaternion &q2, float t) 
{ 
	return (q1 * (1-t) + q2 * t).normalized(); 
}

//! spherical linear interpolation
Quaternion Quaternion::slerp(const Quaternion &q1, const Quaternion &q2, float t) 
{
	Quaternion q3;
	float dot = Quaternion::dot(q1, q2);

	/*	dot = cos(theta)
		if (dot < 0), q1 and q2 are more than 90 degrees apart,
		so we can invert one to reduce spinning	*/
	if (dot < 0)
	{
		dot = -dot;
		q3 = -q2;
	} 
	else 
		q3 = q2;
		
	if (dot < 0.95f)
	{
		float angle = acosf(dot);
		return (q1 * sinf(angle * (1 - t)) + q3 * sinf(angle * t))/sinf(angle);
	} 
	else // if the angle is small, use linear interpolation								
		return lerp(q1, q3, t);		
}

//! This version of slerp, used by squad, does not check for theta > 90.
Quaternion Quaternion::slerpNoInvert(const Quaternion &q1, const Quaternion &q2, float t) 
{
	float dot = Quaternion::dot(q1, q2);

	if (dot > -0.95f && dot < 0.95f)
	{
		float angle = acosf(dot);			
		return (q1 * sinf(angle*(1-t)) + q2 * sinf(angle * t))/sinf(angle);
	} 
	else  // if the angle is small, use linear interpolation								
		return lerp(q1, q2, t);			
}

//! spherical cubic interpolation
Quaternion Quaternion::squad(const Quaternion &q1, const Quaternion &q2, const Quaternion &a, const Quaternion &b, float t)
{
	Quaternion c = slerpNoInvert(q1,q2,t),
			    d = slerpNoInvert(a,b,t);		
	return slerpNoInvert(c, d, 2 * t * (1 - t));
}

//! Shoemake-Bezier interpolation using De Castlejau algorithm
Quaternion Quaternion::bezier(const Quaternion &q1, const Quaternion &q2, const Quaternion &a, const Quaternion &b, float t)
{
	// level 1
	Quaternion q11= slerpNoInvert(q1, a, t),
			q12= slerpNoInvert(a, b, t),
			q13= slerpNoInvert(b, q2, t);		
	// level 2 and 3
	return slerpNoInvert(slerpNoInvert(q11, q12, t), slerpNoInvert(q12, q13, t), t);
}

//! Given 3 quaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
Quaternion Quaternion::spline(const Quaternion &qnm1,const Quaternion &qn,const Quaternion &qnp1)
{
	Quaternion qni(qn.m_real, -qn.m_imaginary);	
	return qn * (( (qni * qnm1).log()+(qni * qnp1).log())/-4).exp();
}

//! converts from a normalized axis - angle pair rotation to a quaternion
inline Quaternion Quaternion::from_axis_angle(const Vector &axis, float angle)
{ 
	return Quaternion(cosf(angle/2), axis * sinf(angle/2));
}

//! returns the axis and angle of this unit quaternion
void Quaternion::to_axis_angle(Vector &axis, float &angle)const
{
	angle = acosf(m_real);

	// pre-compute to save time
	float sinf_theta_inv = 1.0/sinf(angle);

	// now the vector
	axis.x = m_imaginary.x*sinf_theta_inv;
	axis.y = m_imaginary.y*sinf_theta_inv;
	axis.z = m_imaginary.z*sinf_theta_inv;

	// multiply by 2
	angle *= 2;
}

//! rotates v by this quaternion (quaternion must be unit)
Vector Quaternion::rotate(const Vector &v)
{   
	Quaternion V(0, v);
	Quaternion conjugate(*this);
	conjugate.conjugate();
	return (*this * V * conjugate).m_imaginary;
}

//! returns the euler angles from a rotation quaternion
Vector Quaternion::euler_angles(bool homogenous)const
{
	float sqw = m_real * m_real;    
	float sqx = m_imaginary.x * m_imaginary.x;    
	float sqy = m_imaginary.y * m_imaginary.y;    
	float sqz = m_imaginary.z * m_imaginary.z;    

	Vector euler;
	if (homogenous) 
	{
		euler.x = atan2f(2.f * (m_imaginary.x*m_imaginary.y + m_imaginary.z*m_real), sqx - sqy - sqz + sqw);    		
		euler.y = asinf(-2.f * (m_imaginary.x*m_imaginary.z - m_imaginary.y*m_real));
		euler.z = atan2f(2.f * (m_imaginary.y*m_imaginary.z + m_imaginary.x*m_real), -sqx - sqy + sqz + sqw);    
	} 
	else 
	{
		euler.x = atan2f(2.f * (m_imaginary.z*m_imaginary.y + m_imaginary.x*m_real), 1 - 2 * (sqx + sqy));
		euler.y = asinf(-2.f * (m_imaginary.x*m_imaginary.z - m_imaginary.y*m_real));
		euler.z = atan2f(2.f * (m_imaginary.x*m_imaginary.y + m_imaginary.z*m_real), 1 - 2 * (sqy + sqz));
	}
	return euler;
}