#ifndef SKYBOX_H
#define SKYBOX_H

#include <SDL/SDL.h>
#include <vector>
#include "../Rendering/Drawable.h"
#include "../Definition.h"

#define SKYBOX_HALFSIDE 2.0f

class Matrix;
class Camera;
class Vector;

class SkyBox: public Drawable
{
    public:
		SkyBox(ShaderType i_shaderType);
		~SkyBox();
		void animate(const Uint32 i_timestep);
    protected:
		virtual void preTreatementDraw()const;
		virtual void transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const;
    private:
		void load();
		void generateBufferData(std::vector<VertexPosition>& i_vertices, std::vector<GLuint>& i_indices);

		float m_angle;
};

#endif
