#ifndef GBUFFER_H
#define	GBUFFER_H

#include <GL/glew.h>

enum GBUFFER_TEXTURE_TYPE {
	GBUFFER_TEXTURE_TYPE_POSITION,
	GBUFFER_TEXTURE_TYPE_DIFFUSE,
	GBUFFER_TEXTURE_TYPE_NORMAL,
	GBUFFER_NUM_TEXTURES
};
class GBuffer
{
public:
    GBuffer(int i_offset, int i_targetTexture = -1);

    ~GBuffer();

    virtual bool init(unsigned int i_windowWidth, unsigned int i_windowHeight);

    virtual void startFrame();
    virtual void bindForGeomPass();
    virtual void bindForStencilPass();
    virtual void bindForLightPass();
	virtual void bindForSkyboxPass();
    virtual void bindForFinalPass();
	virtual void bindForWhiteLinePass();

	void linkTexture();

	int getTextureLocation(int i_index);

private:
    GLuint m_fbo;
    int m_textures[GBUFFER_NUM_TEXTURES];
    int m_depthTexture;
    int m_finalTexture;
	int m_offset;
};

#endif	/* GBUFFER_H */

