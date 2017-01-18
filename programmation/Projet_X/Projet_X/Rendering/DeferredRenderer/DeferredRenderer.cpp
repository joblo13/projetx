#include "DeferredRenderer.h"
#include "../Texture.h"
#include "../../Shaders/Shader.h"
#include "../../Camera/Camera.h"
#include "../../SceneOpenGL.h"
#include "../Drawable.h"
#include "../../Terrain/SkyBox.h"
#include "../../OpenGLHandler.h"

#include "../../Debugger/GlDebugger.h"

enum{SPHERE_LIGHT_MESH, QUAD_LIGHT_MESH, NB_LIGHT_MESH};

DeferredRenderer* DeferredRenderer::mp_instance = 0;

float fmax(float a, float b)
{
	if (a > b)
		return a;
	else
		return b;
}

DeferredRenderer::DeferredRenderer()
:m_lightRenderInformation(2, RenderInformation(this)), m_gBuffer(0), m_gBufferUnitPreview(1)
{        
    initLights();
	m_view.loadIdentity();
	m_projection.loadIdentity();
}


DeferredRenderer::~DeferredRenderer()
{

}    

bool DeferredRenderer::init()
{
	SceneOpenGL* p_sceneOpenGL(SceneOpenGL::getInstance());
    if (!m_gBuffer.init(p_sceneOpenGL->getWindowWidth(), p_sceneOpenGL->getWindowHeight())) 
        return false;   

	Shader::chooseShader(GEOM_PASS_SHADER);
	Shader* p_shader(Shader::getShader());
	
	Shader::chooseShader(POINT_LIGHT_PASS_SHADER);
	p_shader = Shader::getShader();
	
	m_gBuffer.linkTexture();
	glUniform2f(p_shader->getUniformVariableSafe("InScreenSize"), (float)p_sceneOpenGL->getWindowWidth(), (float)p_sceneOpenGL->getWindowHeight());

	Shader::chooseShader(DIR_LIGHT_PASS_SHADER);
	p_shader = Shader::getShader();

	m_gBuffer.linkTexture();

	glUniform3f(p_shader->getUniformVariableSafe("InDirectionalLight.Base.Color"), m_dirLight.Color.x, m_dirLight.Color.y, m_dirLight.Color.z);
	glUniform1f(p_shader->getUniformVariableSafe("InDirectionalLight.Base.AmbientIntensity"), m_dirLight.AmbientIntensity);
	Vector Direction = m_dirLight.Direction;
	Direction.normalize();
	glUniform3f(p_shader->getUniformVariableSafe("InDirectionalLight.Direction"), Direction.x, Direction.y, Direction.z);
	glUniform1f(p_shader->getUniformVariableSafe("InDirectionalLight.Base.DiffuseIntensity"), m_dirLight.DiffuseIntensity);

	glUniform2f(p_shader->getUniformVariableSafe("InScreenSize"), (float)p_sceneOpenGL->getWindowWidth(), (float)p_sceneOpenGL->getWindowHeight());

    Matrix WVP;
    WVP.loadIdentity();
	glUniformMatrix4fv(p_shader->getUniformVariableSafe("InProjectionModelview"), 1, GL_TRUE, (const GLfloat*)WVP.getValues());  

	Shader::chooseShader(NULL_SHADER);
       
	mp_lightMeshes.resize(NB_LIGHT_MESH);

	mp_skybox = new SkyBox(SKYBOX_SHADER);
	mp_lightMeshes[SPHERE_LIGHT_MESH] = Mesh::getMesh("../../../graphique/mesh/sphere.obj", POINT_LIGHT_PASS_SHADER);
	mp_lightMeshes[QUAD_LIGHT_MESH] = Mesh::getMesh("../../../graphique/mesh/quadLight.obj", DIR_LIGHT_PASS_SHADER);

	mp_lightMeshes[QUAD_LIGHT_MESH]->addToRenderInfoList(m_lightRenderInformation[0]);
    mp_lightMeshes[SPHERE_LIGHT_MESH]->addToRenderInfoList(m_lightRenderInformation[1]);

	return true;
}
    

