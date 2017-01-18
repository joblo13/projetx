#ifndef SHADER_H
#define SHADER_H

//Includes Externe
#include <iostream>
#include <string>
#include <fstream>
#include <hash_map>
#include <hash_set>
#include <assert.h>

//Includes interne
#include "../Definition.h"

//Shaders
enum ShaderType{SKYBOX_SHADER, WHITE_LINE_SHADER, MAP_SHADER, 
NULL_SHADER, POINT_LIGHT_PASS_SHADER, GEOM_PASS_SHADER, DIR_LIGHT_PASS_SHADER, NB_SHADER};

class Shader
{
	public:
		//Singleton avec instance multiple
		static Shader* getShader(){return m_shaders[m_shaderCourant];}
		static int getCurrentShader(){return m_shaderCourant;}

		//Fonction permettant de choisir le shader courant
		static void chooseShader(int i_index)
		{
			//Le paramètre ne devrait jamais poser d'erreur puisqu'il est définie par le programmeur
			assert(i_index < NB_SHADER);
			assert(i_index >= -1);

			//Si le shader choisi est plus grand que 1, on utilise des shaders
			if(i_index >= 0)
			{
				//On modifie le shader courant si il est différent de la proposition
				if( m_shaderCourant != i_index )
				{
					m_shaderCourant = i_index;
					glUseProgram(getShader(i_index)->getProgramID());
				}
			}
			//Si le shader choisi est le -1, on utilise le pipeline d'opengl
			else if(i_index == -1)
			{
					m_shaderCourant = i_index;
					glUseProgram(0);
			}
		}

		static void releaseShaders()
		{
			//On détruit tout les shaders utilisés
			for(int n_i = 0; n_i < NB_SHADER; ++n_i)
			{
				if(m_shaders[n_i] != 0)
				{
					delete m_shaders[n_i];
					m_shaders[n_i] = 0;
				}
			}
		}
	
		//Acceseur
		GLuint getProgramID()const{return m_programID;}

		int getUniformVariable(const std::string& ik_index)const
		{
			std::hash_map<std::string, int>::const_iterator iterator(m_uniformVariable.find(ik_index));
			if(iterator == m_uniformVariable.end())
				return -1;
			return iterator->second;
		}

		int getUniformVariableSafe(const std::string& ik_index)const
		{
			int uniform(getUniformVariable(ik_index));
			assert(uniform != -1);
			return uniform;
		}

		inline int getAttribVariable(const std::string& ik_index)const
		{
			std::hash_map<std::string, int>::const_iterator iterator(m_attribVariable.find(ik_index));
			if(iterator == m_attribVariable.end())
				return -1;
			return iterator->second;
		}

		inline int getAttribVariableSafe(const std::string& i_index)const
		{
			int attrib = getAttribVariable(i_index);
			//assert(attrib != -1);
			return attrib;
		}

	private:
		//Fonction privée gérant la création des différents shaders
		static Shader* getShader(int i_index);

		//Fonction de création et de destruction des shaders
		virtual bool initShaderType(GLuint& i_shaderID, GLenum i_type, const std::string& ik_source)const;
		virtual bool initProgram();
		
		//Fonction permettant de trouver la localisation des variables des shaders
		virtual void bindAttribLocation() = 0;
		virtual void bindUniformLocation() = 0;

	protected:
		//Singleton
		Shader(){}
		virtual ~Shader() = 0;

		//Chemin d'accès des fichiers sources des shaders
		std::string m_vertexSource;
		std::string m_fragmentSource;

		//ID des différents programmes compilé sur la carte graphique
		GLuint m_vertexID;
		GLuint m_fragmentID;
		GLuint m_programID;

		//Liste de la localisation des variables sur la carte graphique
		std::hash_map<std::string, int> m_uniformVariable;
		std::hash_map<std::string, int> m_attribVariable;

		//Liste des shaders créés
		static Shader* m_shaders[NB_SHADER];

		//Shader courant
		static int m_shaderCourant;
};


#endif //SHADER_H
