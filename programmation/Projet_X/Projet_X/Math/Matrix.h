
#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
#include <iostream>
#include <scene.h>
#include <vector>

class Vector;
class Vector4f;

class Matrix
{
	public:
	//Constructeur
    Matrix();
    Matrix(float* ip_values);
    Matrix(Matrix const& ik_matrix);

	Matrix(const aiMatrix4x4& ik_assimpMatrix);
	Matrix(const aiMatrix3x3& ik_assimpMatrix);
	//Destructeur
    ~Matrix();

	//Operateur
	Matrix operator*(Matrix const& ik_matrix)const;
	Matrix operator*=(Matrix const& ik_matrix);
	Matrix& operator=(Matrix const& ik_matrix);
	Vector operator*(Vector const& ik_vector);
	Vector4f operator*(Vector4f const& ik_vector);

	//Accesseur
    float* const getValues() const;
	void getValues(std::vector<float>& i_value) const;

	//Tranformation
	void translate(float i_x, float i_y, float i_z);
	inline void scale(float i_factor){scale(i_factor, i_factor, i_factor);}
	void scale(float i_x, float i_y, float i_z);
	void rotate(float i_angle, float i_x, float i_y, float i_z); 

	//Fonction opengl
	void loadIdentity();
	void loadZero();
	void perspective(float i_angle, float i_ratio, float i_near, float i_far);
	void lookAt(float i_eyeX, float i_eyeY, float i_eyeZ, float i_centerX, float i_centerY, float i_centerZ, float i_upX, float i_upY, float i_upZ);

	Matrix transpose();

	//Fonction de la pile
    bool push();
    bool pop();
    void clear();

	private:
	//Valeurs de la matrice
	//0, 1, 2, 3
	//4, 5, 6, 7
	//8, 9,10,11
	//12,13,14,15
    float ma_values[16];

	//Liste chainée
    Matrix* mp_previousSave;
};

#endif //MATRIX_H