void DeferredRenderer::draw()
{   
	drawScene();
}
void DeferredRenderer::drawScene()
{
	OpenGLHandler::enable(GL_DEPTH_TEST);
	OpenGLHandler::enable(GL_CULL_FACE);

	SceneOpenGL* p_sceneOpenGL(SceneOpenGL::getInstance());
	Camera* p_camera(p_sceneOpenGL->getCamera());
	p_camera->look(m_view, m_projection, p_sceneOpenGL->getWindowWidth(), p_sceneOpenGL->getWindowHeight());

	m_gBuffer.startFrame();

    doGeometryPass(m_gBuffer);

	// We need stencil to be enabled in the stencil pass to get the stencil buffer
	// updated and we also need it in the light pass because we render the light
	// only if the stencil passes.
	doSkyboxPass(m_gBuffer);
	OpenGLHandler::enable(GL_STENCIL_TEST);
	for (unsigned int i(0), nbPointLight(m_pointLights.size()); i < nbPointLight; ++i) 
	{
		doStencilPass(i, m_gBuffer);
		doPointLightPass(i, m_gBuffer);
	}

	//// The directional light does not need a stencil test because its volume
	//// is unlimited and the final pass simply copies the texture.
	OpenGLHandler::disable(GL_STENCIL_TEST);

	doDirectionalLightPass(m_gBuffer);

	doWhiteLinePass(m_gBuffer);

	doFinalPass(m_gBuffer);
}

void DeferredRenderer::drawUnitPreview()
{
	OpenGLHandler::enable(GL_DEPTH_TEST);
	OpenGLHandler::enable(GL_CULL_FACE);

	SceneOpenGL* p_sceneOpenGL(SceneOpenGL::getInstance());
	Camera* p_camera(p_sceneOpenGL->getCamera());
	p_camera->look(m_view, m_projection, p_sceneOpenGL->getWindowWidth(), p_sceneOpenGL->getWindowHeight());

	m_gBufferUnitPreview.startFrame();
	
    doGeometryPass(m_gBufferUnitPreview);

	// We need stencil to be enabled in the stencil pass to get the stencil buffer
	// updated and we also need it in the light pass because we render the light
	// only if the stencil passes.
	doSkyboxPass(m_gBufferUnitPreview);
	OpenGLHandler::enable(GL_STENCIL_TEST);
	for (unsigned int i(0), nbPointLight(m_pointLights.size()); i < nbPointLight; ++i) 
	{
		doStencilPass(i, m_gBufferUnitPreview);
		doPointLightPass(i, m_gBufferUnitPreview);
	}

	//// The directional light does not need a stencil test because its volume
	//// is unlimited and the final pass simply copies the texture.
	OpenGLHandler::disable(GL_STENCIL_TEST);

	doDirectionalLightPass(m_gBufferUnitPreview);

	doWhiteLinePass(m_gBufferUnitPreview);

	doFinalPass(m_gBufferUnitPreview);
}

void DeferredRenderer::doWhiteLinePass(GBuffer& i_buffer)
{
	i_buffer.bindForWhiteLinePass();
	Shader::chooseShader(WHITE_LINE_SHADER);

	const Camera* p_camera(SceneOpenGL::getInstance()->getCamera());

	OpenGLHandler::enable(GL_DEPTH_TEST);
	OpenGLHandler::disable(GL_BLEND);


	Matrix identityMatrix;
	identityMatrix.loadIdentity();

	for(unsigned int i(0), nbWhiteLine(mp_whiteLines.size()); i < nbWhiteLine; ++i) 
		mp_whiteLines[i]->draw(m_projection, m_view, p_camera); 		
}

