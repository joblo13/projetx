
#ifndef MATRIX3X3_H
#define MATRIX3X3_H

#include <cmath>
#include <iostream>
#include <scene.h>

class Vector;
class Matrix;

class Matrix3x3
{
	public:
	//Constructeur
    Matrix3x3();
	Matrix3x3(const Matrix& i_matrix);
    Matrix3x3(float* ip_values);
    Matrix3x3(Matrix3x3 const& ik_matrix3x3);

	Matrix3x3(const aiMatrix4x4& ik_assimpMatrix);
	Matrix3x3(const aiMatrix3x3& ik_assimpMatrix);
	//Destructeur
    ~Matrix3x3();

	//Operateur
	Matrix3x3 operator*(Matrix3x3 const& ik_matrix3x3);
	Matrix3x3& operator=(Matrix3x3 const& ik_matrix3x3);
	Vector operator*(Vector const& ik_vector);

	//Accesseur
    float* const getValues() const;

	//Fonction opengl
	void invert();
	void transpose();
	void loadIdentity();
	void loadZero();
	void perspective(float i_angle, float i_ratio, float i_near, float i_far);
	void lookAt(float i_eyeX, float i_eyeY, float i_eyeZ, float i_centerX, float i_centerY, float i_centerZ, float i_upX, float i_upY, float i_upZ);

	//Fonction de la pile
    bool push();
    bool pop();
    void clear();

	private:
	//Valeurs de la matrice
	//0, 1, 2
	//3, 4, 5
	//6, 7, 8
    float ma_values[9];

	//Liste chainée
    Matrix3x3* mp_previousSave;
};

#endif //MATRIX3X3_H
