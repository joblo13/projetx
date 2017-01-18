#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <vector>
#include <SDL/SDL.h>
#include "../Shaders/Shader.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define INVALID_MATERIAL 0xFFFFFFFF
#define NUM_BONES_PER_VEREX 4
#define ZERO_MEM(a) memset(a, 0, sizeof(a))

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    BONE_VB,
	COLOR_VB,
	MODELVIEW_VB,
	HITPOINT_BAR_VB,
	ARMOR_BAR_VB,
    NUM_VBs       
};

enum{CULLFACE, UP, NB_CUSTOMISATION};
enum{UP_X, UP_Y, UP_Z, UP_XN, UP_YN, UP_ZN};

class Matrix;
class Camera;

class Drawable
{
public:
	Drawable(ShaderType i_shaderType);
	~Drawable();
	virtual void animate(Uint32 i_timestep) = 0;
	virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const;

	const std::vector<int>* getTexturesIndex(){return &m_texturesIndex;}
	const std::vector<float>* getShininess(){return &m_shininess;}
	const std::vector<float>* getShininessStrength(){return &m_shininessStrength;}
	void setID(int i_ID){m_ID = i_ID;}
	int getID(){return m_ID;}
	GLuint getVAO(){return m_VAOID;}

	void addTexture(const std::string& i_variableName, const std::string& i_path);

protected:
	virtual void preTreatementDraw()const{};
	virtual void postTreatementDraw()const{};
	virtual void transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const{};
	std::vector<int> m_texturesIndex;
	std::vector<float> m_shininess;
	std::vector<float> m_shininessStrength;

	//Information général
	ShaderType m_shaderType;
	GLuint m_VAOID;
	GLuint m_buffers[NUM_VBs];
	GLuint m_indexSize;

	GLenum m_drawingMode;

private:
	int m_ID;

};

#endif // DRAWABLE_H
