#include "Shader.h"

#include "ShaderSkyBox.h"
#include "ShaderWhiteLine.h"
#include "ShaderMap.h"

#include "ShaderNull.h"
#include "DirLightPassShader.h"
#include "GeomPassShader.h"
#include "PointLightPassShader.h"

//Initialisation du shader courant
Shader* Shader::m_shaders[NB_SHADER] = {0};
int Shader::m_shaderCourant = -1;

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Shader() 
///
/// Selectionne et initialise le shader courant
///
/// @return Shader courant
///
////////////////////////////////////////////////////////////////////////
Shader* Shader::getShader(int in_index)
{
	if(m_shaders[in_index] == 0)
	{
		switch(in_index)
		{
			case SKYBOX_SHADER:
				m_shaders[in_index] = new ShaderSkyBox(); break;
			case WHITE_LINE_SHADER:
				m_shaders[in_index] = new ShaderWhiteLine(); break;
			case MAP_SHADER:
				m_shaders[in_index] = new ShaderMap(); break;
			case DIR_LIGHT_PASS_SHADER:
				m_shaders[in_index] = new DirLightPassShader(); break;
			case GEOM_PASS_SHADER:
				m_shaders[in_index] = new GeomPassShader(); break;
			case POINT_LIGHT_PASS_SHADER:
				m_shaders[in_index] = new PointLightPassShader(); break;
			case NULL_SHADER:
				m_shaders[in_index] = new ShaderNull(); break;
			default:
				exit(-3);
		}

			if(!m_shaders[in_index]->initProgram())
				exit(-2);
	}
	return m_shaders[in_index];
}
////////////////////////////////////////////////////////////////////////
///
/// @fn ~Shader() 
///
/// Destructeur
/// Detruit les programmes sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Shader::~Shader()
{
	// Destruction des objets OpenGL
	glDeleteShader(m_vertexID);
	glDeleteShader(m_fragmentID);
	glDeleteProgram(m_programID);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initProgram()
///
/// Fonction permettant d'initialiser un programme composé d'un vertex et d'un fragment shader
/// 
/// @return Booléen confirmant que l'initialisation à pu être effectuée
///
////////////////////////////////////////////////////////////////////////
bool Shader::initProgram()
{
	// Création des shaders
	if(initShaderType(m_vertexID, GL_VERTEX_SHADER, m_vertexSource) == false)
		return false;

	if(initShaderType(m_fragmentID, GL_FRAGMENT_SHADER, m_fragmentSource) == false)
		return false;

	// Création du program

	m_programID = glCreateProgram();

	glAttachShader(m_programID, m_vertexID);
	glAttachShader(m_programID, m_fragmentID);

	// Linkage du program

	glLinkProgram(m_programID);
	bindAttribLocation();
	bindUniformLocation();

	// On vérifie que le link c'est bien passé

	GLint linkError(0);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &linkError);

	if(linkError != GL_TRUE)
	{
		// Récupération de la taille de l'erreur

		GLint errorSize(0);
		char* p_error(NULL);

		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &errorSize);


		// Allocation de l'erreur

		p_error = new char[errorSize + 1];


		// Copie de l'erreur dans la chaine de caractères

		glGetProgramInfoLog(m_programID, errorSize, &errorSize, p_error);
		p_error[errorSize] = '\0';


		// Affichage de l'erreur

		std::cout << "Erreur lors du link du program : " << p_error << std::endl;


		// On retourne false
		delete[] p_error;
		return false;
	}


	// Tout s'est bien passée, on retourne true
	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initShaderType(GLuint& i_shaderID, GLenum i_type, const std::string& ik_source)const
///
/// Fonction permettant d'initialiser un shader individuel
/// 
/// @param[in] i_shaderID: Valeur de retour du ID associé au shader
/// @param[in] i_type: Type de shader à initialiser
/// @param[in] ik_source: Code complet du shader
///
/// @return Booléen confirmant que l'initialisation du shader à pu être effectuée
///
////////////////////////////////////////////////////////////////////////
bool Shader::initShaderType(GLuint& i_shaderID, GLenum i_type, const std::string& ik_source)const
{
	// Génération de l'objet OpenGL Shader
	if(i_type == GL_VERTEX_SHADER || i_type == GL_FRAGMENT_SHADER)
		i_shaderID = glCreateShader(i_type);
	else
	{
		glDeleteShader(i_shaderID);
		return false;
	}

	// Ouverture du fichier source
	std::string sourceCode, str_sourceCodeLine;
	std::ifstream sourceFile(ik_source.c_str());

	//On test l'ouverture du fichier
	if(!sourceFile)
	{
		std::cout << "Erreur le fichier : " << ik_source << " n'existe pas" << std::endl;
		glDeleteShader(i_shaderID);
		return false;
	}

	//Si le fichier existe et qu'il est ouvert, alors on peut lire son contenu
	while(getline(sourceFile, str_sourceCodeLine))
	{
		sourceCode += str_sourceCodeLine + '\n';
	}

	sourceFile.close();

	// Compilation du shader
	GLint compilationError(0), gn_errorSize(0);
	const GLchar* p_sourceCodeStr = sourceCode.c_str();

	glShaderSource(i_shaderID, 1, &p_sourceCodeStr, NULL);
	glCompileShader(i_shaderID);

	// Vérification de la compilation
	glGetShaderiv(i_shaderID, GL_COMPILE_STATUS, &compilationError);

	if(compilationError != GL_TRUE)
	{
		// Récupération de la taille de l'erreur
		glGetShaderiv(i_shaderID, GL_INFO_LOG_LENGTH, &gn_errorSize);

		// Allocation d'une chaine de caractères
		char* p_error = new char[gn_errorSize + 1];
		p_error[gn_errorSize] = '\0';

		// Récupération de l'erreur
		glGetShaderInfoLog(i_shaderID, gn_errorSize, &gn_errorSize, p_error);

		// Affichage de l'erreur
		std::cout << "Erreur de compilation du shader (" << i_type << ") " << p_error << std::endl;

		// On libère la mémoire
		delete[] p_error;
		return false;
	}

	return true;
}