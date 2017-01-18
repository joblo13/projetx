#include <stdio.h>

#include "Util.h"
#include "gbuffer.h"

#include "../Texture.h"

GBuffer::GBuffer(int i_offset, int i_targetTexture)
:m_offset(i_offset), m_finalTexture(i_targetTexture)
{
    m_fbo = 0;
	m_depthTexture = 0;
    ZERO_MEM(m_textures);
}

GBuffer::~GBuffer()
{
    if (m_fbo != 0) 
        glDeleteFramebuffers(1, &m_fbo);

	for(unsigned int i(0); i < ARRAY_SIZE_IN_ELEMENTS(m_textures); ++i)
		if (m_textures[0] != 0) 
		{
			Texture::chooseTexture(m_textures[i]);
			Texture::releaseTextures();
		}

	if (m_depthTexture != 0)
	{
		Texture::chooseTexture(m_depthTexture);
		Texture::releaseTextures();
	}

	if (m_finalTexture != 0)
	{
		Texture::chooseTexture(m_finalTexture);
		Texture::releaseTextures();
	}
}

bool GBuffer::init(unsigned int i_windowWidth, unsigned int i_windowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);    
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
	
	m_textures[GBUFFER_TEXTURE_TYPE_POSITION] = Texture::initTexture("InPositionMap");
	m_textures[GBUFFER_TEXTURE_TYPE_DIFFUSE] = Texture::initTexture("InColorMap");
	m_textures[GBUFFER_TEXTURE_TYPE_NORMAL] = Texture::initTexture("InNormalMap");

	m_depthTexture = Texture::initTexture();

	if(m_finalTexture == -1)
		m_finalTexture = Texture::initTexture();

    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(m_textures) ; ++i) 
	{
		Texture::chooseTexture(m_textures[i]);
		Texture::bindTexture();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, i_windowWidth, i_windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i + m_offset, GL_TEXTURE_2D, Texture::getTextureLocation(), 0);
    }

	// depth
	Texture::chooseTexture(m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, Texture::getTextureLocation());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, i_windowWidth, i_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, Texture::getTextureLocation(), 0);

	// final
	Texture::chooseTexture(m_finalTexture);
	glBindTexture(GL_TEXTURE_2D, Texture::getTextureLocation());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_windowWidth, i_windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset, GL_TEXTURE_2D, Texture::getTextureLocation(), 0);	

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);


    if (Status != GL_FRAMEBUFFER_COMPLETE) 
	{
        printf("FrameBuffer error, status: 0x%x\n", Status);
        return false;
    }

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}


void GBuffer::startFrame()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	OpenGLHandler::drawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset);
	glClear(GL_COLOR_BUFFER_BIT);
}


void GBuffer::bindForGeomPass()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

	std::vector<GLenum> drawBuffers;
	drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + m_offset);
	drawBuffers.push_back(GL_COLOR_ATTACHMENT1 + m_offset);
	drawBuffers.push_back(GL_COLOR_ATTACHMENT2 + m_offset);

    OpenGLHandler::drawBuffers(drawBuffers);
}

void GBuffer::bindForWhiteLinePass()
{
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    OpenGLHandler::drawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset);
}


void GBuffer::bindForStencilPass()
{
    // must disable the draw buffers 
	OpenGLHandler::drawBuffer(GL_NONE);
} 



void GBuffer::bindForLightPass()
{
	OpenGLHandler::drawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset);

	for (unsigned int i(0) ; i < GBUFFER_NUM_TEXTURES; ++i) 
	{
		Texture::chooseTexture(m_textures[i]);
		Texture::bindTexture();
	}
}

void GBuffer::bindForSkyboxPass()
{
	OpenGLHandler::drawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset);
}


void GBuffer::bindForFinalPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_NUM_TEXTURES + m_offset);
}

int GBuffer::getTextureLocation(int i_index)
{
	Texture::chooseTexture(m_textures[i_index]);
	return Texture::getTextureLocation();
}

void GBuffer::linkTexture()
{
	for(unsigned int i(0); i < GBUFFER_NUM_TEXTURES; ++i)
	{
		Texture::chooseTexture(m_textures[i]);
		Texture::linkTexture();
	}
}