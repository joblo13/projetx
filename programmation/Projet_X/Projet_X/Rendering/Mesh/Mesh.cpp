#include "Mesh.h"
#include "../Texture.h"
#include "../../Shaders/Shader.h"

#include "../../Math/Matrix3x3.h"
#include "../../Math/Vector.h"
#include "../../Camera/Camera.h"
#include "../../Entities/Unit.h"
#include "../../Terrain/Terrain.h"
#include "../RenderInformation.h"
#include "../../Debugger/GlDebugger.h"
#include "../DeferredRenderer/DeferredRenderer.h"

enum {POSITION_LOCATION, TEX_COORD_LOCATION, NORMAL_LOCATION, BONE_ID_LOCATION, BONE_WEIGHT_LOCATION};

#define SPECIAL_TEXTURE 2

std::hash_map<std::string, Mesh*> Mesh::mp_meshes;

Mesh* Mesh::getMesh(const std::string& i_path, ShaderType i_shaderType, int i_alphaColor)
{
	std::hash_map<std::string, Mesh*>::iterator it_meshes(mp_meshes.find(i_path));
	if(it_meshes == mp_meshes.end())
	{
		mp_meshes.insert(std::pair<std::string, Mesh*>(i_path, new Mesh(i_path, i_shaderType, i_alphaColor)));
		Mesh* p_mesh(mp_meshes[i_path]);
		if(!p_mesh->loadMesh(i_path))
			exit(-10);

		if(i_shaderType == GEOM_PASS_SHADER)
			DeferredRenderer::getInstance()->addDrawable(p_mesh);
		return p_mesh;
	}
	else
	{
		return it_meshes->second;
	}
}
Mesh* Mesh::getLoadedMesh(const std::string& i_path)
{
	std::hash_map<std::string, Mesh*>::iterator it_meshes(mp_meshes.find(i_path));
	if(it_meshes == mp_meshes.end())
	{
		return 0;
	}
	else
	{
		return it_meshes->second;
	}
}
////////////////////////////////////////////////////////////////////////
///
/// @fn Mesh()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Mesh::Mesh(const std::string& i_path, ShaderType i_shaderType, int i_alphaColor)
: m_numBones(0), m_animTextureID(-1), m_path(i_path), m_animated(false), Instanciable(i_shaderType), m_alphaColor(i_alphaColor)
{
	m_customisation[CULLFACE] = GL_BACK;
	m_customisation[UP] = UP_Y;
	loadInfoFromFile();
    ZERO_MEM(m_buffers);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Mesh()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Mesh::~Mesh()
{
	clear();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void clear()
///
/// Fonction permettant de vider la mémoire de la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::clear()
{
    if (m_buffers[0] != 0) 
	{
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);
    }
       
    if (m_VAOID != 0) 
	{
        glDeleteVertexArrays(1, &m_VAOID);
        m_VAOID = 0;
    }
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool loadMesh(const std::string& ik_filename)
///
/// Fonction permettant de charger une mesh dans la carte graphique
/// 
/// @param[in] ik_filename : nom du fichier à charger
///
/// @return Vrai si le chargement a réussi
///
////////////////////////////////////////////////////////////////////////
bool Mesh::loadMesh(const std::string& ik_filename)
{
    //On libère la mémoire
    clear();
 
    //On crée les VAO
    glGenVertexArrays(1, &m_VAOID);   
    glBindVertexArray(m_VAOID);
    
    //On crée les buffers pour les sommets
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_buffers), m_buffers);

    bool isLoaded(false);    
  
    const aiScene* p_scene = m_importer.ReadFile(ik_filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
	//Initialisation du mesh
    if (p_scene)  
        isLoaded = initFromScene(p_scene, ik_filename);
    else 
        printf("Error parsing '%s': '%s'\n", ik_filename.c_str(), m_importer.GetErrorString());

    //On s'assure que les modifications n'affecte pas l'extérieur
    glBindVertexArray(0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    return isLoaded;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initFromScene(const aiScene* ip_scene, const std::string& ik_filename)
///
/// initialisation du mesh
/// 
/// @param[in] ip_scene : scène contenant tous les meshes
/// @param[in] ik_filename : nom du fichier du mesh
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Mesh::initFromScene(const aiScene* ip_scene, const std::string& ik_filename)
{  
	m_animated = ip_scene->HasAnimations();
    m_entries.resize(ip_scene->mNumMeshes);
    m_texturesIndex.resize(ip_scene->mNumMaterials, -1);
	m_shininess.resize(ip_scene->mNumMaterials, -1);
	m_shininessStrength.resize(ip_scene->mNumMaterials, -1);

    std::vector<VertexPosition> vertexInformations;
    std::vector<VertexBoneData> bones;
    std::vector<unsigned int> indices;
       
    unsigned int numVertices = 0;
    unsigned int numIndices = 0;
    
    //On extrait les informations de la scene
    for (unsigned int i(0), entriesSize(m_entries.size()); i < entriesSize; ++i) 
	{
        m_entries[i].materialIndex = ip_scene->mMeshes[i]->mMaterialIndex;        
        m_entries[i].numIndices    = ip_scene->mMeshes[i]->mNumFaces * 3;
        m_entries[i].baseVertex    = numVertices;
        m_entries[i].baseIndex     = numIndices;
        
        numVertices += ip_scene->mMeshes[i]->mNumVertices;
        numIndices  += m_entries[i].numIndices;
    }
    
    //On reserve de l'espace mémoire pour les sommets et les index
    vertexInformations.reserve(numVertices);
    bones.resize(numVertices);
    indices.reserve(numIndices);
        
    //Initialisation des meshes de la scene un par un
    for (unsigned int i(0), entriesSize(m_entries.size()); i < entriesSize; ++i) 
	{
        const aiMesh* p_aiMesh = ip_scene->mMeshes[i];
        initMesh(i, p_aiMesh, vertexInformations, bones, indices);
    }
	if(m_animated)
		createBoneTree(ip_scene);


    if (!initMaterials(ip_scene, ik_filename)) 
	{
        return false;
    }

	Shader::chooseShader(m_shaderType);
	Shader* p_currentShader(Shader::getShader());
    //On remplis les tableaux
	int attribVariable;

	//On ajoute les coordonnées de sommets
	attribVariable = p_currentShader->getAttribVariableSafe("InVertex");

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexInformations[0]) * vertexInformations.size(), &vertexInformations[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(0)); 
	//On ajoute les normals
	if(m_shaderType == GEOM_PASS_SHADER)
	{	
		attribVariable = p_currentShader->getAttribVariableSafe("InNormal");
		glEnableVertexAttribArray(attribVariable);
		glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(3*sizeof(float)));
	}
	//On ajoute les coordonnées de textures
	if(m_shaderType == GEOM_PASS_SHADER)
	{	
		attribVariable = p_currentShader->getAttribVariableSafe("InUV");
		glEnableVertexAttribArray(attribVariable);
		glVertexAttribPointer(attribVariable, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(6*sizeof(float) + sizeof(int)));
	}

	//On ajoute les identifiants de bones
	if(m_animated)
	{
		attribVariable = p_currentShader->getAttribVariableSafe("InBoneIDs");

		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[BONE_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(attribVariable);
		glVertexAttribIPointer(attribVariable, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	
		//On ajoute les poids des bones
		attribVariable = p_currentShader->getAttribVariableSafe("InWeights");

		glEnableVertexAttribArray(attribVariable);    
		glVertexAttribPointer(attribVariable, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
	}

	//On ajoute la couleur
	if(m_shaderType == GEOM_PASS_SHADER)
	{	
		std::vector<int> color(vertexInformations.size());
		for(int i(0), colorSize(color.size()); i < colorSize; ++i)
		{
			color[i] = m_alphaColor;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_buffers[COLOR_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color[0]) * color.size(), &color[0], GL_STATIC_DRAW);
		attribVariable = p_currentShader->getAttribVariableSafe("InColor");
		glEnableVertexAttribArray(attribVariable);
		glVertexAttribIPointer(p_currentShader->getAttribVariable("InColor"), 1, GL_INT, sizeof(color[0]), BUFFER_OFFSET(0));
	}

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void initMesh(unsigned int i_meshIndex, const aiMesh* ip_aiMesh, std::vector<Vector>& i_positions, std::vector<Vector>& i_normals, std::vector<VertexUV>& i_texCoords, std::vector<VertexBoneData>& i_bones, std::vector<unsigned int>& i_indices)
///
/// Constructeur
/// 
/// @param[in] i_meshIndex : index du mesh
/// @param[in] ip_aiMesh : mesh à initialiser 
/// @param[in] i_positions : position des sommets
/// @param[in] i_normals : normal des sommets
/// @param[in] i_texCoords : coordonnées de textures
/// @param[in] i_bones : bones pour l'animation
/// @param[in] i_indices : indices des sommets
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::initMesh(unsigned int i_meshIndex, const aiMesh* ip_aiMesh, std::vector<VertexPosition>& i_vertices, std::vector<VertexBoneData>& i_bones, std::vector<unsigned int>& i_indices)
{    
    const aiVector3D k_zero3D(0.0f, 0.0f, 0.0f);
    
    //On ajoute les sommets dans la mémoire
	VertexPosition vertexPosition;
    for(unsigned int i(0), numFaces(ip_aiMesh->mNumVertices); i < numFaces; ++i) 
	{
        const aiVector3D* kp_pos(&(ip_aiMesh->mVertices[i]));
        const aiVector3D* kp_normal(&(ip_aiMesh->mNormals[i]));
        const aiVector3D* kp_texCoord(ip_aiMesh->HasTextureCoords(0) ? &(ip_aiMesh->mTextureCoords[0][i]) : &k_zero3D);

		vertexPosition.x = kp_pos->x;
		vertexPosition.y = kp_pos->y;
		vertexPosition.z = kp_pos->z;
		vertexPosition.normal[0] = kp_normal->x;
		vertexPosition.normal[1] = kp_normal->y;
		vertexPosition.normal[2] = kp_normal->z;

		vertexPosition.u = kp_texCoord->x;
		vertexPosition.v = kp_texCoord->y;
        i_vertices.push_back(vertexPosition);        
    }
    generateBaseCorrection(i_vertices);
	if(m_animated)
		loadBones(i_meshIndex, ip_aiMesh, i_bones);
    
    //On ajoute l'index en mémoire
    for (unsigned int i(0), numFaces(ip_aiMesh->mNumFaces); i < numFaces; ++i) 
	{
        const aiFace& k_face = ip_aiMesh->mFaces[i];
        assert(k_face.mNumIndices == 3);
        i_indices.push_back(k_face.mIndices[0]);
        i_indices.push_back(k_face.mIndices[1]);
        i_indices.push_back(k_face.mIndices[2]);
    }
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool initMaterials(const aiScene* ip_scene, const std::string& ik_filename)
///
/// Fonction permettant d'initialiser les matériaux
/// 
/// @param[in] ip_scene : scene contenant tout les meshes
/// @param[in] ik_filename : nom du fichier à initialiser
///
/// @return Vrai si l'initialisation a réussi
///
////////////////////////////////////////////////////////////////////////
bool Mesh::initMaterials(const aiScene* ip_scene, const std::string& ik_filename)
{
    //On extrait le nom du fichier
    std::string::size_type slashIndex(ik_filename.find_last_of("/"));
    std::string directory;

    if (slashIndex == std::string::npos)
        directory = ".";
    else if (slashIndex == 0)
        directory = "/";
    else
        directory = ik_filename.substr(0, slashIndex);

    bool isInitialised(true);

    //On initialise le matériel
    for (unsigned int i(0), numMaterials(ip_scene->mNumMaterials); i < ip_scene->mNumMaterials ; ++i) 
	{
        const aiMaterial* kp_material = ip_scene->mMaterials[i];

		//Si il existe des textures
        if (kp_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
		{
            aiString path;

            if (kp_material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) 
			{
                std::string strPath(path.data);
                
                if (strPath.substr(0, 2) == ".\\")                    
                    strPath = strPath.substr(2, strPath.size() - 2);
                               
                std::string fullPath = directory + "/" + strPath;
                    
                m_texturesIndex[i] = Texture::initTexture("TextureSampler", fullPath.c_str());

				float shininess(1);
				kp_material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess);
				m_shininessStrength[i] = shininess;

				kp_material->Get(AI_MATKEY_SHININESS, shininess);
				m_shininess[i] = shininess;
				
            }
        }
    }

    return isInitialised;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void loadBones(unsigned int i_meshIndex, const aiMesh* ip_mesh, std::vector<VertexBoneData>& i_bones)
///
/// Fonction permettant de charger les informations sur les bones
/// 
/// @param[in] i_meshIndex : index du mesh contenant les bones à charger
/// @param[in] ip_mesh : mesh contenant les bones à charger
/// @param[in] i_bones : liste des bones
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::loadBones(unsigned int i_meshIndex, const aiMesh* ip_mesh, std::vector<VertexBoneData>& i_bones)
{
	//Pour chaque bones
    for(unsigned int i(0), numBones(ip_mesh->mNumBones); i < numBones; ++i) 
	{                
        unsigned int boneIndex(0);        
        std::string boneName(ip_mesh->mBones[i]->mName.data);
        
		//S'il n'est pas dans la liste on l'ajoute
        if (m_boneMapping.find(boneName) == m_boneMapping.end()) 
		{
            boneIndex = m_numBones;
            m_numBones++;                 
            m_boneMapping[boneName] = boneIndex;
        }
		//Sinon on y accède
        else 
		{
            boneIndex = m_boneMapping[boneName];
        }                      
        
		//On ajoute les informations au bones
        for(unsigned int j(0); j < ip_mesh->mBones[i]->mNumWeights ; ++j) 
		{
            unsigned int VertexID = m_entries[i_meshIndex].baseVertex + ip_mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = ip_mesh->mBones[i]->mWeights[j].mWeight;                   
            i_bones[VertexID].addBoneData(boneIndex, Weight);
        }
    }    
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addBoneData(unsigned int i_boneID, float i_weight)
///
/// Fonction ajoutant un bones à un sommet avec un poids
/// 
/// @param[in] i_boneID : bones affectant le sommet
/// @param[in] i_weight : point du bones pour le sommet
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void VertexBoneData::addBoneData(unsigned int i_boneID, float i_weight)
{
	//On trouve le premier emplacement de bone qui n'est pas encore utilisé
    for(unsigned int i(0) ; i < ARRAY_SIZE_IN_ELEMENTS(IDs); ++i) 
	{
        if (weights[i] == 0.0) 
		{
            IDs[i]     = i_boneID;
            weights[i] = i_weight;
            return;
        }        
    }
    
    //si on se rend à cette ligne, il y a eu une erreur
    assert(0);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void boneTransform(float i_timeInSeconds, std::vector<Matrix>& i_transforms)
///
/// Fonction permettant de 
/// 
/// @param[in] i_timeInSeconds : temps total en seconde
/// @param[in] i_transforms : vecteur de matrice pour chaque bones
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::boneTransform(Unit* ip_unit)
{
	AnimationInfo* p_animationInfo(&ip_unit->m_renderInformation.m_animationInfo);

	//On charge la matrice identitée
	Matrix identity;
	identity.loadIdentity();

	//On trouve le tick actuel
	float timeInTicks(p_animationInfo->m_totalAnimationTimeSecond * m_animations[p_animationInfo->m_currentAnimation].m_ticksPerSecond * p_animationInfo->m_animationSpeedModifier);
	//On trouve le temps de l'animation relié au tick
	float animationTime(fmod(timeInTicks, m_animations[p_animationInfo->m_currentAnimation].m_duration));

	if(p_animationInfo->m_invertedAnimation && animationTime != 0.0f)
	{
		animationTime = m_animations[p_animationInfo->m_currentAnimation].m_duration - animationTime;
	}

	//On determine les transformations de façon récursive
	evaluateLocalTransformation(animationTime, p_animationInfo);

	//On retourne les transformations finales de tout les bones
	for(int i(0); i < m_boneList.size(); ++i) 
	{
		p_animationInfo->m_transforms.push_back(m_boneList[i]->m_finalTransform);
	}
}
void Mesh::boneTransformBaked(Unit* ip_unit)
{
	AnimationInfo* p_animationInfo(&ip_unit->m_renderInformation.m_animationInfo);
	//On trouve le tick actuel
	float timeInTicks(p_animationInfo->m_totalAnimationTimeSecond * m_animations[p_animationInfo->m_currentAnimation].m_ticksPerSecond * p_animationInfo->m_animationSpeedModifier);
	//On trouve le temps de l'animation relié au tick
	float animationTime(fmod(timeInTicks, m_animations[p_animationInfo->m_currentAnimation].m_duration));

	if(p_animationInfo->m_invertedAnimation && animationTime != 0.0f)
		animationTime = m_animations[p_animationInfo->m_currentAnimation].m_duration - animationTime;

	int startingFrame(0);
	if(p_animationInfo->m_lastTime < animationTime)
		startingFrame = p_animationInfo->m_currentFrame;

	p_animationInfo->m_currentFrame = 0;
	//On itère sur tout les keyframes et on trouve la première qui est plus grande que le temps fournis
	for (unsigned int i(startingFrame), animationSize(m_bakedAnimations[p_animationInfo->m_currentAnimation].size() - 1); i < animationSize; ++i) 
		if(animationTime < m_bakedAnimations[p_animationInfo->m_currentAnimation][i].first) 
		{
			p_animationInfo->m_currentFrame = i;
			break;
		}
}

void Mesh::bakeAnimation(int i_animationIndex)
{
	if(m_bakedAnimations.size() <= m_animations.size())
		m_bakedAnimations.resize(m_animations.size());

	AnimationInfo* animationInfo = new AnimationInfo();
	float time;
	const float framePerTime(1);

	int numFrame(m_animations[i_animationIndex].m_duration*framePerTime);

	if(m_bakedAnimations[i_animationIndex].size() <= numFrame)
		m_bakedAnimations[i_animationIndex].resize(numFrame);

	for(int i(0); i < numFrame; ++i)
	{
		time = i/framePerTime;
		//On determine les transformations de façon récursive
		evaluateLocalTransformation(time, animationInfo);

		//On retourne les transformations finales de tout les bones
		m_bakedAnimations[i_animationIndex][i].first = time;
		for(int j(0); j < m_boneList.size(); ++j) 
		{
			m_bakedAnimations[animationInfo->m_currentAnimation][i].second.push_back(m_boneList[j]->m_finalTransform);
		}
	}
	delete animationInfo;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void readNodeHeirarchy(float i_animationTime, const aiNode* ikp_node, const Matrix& ik_parentTransform)
///
/// Fonction récursive permetant de trouver les transformations selon leur parent
/// 
/// @param[in] i_animationTime : temps actuel de l'animation en secondes
/// @param[in] ikp_node : noeud traité
/// @param[in] ik_parentTransform : transformation parente du noeud
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::evaluateLocalTransformation(float i_animationTime, AnimationInfo* ip_animationInfo)
{
	int channelIndex(-1);
	for(int i(0), boneListSize(m_boneList.size()); i < boneListSize; ++i)
	{	
		channelIndex = m_animations[ip_animationInfo->m_currentAnimation].m_nameToIndexChannel.find(m_boneList[i]->m_boneName)->second;
		if(channelIndex >= 0)
		{
			//Création de la matrice de scaling
			aiVector3D scaling;
			calcInterpolatedScaling(scaling, i_animationTime, channelIndex, ip_animationInfo);
			Matrix scalingMatrix;
			scalingMatrix.loadIdentity();
			scalingMatrix.scale(scaling.x);

			//Création de la matrice de rotation
			aiQuaternion rotationQuaternion;
			calcInterpolatedRotation(rotationQuaternion, i_animationTime, channelIndex, ip_animationInfo);        
			Matrix rotationMatrix = Matrix(rotationQuaternion.GetMatrix());

			//Création de la translation
			aiVector3D translation;
			calcInterpolatedPosition(translation, i_animationTime, channelIndex, ip_animationInfo);
			Matrix translationMatrix;
			translationMatrix.loadIdentity();
			translationMatrix.translate(translation.x, translation.y, translation.z);


			//Création de la matrice finale

			//On multiplie la matrice du parent avec les transformations
			m_boneList[i]->m_localTransform = translationMatrix * rotationMatrix * scalingMatrix;
		}
	}
	ip_animationInfo->m_lastTime = i_animationTime;
	updateTransforms(mp_skeleton);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void calcInterpolatedPosition(aiVector3D& i_out, float i_animationTime, const aiNodeAnim* ikp_nodeAnim)
///
/// Fonction permettant de calculer l'interpolation entre les keyframes pour une translation
/// 
/// @param[in] i_out : vecteur interpolé
/// @param[in] i_animationTime : temps de l'animation
/// @param[in] ikp_nodeAnim : noeud à animer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::calcInterpolatedPosition(aiVector3D& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	std::vector<aiVectorKey>* positionKeys(&m_animations[ip_animationInfo->m_currentAnimation].m_channels[i_channelIndex].m_positionKeys);
	int rotationKeysSize(positionKeys->size());

	//On vérifie s'il y a seulement une seul frame
	if (rotationKeysSize == 1) 
	{
		i_out = (*positionKeys)[0].mValue;
		return;
	}

	//On trouve la l'index de l'animation et le suivant
	unsigned int positionIndex(findKeyFrameTranslation(i_animationTime, positionKeys, i_channelIndex, ip_animationInfo));
	unsigned int nextPositionIndex(positionIndex + 1);

	assert(nextPositionIndex < rotationKeysSize);

	//On calcule la différence de temps entre les deux keyframes et on calcul le facteur d'avancement entre les deux frames
	float deltaTime((float)((*positionKeys)[nextPositionIndex].mTime - (*positionKeys)[positionIndex].mTime));
	float factor((i_animationTime - (float)(*positionKeys)[positionIndex].mTime) / deltaTime);

	assert(factor >= 0.0f && factor <= 1.0f);

	//On trouve la position des deux keyframes
	const aiVector3D& start = (*positionKeys)[positionIndex].mValue;
	const aiVector3D& end = (*positionKeys)[nextPositionIndex].mValue;

	//On calcule la différence entre les deux vecteurs
	aiVector3D delta = end - start;
	//On calcule la position final du vecteur
	i_out = start + factor * delta;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void calcInterpolatedRotation(aiQuaternion& i_out, float i_animationTime, const aiNodeAnim* ikp_nodeAnim)
///
/// Fonction permettant de calculer l'interpolation entre les keyframes pour une rotation
/// 
/// @param[in] i_out : vecteur interpolé
/// @param[in] i_animationTime : temps de l'animation
/// @param[in] ikp_nodeAnim : noeud à animer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::calcInterpolatedRotation(aiQuaternion& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	std::vector<aiQuatKey>* rotationKeys(&m_animations[ip_animationInfo->m_currentAnimation].m_channels[i_channelIndex].m_rotationKeys);
	int rotationKeysSize(rotationKeys->size());

	//On vérifie s'il y a seulement une seul frame
	if (rotationKeysSize == 1) 
	{
		i_out = (*rotationKeys)[0].mValue;
		return;
	}

	//On trouve la l'index de l'animation et le suivant
	unsigned int rotationIndex(findKeyFrameRotation(i_animationTime, rotationKeys, i_channelIndex, ip_animationInfo));
	unsigned int nextRotationIndex(rotationIndex + 1);

	assert(nextRotationIndex < rotationKeysSize);

	//On calcule la différence de temps entre les deux keyframes et on calcul le facteur d'avancement entre les deux frames
	float deltaTime((float)((*rotationKeys)[nextRotationIndex].mTime - (*rotationKeys)[rotationIndex].mTime));
	float factor((i_animationTime - (float)(*rotationKeys)[rotationIndex].mTime) / deltaTime);

	assert(factor >= 0.0f && factor <= 1.0f);

	//On trouve la position des deux keyframes
	const aiQuaternion& k_startRotationQ = (*rotationKeys)[rotationIndex].mValue;
	const aiQuaternion& k_endRotationQ   = (*rotationKeys)[nextRotationIndex].mValue; 

	//On interpole entre les deux transformations
	aiQuaternion::Interpolate(i_out, k_startRotationQ, k_endRotationQ, factor);
	i_out = i_out.Normalize();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void calcInterpolatedScaling(aiVector3D& i_out, float i_animationTime, const aiNodeAnim* ikp_nodeAnim)
///
/// Fonction permettant de calculer l'interpolation entre les keyframes pour une homothetie
/// 
/// @param[in] i_out : vecteur interpolé
/// @param[in] i_animationTime : temps de l'animation
/// @param[in] ikp_nodeAnim : noeud à animer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Mesh::calcInterpolatedScaling(aiVector3D& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	//On vérifie s'il y a seulement une seul frame
	std::vector<aiVectorKey>* scalingKeys(&m_animations[ip_animationInfo->m_currentAnimation].m_channels[i_channelIndex].m_scalingKeys);
	int scalingKeysSize(scalingKeys->size());
	if (scalingKeysSize == 1) 
	{
		i_out = (*scalingKeys)[0].mValue;
		return;
	}

	//On trouve l'index de l'animation et le suivant
	unsigned int scalingIndex(findKeyFrameTranslation(i_animationTime, scalingKeys, i_channelIndex, ip_animationInfo));
	unsigned int nextScalingIndex(scalingIndex + 1);

	assert(nextScalingIndex < scalingKeysSize);

	//On calcule la différence de temps entre les deux keyframes et on calcul le facteur d'avancement entre les deux frames
	float deltaTime((float)((*scalingKeys)[nextScalingIndex].mTime - (*scalingKeys)[scalingIndex].mTime));
	float factor((i_animationTime - (float)(*scalingKeys)[scalingIndex].mTime) / deltaTime);

	assert(factor >= 0.0f && factor <= 1.0f);

	//On trouve la position des deux keyframes
	const aiVector3D& k_start = (*scalingKeys)[scalingIndex].mValue;
	const aiVector3D& k_end   = (*scalingKeys)[nextScalingIndex].mValue;

	//On calcule la différence entre les deux vecteurs
	aiVector3D delta(k_end - k_start);
	//On calcule la position final du vecteur
	i_out = k_start + factor * delta;
}

unsigned int Mesh::findKeyFrameTranslation(float i_animationTime, const std::vector<aiVectorKey>* i_vectorKey, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	//On vérifie s'il y a des homotheties
	assert(i_vectorKey->size() > 0);

	if(i_channelIndex >= ip_animationInfo->m_lastPositions.size())
		ip_animationInfo->m_lastPositions.resize(m_animations[ip_animationInfo->m_currentAnimation].m_channels.size(), std::make_tuple( 0, 0, 0));

	unsigned int startingframe((i_animationTime >= ip_animationInfo->m_lastTime) ? std::get<0>(ip_animationInfo->m_lastPositions[i_channelIndex]): 0);

	//On itère sur tout les keyframes et on trouve la première qui est plus grande que le temps fournis
	for (unsigned int i(startingframe), vectKeySize(i_vectorKey->size() - 1); i < vectKeySize; ++i) 
		if (i_animationTime < (float)(*i_vectorKey)[i + 1].mTime) 
		{
			std::get<0>(ip_animationInfo->m_lastPositions[i_channelIndex]) = i;
			return i;
		}

	//Si l'on arrive ici, il y a une erreur
	assert(false);
	return 0;
}

unsigned int Mesh::findKeyFrameScaling(float i_animationTime, const std::vector<aiVectorKey>* i_vectorKey, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	//On vérifie s'il y a des homotheties
	assert(i_vectorKey->size() > 0);

	if(i_channelIndex >= ip_animationInfo->m_lastPositions.size())
		ip_animationInfo->m_lastPositions.resize(m_animations[ip_animationInfo->m_currentAnimation].m_channels.size(), std::make_tuple(0, 0, 0));

	unsigned int startingframe((i_animationTime >= ip_animationInfo->m_lastTime) ? std::get<1>(ip_animationInfo->m_lastPositions[i_channelIndex]): 0);

	//On itère sur tout les keyframes et on trouve la première qui est plus grande que le temps fournis
	for (unsigned int i(startingframe), vectKeySize(i_vectorKey->size() - 1); i < vectKeySize; ++i) 
		if (i_animationTime < (float)(*i_vectorKey)[i + 1].mTime) 
		{
			std::get<1>(ip_animationInfo->m_lastPositions[i_channelIndex]) = i;
			return i;
		}

	//Si l'on arrive ici, il y a une erreur
	assert(false);
	return 0;
}


unsigned int Mesh::findKeyFrameRotation(float i_animationTime, const std::vector<aiQuatKey>* i_quatKey, int i_channelIndex, AnimationInfo* ip_animationInfo)
{
	//On vérifie s'il y a des homotheties
	assert(i_quatKey->size() > 0);

	if(i_channelIndex >= ip_animationInfo->m_lastPositions.size())
		ip_animationInfo->m_lastPositions.resize(m_animations[ip_animationInfo->m_currentAnimation].m_channels.size(), std::make_tuple(0, 0, 0));

	unsigned int startingframe((i_animationTime >= ip_animationInfo->m_lastTime) ? std::get<2>(ip_animationInfo->m_lastPositions[i_channelIndex]): 0);

	//On itère sur tout les keyframes et on trouve la première qui est plus grande que le temps fournis
	for (unsigned int i(startingframe), quatKeySize(i_quatKey->size() - 1); i < quatKeySize; ++i) 
		if (i_animationTime < (float)(*i_quatKey)[i + 1].mTime) 
		{
			std::get<2>(ip_animationInfo->m_lastPositions[i_channelIndex]) = i;
			return i;
		}

	//Si l'on arrive ici, il y a une erreur
	assert(false);
	return 0;
}

void Mesh::addAnimation(const std::string& i_path)
{
	std::hash_map<std::string, int>::iterator it_animations(m_animationNameToID.find(i_path));
	if(it_animations == m_animationNameToID.end())
	{
		Assimp::Importer importer;
		const aiScene* mp_scene = importer.ReadFile(i_path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

		//Initialisation du mesh
		if (mp_scene)  
		{
			if(m_animTextureID == -1)
				m_animTextureID = Texture::getNewTextureID();
			m_animated = true;
			m_animations.push_back(Animation(mp_scene->mAnimations[0]));
			m_animationNameToID.insert(std::pair<std::string, int>(i_path, m_animations.size()-1));
		}
		else
			exit(-13);

		bakeAnimation(m_animations.size() - 1);
	}
}


void Mesh::createBoneTree(const aiScene* ip_scene)
{
	// Generate a new texture
	glGenBuffers(1, &m_animMatrixBufferID);
	glGenTextures(1, &m_animMatrixTextureID);
	

	mp_skeleton = createBoneTree( ip_scene->mRootNode, NULL, ip_scene);

	for(int i(0); i < ip_scene->mNumMeshes; ++i)
	{
		for(int j(0); j < ip_scene->mMeshes[i]->mNumBones; ++j)
		{
			if(m_boneNameSet.find(ip_scene->mMeshes[i]->mBones[j]->mName.C_Str()) == m_boneNameSet.end())
			{
				for(std::hash_set<Bone*>::iterator it(m_boneSet.begin()); it != m_boneSet.end(); ++it)
					if((*it)->m_boneName == ip_scene->mMeshes[i]->mBones[j]->mName.C_Str())
					{
						m_boneList.push_back(*it);
						break;
					}
					m_boneNameSet.insert(ip_scene->mMeshes[i]->mBones[j]->mName.C_Str());
			}
		}
	}
}
Bone* Mesh::createBoneTree( aiNode* ip_node, Bone* ip_parent, const aiScene* ip_scene)
{
	// create a node
	Bone* internalNode(new Bone());
	// get the name of the bone
	internalNode->m_boneName = ip_node->mName.data;
	//set the parent, in the case this is theroot node, it will be null
	internalNode->m_parent = ip_parent; 

	// use the name as a key
	internalNode->m_localTransform = Matrix(ip_node->mTransformation);

	for(int i(0); i < ip_scene->mNumMeshes; ++i)
	{
		for(int j(0); j < ip_scene->mMeshes[i]->mNumBones; ++j)
		{
			if(ip_scene->mMeshes[i]->mBones[j]->mName.C_Str() == internalNode->m_boneName)
			{
				internalNode->m_offset = Matrix(ip_scene->mMeshes[i]->mBones[j]->mOffsetMatrix);
				m_boneSet.insert(internalNode);
			}
		}
	}
	//internalNode->m_localTransform.transpose();
	// a copy saved
	internalNode->m_originalLocalTransform = internalNode->m_localTransform;

	calculateBoneToWorldTransform(internalNode);

	// continue for all child nodes and assign the created internal nodes as our children
	for( unsigned int a(0), numChildren(ip_node->mNumChildren); a < numChildren; ++a)
	{
		// recursivly call this function on all children
		internalNode->mp_children.push_back(createBoneTree( ip_node->mChildren[a], internalNode, ip_scene));
	}
	return internalNode;
}

void Mesh::calculateBoneToWorldTransform(Bone* ip_child)
{
	ip_child->m_globalTransform = ip_child->m_localTransform;

	Bone* parent(ip_child->m_parent);
	while( parent )
	{
		// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
		ip_child->m_globalTransform = parent->m_localTransform * ip_child->m_globalTransform;

		// get the parent of the bone we are working on 

		parent  = parent->m_parent;		
	}
	ip_child->m_finalTransform = ip_child->m_globalTransform * ip_child->m_offset;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void Mesh::updateTransforms(Bone* ip_node) 
{
	// update global transform as well
	calculateBoneToWorldTransform( ip_node);
	// continue for all children
	for( std::vector<Bone*>::iterator it(ip_node->mp_children.begin()), childrenEnd(ip_node->mp_children.end()); it != childrenEnd; ++it)
		updateTransforms( *it);
}

void Mesh::calculateBoneToWorldTransformBaked(Bone* ip_child, Animation* ip_animation)
{
	ip_child->m_globalTransform = ip_child->m_localTransform;
	Bone* parent(ip_child->m_parent);

	int parentChannelIndex;
	int currentChannelIndex(ip_animation->m_nameToIndexChannel.find(ip_child->m_boneName)->second);
	std::hash_map<std::string, int>::iterator channelIt;

	while( parent )
	{
		// this will climb the nodes up along through the parents concentating all the matrices to get the Object to World transform, or in this case, the Bone To World transform
		channelIt = ip_animation->m_nameToIndexChannel.find(parent->m_boneName);
		

		if(channelIt != ip_animation->m_nameToIndexChannel.end())
		{
			parentChannelIndex = channelIt->second;

			for(int i(0), translationSize(ip_animation->m_bakedTranslation[parentChannelIndex].size()); i < translationSize; ++i)
				ip_animation->m_bakedTranslation[currentChannelIndex][i] = ip_animation->m_bakedTranslation[parentChannelIndex][i] * ip_animation->m_bakedTranslation[currentChannelIndex][i];

			for(int i(0), rotationSize(ip_animation->m_bakedRotation[parentChannelIndex].size()); i < rotationSize; ++i)
				ip_animation->m_bakedRotation[currentChannelIndex][i] = ip_animation->m_bakedRotation[parentChannelIndex][i] * ip_animation->m_bakedRotation[currentChannelIndex][i];

			for(int i(0), scalingSize(ip_animation->m_bakedScaling[parentChannelIndex].size()); i < scalingSize; ++i)
				ip_animation->m_bakedScaling[currentChannelIndex][i] = ip_animation->m_bakedScaling[parentChannelIndex][i] * ip_animation->m_bakedScaling[currentChannelIndex][i];
		}

		// get the parent of the bone we are working on 

		parent = parent->m_parent;		
	}
	ip_child->m_finalTransform = ip_child->m_globalTransform * ip_child->m_offset;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void Mesh::updateTransformsBaked(Bone* ip_node, Animation* ip_animation) 
{
	// update global transform as well
	calculateBoneToWorldTransformBaked(ip_node, ip_animation);
	// continue for all children
	for( std::vector<Bone*>::iterator it(ip_node->mp_children.begin()), childrenEnd(ip_node->mp_children.end()); it != childrenEnd; ++it)
		updateTransformsBaked( *it, ip_animation);
}

void Mesh::animate(Uint32 i_timestep)
{       

}
void Mesh::updateAdditionnalBuffers()const
{
	if(m_animated == true)
	{
		Shader* p_currentShader(Shader::getShader());

		int numBones(m_boneList.size());

		std::vector<float> bonesTransforms;

		AnimationInfo* p_animationInfo(0);
		for(int i(0), unitListSize(mp_renderInfoList.size()); i < unitListSize; ++i)
		{
			for(int j(0); j < numBones; ++j)
			{
				p_animationInfo = &mp_renderInfoList[i]->m_animationInfo;
				getBakedAnimationsTranspose(p_animationInfo->m_currentAnimation, p_animationInfo->m_currentFrame, j, bonesTransforms);
			}
		}
		glUniform1i(p_currentShader->getUniformVariableSafe("numBones"), numBones);

		glActiveTexture(GL_TEXTURE0 + m_animTextureID);
		glUniform1i(Shader::getShader()->getUniformVariableSafe("AnimMatrices"), m_animTextureID);

		glBindBuffer(GL_TEXTURE_BUFFER, m_animMatrixBufferID);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(bonesTransforms[0]) * bonesTransforms.size(), &bonesTransforms[0], GL_STREAM_DRAW);

		glBindTexture(GL_TEXTURE_BUFFER, m_animMatrixTextureID);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_animMatrixBufferID);
		glBindBuffer(GL_TEXTURE_BUFFER_EXT, 0);
	}
}

void Mesh::getMeshesList(std::vector<std::string>& i_list)
{
	for(std::hash_map<std::string, Mesh*>::iterator it(mp_meshes.begin()); it != mp_meshes.end(); ++it)
		i_list.push_back(it->first);
}

float* const Mesh::getBakedAnimationsTranspose(int i_animationIndex, int i_frameIndex, int i_bonesIndex)
{
	Matrix matrix(m_bakedAnimations[i_animationIndex][i_frameIndex].second[i_bonesIndex]);
	matrix.transpose();
	return matrix.getValues();
}

void Mesh::getBakedAnimationsTranspose(int i_animationIndex, int i_frameIndex, int i_bonesIndex, std::vector<float>& i_value)const
{
	Matrix matrix(m_bakedAnimations[i_animationIndex][i_frameIndex].second[i_bonesIndex]);
	matrix.transpose();
	matrix.getValues(i_value);
}

void Mesh::preTreatementDraw()const
{
	OpenGLHandler::cullFace(m_customisation[CULLFACE]);

	if(m_animated)
	{
		Shader* p_currentShader(Shader::getShader());
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InBoneIDs"));
		glEnableVertexAttribArray(p_currentShader->getAttribVariableSafe("InWeights"));
		glUniform1i(p_currentShader->getUniformVariableSafe("InExecutionType"), ANIMATED_MESH);
	}
}
void Mesh::postTreatementDraw()const
{
	if(m_animated)
	{
		Shader* p_currentShader(Shader::getShader());
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InBoneIDs"));
		glDisableVertexAttribArray(p_currentShader->getAttribVariableSafe("InWeights"));	
	}

}

bool Mesh::loadInfoFromFile()
{
	//On ouvre le fichier
	std::ifstream ifstreamFile;
	ifstreamFile.open(m_path.substr(0, m_path.rfind(".")) + ".info", std::ios::binary);

	//On définie les variables de lecture
	char a_buffer[4];

	//On vérifie si le fichier à pu être ouvert
	if(ifstreamFile.is_open())
	{
		//Tant que l'on ateint pas la fin du fichier
		for(int i(0); i < NB_CUSTOMISATION; ++i)
		{
			if(ifstreamFile.eof())
				return false;
			//On lit une valeur
			ifstreamFile.read (a_buffer, sizeof(int));
			memcpy(&m_customisation[i], a_buffer, sizeof(int));
		}
	}
	else
		return false;
	return true;
}
void Mesh::saveInfoToFile()
{
	//On ouvre le fichier
	std::ofstream ofstreamFile;
	ofstreamFile.open(m_path.substr(0, m_path.rfind(".")) + ".info", std::ios::binary);

	//On créer les variables pour l'écriture
	char a_buffer[4];

	//On écrit le header dans un fichier
	for(int i(0); i < NB_CUSTOMISATION; ++i)
	{
		memcpy(&a_buffer[0], &m_customisation[i], sizeof(int));
		ofstreamFile.write(&a_buffer[0], 4);
	}

	//On ferme le fichier
	ofstreamFile.close();
}

void Mesh::generateBaseCorrection(std::vector<VertexPosition>& i_vertexInformations)
{

	float xMin(INT_MAX), yMin(INT_MAX), zMin(INT_MAX), 
		  xMax(INT_MIN), yMax(INT_MIN), zMax(INT_MIN);
	for(int i(0), positionsSize(i_vertexInformations.size()); i < positionsSize; ++i)
	{
		xMin = min(i_vertexInformations[i].x, xMin);
		yMin = min(i_vertexInformations[i].y, yMin);
		zMin = min(i_vertexInformations[i].z, zMin);

		xMax = max(i_vertexInformations[i].x, xMax);
		yMax = max(i_vertexInformations[i].y, yMax);
		zMax = max(i_vertexInformations[i].z, zMax);
	}
	float scale;
	Vector rotationAxies;
	float rotationAngle;
	Vector translation((xMax - xMin)/2.f, (yMax - yMin)/2.f, (zMax - zMin)/2.f);

	if(m_customisation[UP] == UP_X || m_customisation[UP] == UP_XN)
	{
		rotationAxies.x = 0.f;
		rotationAxies.y = 0.f;
		rotationAxies.z = 1.f;
		scale = 1/(xMax - xMin);
		if(m_customisation[UP] == UP_X)
			rotationAngle = 90.f;
		else
			rotationAngle = -90.f;
	}

	if(m_customisation[UP] == UP_Y || m_customisation[UP] == UP_YN)
	{
		rotationAxies.x = 1.f;
		rotationAxies.y = 0.f;
		rotationAxies.z = 0.f;
		scale = 1/(yMax - yMin);

		if(m_customisation[UP] == UP_Y)
			rotationAngle = 90.f;
		else
			rotationAngle = -90.f;
	}
	if(m_customisation[UP] == UP_Z || m_customisation[UP] == UP_ZN)
	{
		rotationAxies.x = 1.f;
		rotationAxies.y = 0.f;
		rotationAxies.z = 0.f;
		scale = 1/(zMax - zMin);

		if(m_customisation[UP] == UP_Z)
			rotationAngle = 0.f;
		else
			rotationAngle = 180.f;
	}
	m_normalisationMatrix.loadIdentity();
	//m_normalisationMatrix.translate(translation.x, translation.x, translation.z);
	//m_normalisationMatrix.rotate(rotationAngle, rotationAxies.x, rotationAxies.y, rotationAxies.z);
	m_normalisationMatrix.scale(scale);

}