void DeferredRenderer::doGeometryPass(GBuffer& i_buffer)
{
	Shader::chooseShader(GEOM_PASS_SHADER);

    i_buffer.bindForGeomPass();

	OpenGLHandler::disable(GL_BLEND);
	// Only the geometry pass updates the depth buffer
	glDepthMask(GL_TRUE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	OpenGLHandler::enable(GL_DEPTH_TEST);

	const Camera* p_camera(SceneOpenGL::getInstance()->getCamera());

    for(unsigned int i(0), nbMesh(mp_meshes.size()); i < nbMesh; ++i) 
		mp_meshes[i]->draw(m_projection, m_view, p_camera);  

	// When we get here the depth buffer is already populated and the stencil pass
	// depends on it, but it does not write to it.
	glDepthMask(GL_FALSE);		
	OpenGLHandler::enable(GL_BLEND);
}

void DeferredRenderer::doStencilPass(unsigned int i_pointLightIndex, GBuffer& i_buffer)
{
	Shader::chooseShader(NULL_SHADER);
	Shader* p_shader(Shader::getShader());

	// Disable color/depth write and enable stencil
	i_buffer.bindForStencilPass();
	OpenGLHandler::enable(GL_DEPTH_TEST);

    OpenGLHandler::disable(GL_CULL_FACE);
	OpenGLHandler::disable(GL_BLEND);

	glClear(GL_STENCIL_BUFFER_BIT);

	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	m_view.push();
	m_view.translate(m_pointLights[i_pointLightIndex].position.x, m_pointLights[i_pointLightIndex].position.y, m_pointLights[i_pointLightIndex].position.z);
	float scale = calcPointLightBSphere(m_pointLights[i_pointLightIndex]);        
	m_view.scale(scale);

	mp_lightMeshes[SPHERE_LIGHT_MESH]->setCustom(CULLFACE, GL_FRONT);
	mp_lightMeshes[SPHERE_LIGHT_MESH]->draw(m_projection, m_view, SceneOpenGL::getInstance()->getCamera());
	m_view.pop();
}

float calcPointLightBSphere(const PointLight& i_light)
{
	float maxChannel = fmax(fmax(i_light.Color.x, i_light.Color.y), i_light.Color.z);
        
	float ret = (-i_light.Attenuation.Linear + sqrtf(i_light.Attenuation.Linear * i_light.Attenuation.Linear - 4 * i_light.Attenuation.Exp * (i_light.Attenuation.Exp - 256 * maxChannel * i_light.DiffuseIntensity))) 
	/2 * i_light.Attenuation.Exp;
        
	return ret;
}  
void DeferredRenderer::doPointLightPass(unsigned int i_pointLightIndex, GBuffer& i_buffer)
{
	i_buffer.bindForLightPass();

	Shader::chooseShader(POINT_LIGHT_PASS_SHADER);
	Shader* p_shader(Shader::getShader());
	const Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
	Vector cameraPos(p_camera->getPosition());
	glUniform3f(p_shader->getUniformVariableSafe("InEyeWorldPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
      		
	OpenGLHandler::disable(GL_DEPTH_TEST);
	OpenGLHandler::enable(GL_BLEND);
	OpenGLHandler::blendEquation(GL_FUNC_ADD);
	OpenGLHandler::blendFunc(GL_ONE, GL_ONE);
        
    OpenGLHandler::enable(GL_CULL_FACE);	
	
	glUniform3f(p_shader->getUniformVariableSafe("InPointLight.Base.Color"), m_pointLights[i_pointLightIndex].Color.x, m_pointLights[i_pointLightIndex].Color.y, m_pointLights[i_pointLightIndex].Color.z);
	glUniform1f(p_shader->getUniformVariableSafe("InPointLight.Base.AmbientIntensity"), m_pointLights[i_pointLightIndex].AmbientIntensity);
	glUniform1f(p_shader->getUniformVariableSafe("InPointLight.Base.DiffuseIntensity"), m_pointLights[i_pointLightIndex].DiffuseIntensity);
	glUniform3f(p_shader->getUniformVariableSafe("InPointLight.Position"), m_pointLights[i_pointLightIndex].position.x, m_pointLights[i_pointLightIndex].position.y, m_pointLights[i_pointLightIndex].position.z);
	glUniform1f(p_shader->getUniformVariableSafe("InPointLight.Atten.Constant"), m_pointLights[i_pointLightIndex].Attenuation.Constant);
	glUniform1f(p_shader->getUniformVariableSafe("InPointLight.Atten.Linear"), m_pointLights[i_pointLightIndex].Attenuation.Linear);
	glUniform1f(p_shader->getUniformVariableSafe("InPointLight.Atten.Exp"), m_pointLights[i_pointLightIndex].Attenuation.Exp);

	m_view.push();
	m_view.translate(m_pointLights[i_pointLightIndex].position.x, m_pointLights[i_pointLightIndex].position.y, m_pointLights[i_pointLightIndex].position.z);
	float scale = calcPointLightBSphere(m_pointLights[i_pointLightIndex]);        
	m_view.scale(scale);
	mp_lightMeshes[SPHERE_LIGHT_MESH]->setCustom(CULLFACE, GL_FRONT);
    mp_lightMeshes[SPHERE_LIGHT_MESH]->draw(m_projection, m_view, p_camera); 
    m_view.pop();

	OpenGLHandler::disable(GL_BLEND);
}
	

void DeferredRenderer::doDirectionalLightPass(GBuffer& i_buffer)
{
	i_buffer.bindForLightPass();

	Shader::chooseShader(DIR_LIGHT_PASS_SHADER);
	Shader* p_shader(Shader::getShader());
	const Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
	Vector cameraPos(p_camera->getPosition());
	glUniform3f(p_shader->getUniformVariableSafe("InEyeWorldPos"), cameraPos.x, cameraPos.y, cameraPos.z);


	OpenGLHandler::disable(GL_DEPTH_TEST);
	OpenGLHandler::enable(GL_BLEND);
	OpenGLHandler::blendEquation(GL_FUNC_ADD);
	OpenGLHandler::blendFunc(GL_ONE, GL_ONE);

	
	Matrix identityMatrix;
	identityMatrix.loadIdentity();

	OpenGLHandler::disable(GL_CULL_FACE);
    mp_lightMeshes[QUAD_LIGHT_MESH]->draw(identityMatrix, identityMatrix, p_camera);
	OpenGLHandler::enable(GL_CULL_FACE);

	OpenGLHandler::disable(GL_BLEND);
}

void DeferredRenderer::doSkyboxPass(GBuffer& i_buffer)
{
	const Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
	i_buffer.bindForSkyboxPass();
	Shader::chooseShader(SKYBOX_SHADER);
	mp_skybox->draw(m_projection, m_view, p_camera);
}

void DeferredRenderer::doFinalPass(GBuffer& i_buffer)
{
	SceneOpenGL* p_sceneOpenGL(SceneOpenGL::getInstance());
	i_buffer.bindForFinalPass();
    glBlitFramebuffer(0, 0, p_sceneOpenGL->getWindowWidth(), p_sceneOpenGL->getWindowHeight(), 
                        0, 0, p_sceneOpenGL->getWindowWidth(), p_sceneOpenGL->getWindowHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
}


// The calculation solves a quadratic equation (see http://en.wikipedia.org/wiki/Quadratic_equation)
float DeferredRenderer::calcPointLightBSphere(const PointLight& i_light)
{
    float MaxChannel = fmax(fmax(i_light.Color.x, i_light.Color.y), i_light.Color.z);
        
    float ret = (-i_light.Attenuation.Linear + sqrtf(i_light.Attenuation.Linear * i_light.Attenuation.Linear - 4 * i_light.Attenuation.Exp * (i_light.Attenuation.Exp - 256 * MaxChannel * i_light.DiffuseIntensity))) 
                /
                2 * i_light.Attenuation.Exp;
        
    return ret*1.5f;
}    
        
void DeferredRenderer::initLights()
{
    m_spotLight.AmbientIntensity = 0.2f;
    m_spotLight.DiffuseIntensity = 0.7f;
	m_spotLight.Color = COLOR_WHITE;
    m_spotLight.Attenuation.Linear = 0.01f;
    m_spotLight.position  = Vector(-20.0, 20.0, 5.0f);
    m_spotLight.Direction = Vector(1.0f, -1.0f, 0.0f);
    m_spotLight.Cutoff =  20.0f;

	m_dirLight.AmbientIntensity = 0.1f;
	m_dirLight.Color = COLOR_WHITE;
	m_dirLight.DiffuseIntensity = 0.7f;
	m_dirLight.Direction = Vector(1.0f, -1.0f, 0.5f);

	PointLight pointLight;

	pointLight.DiffuseIntensity = 0.9f;
	pointLight.Color = COLOR_GREEN;
    pointLight.position = Vector(0.0f, 16.5f, 0.0f);
	pointLight.Attenuation.Constant = 0.0f;
    pointLight.Attenuation.Linear = 0.0f;
    pointLight.Attenuation.Exp = 0.3f;

	m_pointLights.push_back(pointLight);
}

int DeferredRenderer::addDrawable(Drawable* i_drawable)
{
	if(i_drawable->getID() != -1)
		return i_drawable->getID();

	int ID(mp_meshes.size());
	i_drawable->setID(ID);

	mp_meshes.push_back(i_drawable);

	Shader::chooseShader(GEOM_PASS_SHADER);
	const std::vector<int>* texturesIndex;
	texturesIndex = i_drawable->getTexturesIndex();
	for(int i(0), texturesIndexSize(texturesIndex->size()); i < texturesIndexSize; ++i)
	{
		if((*texturesIndex)[i] != -1)
		{
			Texture::chooseTexture((*texturesIndex)[i]);
			Texture::linkTexture();
		}
	}

	return ID;

}

int DeferredRenderer::addWhiteLine(Drawable* i_whiteLine)
{
	if(i_whiteLine->getID() != -1)
		return i_whiteLine->getID();

	int ID(mp_whiteLines.size());
	i_whiteLine->setID(ID);

	mp_whiteLines.push_back(i_whiteLine);

	return ID;
}

void DeferredRenderer::removeDrawable(Drawable* i_drawable)
{
	int ID(i_drawable->getID());
	if(ID == -1)
		return;
	mp_meshes[ID] = mp_meshes[mp_meshes.size()-1];

	mp_meshes[ID]->setID(ID);
	i_drawable->setID(-1);
	mp_meshes.pop_back();
}

void DeferredRenderer::removeWhiteLine(Drawable* i_whiteLine)
{
	int ID(i_whiteLine->getID());
	if(ID == -1)
		return;
	mp_whiteLines[ID] = mp_whiteLines[mp_whiteLines.size()-1];

	mp_whiteLines[ID]->setID(ID);
	i_whiteLine->setID(-1);
	mp_whiteLines.pop_back();
}

void DeferredRenderer::animate(Uint32 i_timestep)
{
	Shader::chooseShader(GEOM_PASS_SHADER);
	for(int i(0), meshesNb(mp_meshes.size()); i < meshesNb; ++i)
		mp_meshes[i]->animate(i_timestep);

	mp_skybox->animate(i_timestep);
}