#include "Building.h"
#include "BuildingNode.h"

#include "../Terrain/Terrain.h"
#include "../Entities/UnitHandler.h"
#include "BuildingNode.h"	
#include "../Entities/Unit.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Building(const std::string& ik_filePath)
///
/// Constructeur
/// 
/// @param[in] ik_filePath : chemin vers le fichier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Building::Building(const std::string& ik_filePath)
:m_filePath(ik_filePath)
{
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("1", "2"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("1", "3"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("1", "4"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("2", "3"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("3", "4"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("2", "6"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("4", "6"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("7", "8"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("8", "9"));
	m_nodeLinkList.push_back(std::pair<std::string, std::string>("9", "7"));

}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Building()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Building::~Building()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool load()
///
/// Chargement du bâtiment
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Building::load()
{
	std::ifstream x_ifstreamFile(m_filePath);


	if(x_ifstreamFile)
	{		
		int n_j(0);
		unsigned int n_maxLineNumber(0);
		char c_buffer;
		m_shape.push_back(std::vector<int>());
		x_ifstreamFile.read(&c_buffer, 1);

		while(!x_ifstreamFile.eof())
		{
			if(c_buffer == '\n')
			{
				++n_j;
				m_shape.push_back(std::vector<int>());
			}
			else
			{
				if(c_buffer == ' ')
					m_shape[n_j].push_back(0);
				else
					m_shape[n_j].push_back(atoi(&c_buffer));
				if(n_maxLineNumber < m_shape[n_j].size())
					n_maxLineNumber = m_shape[n_j].size();
			}
			x_ifstreamFile.read(&c_buffer, 1);
		}
		for(int n_i(0), n_shapeSize(m_shape.size()); n_i < n_shapeSize; ++n_i)
			m_shape[n_i].resize(n_maxLineNumber);
	}
	else
		return false;

	//Construction du graphe
	//On cherche les différents liens entre les noeuds. On associe les noeuds avec les noeuds adjacents en bas ou à droite.
	//Pour les diagonales, on fait un lien seulement s'il n'y a pas d'Autres noeuds sur les 2 positions adjacents à la position diagonale.
	std::hash_map<std::string, BuildingNode*> x_nodes;
	std::vector<std::pair<std::string,std::string>> xvect_links;
	for(unsigned int i(0), shapeSize(m_shape.size()); i < shapeSize; ++i)
	{
		for(unsigned int j(0); j < m_shape[i].size(); ++j)
		{
			if(m_shape[i][j] >= 1)
			{
				BuildingNode* x_node = new BuildingNode(Vector((float)i, 0.0f, (float)j));

				char c_buffer[20];
				std::string str_result = "";

				sprintf_s(c_buffer, "%i,%i", i, j);
				str_result = c_buffer;

				bool b_downOrRightNodeAdded = false;

				//On regarde s'il y a un point à la droite
				if(i + 1 < m_shape.size())
				{
					if(m_shape[i + 1][j] >=1)
					{
						//On ajoute le point à droite.
						sprintf_s(c_buffer, "%i,%i", i+1, j);
						std::string str_result2 = c_buffer;
						xvect_links.push_back( std::pair<std::string,std::string>(str_result, str_result2));
						b_downOrRightNodeAdded = true;
					}
					//S'il y en a pas à droite, on regarde en haut.
					else if(j != 0 && m_shape[i][j - 1] < 1)
					{
						//S'il y en a pas en haut, on regarde la diagonale en haut à droite.
						if( m_shape[i + 1][j - 1] >=1 )
						{
							//On ajoute la diagonale en haut à droite.
							sprintf_s(c_buffer, "%i,%i", i+1, j-1);
							std::string str_result2 = c_buffer;
							xvect_links.push_back( std::pair<std::string,std::string>(str_result, str_result2));
						}
					}
				}
				//On regarde s'il y a un point en bas.
				if(j + 1 < m_shape[i].size()) 
				{
					if(m_shape[i][j + 1] >=1)
					{
						//On ajoute le point en bas.
						sprintf_s(c_buffer, "%i,%i", i, j + 1);
						std::string str_result2 = c_buffer;
						xvect_links.push_back( std::pair<std::string,std::string>(str_result, str_result2));
						b_downOrRightNodeAdded = true;
					}
					//S'il y en a pas en bas, on regarde à gauche.
					else if(i != 0 && m_shape[i - 1][j] < 1)
					{
						//S'il y en a pas à gauche, on regarde la diagonale en bas à gauche.
						if( m_shape[i - 1][j + 1] >=1 )
						{
							//On ajoute la diagonale en bas à gauche.
							sprintf_s(c_buffer, "%i,%i", i-1, j+1);
							std::string str_result2 = c_buffer;
							xvect_links.push_back( std::pair<std::string,std::string>(str_result, str_result2));
						}
					}
				}

				//Finalement, on regarde la diagonale en bas à droite, seulement s'il n'y a pas de point en bas ou à droite.
				if(!b_downOrRightNodeAdded && i + 1 < m_shape.size() && j + 1 < m_shape[i].size() && m_shape[i + 1][j + 1] >=1)
				{
					//On ajoute la diagonale en bas à droite.
					sprintf_s(c_buffer, "%i,%i", i+1, j+1);
					std::string str_result2 = c_buffer;
					xvect_links.push_back( std::pair<std::string,std::string>(str_result, str_result2));
				}

				x_nodes[str_result] =  x_node;

				//Ajout du noeud dans le graphe
				m_graph.push_back(x_node);
			}
		}
	}

	m_nodes = x_nodes;
	m_nodeLinkList = xvect_links;

	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float getCycleArea(const std::vector<std::string>& ik_cycle)
///
/// Fonction permettant de trouver une air approximative d'un cycle
/// 
/// @param[in] ik_cycle : cycle
///
/// @return Aire trouvé
///
////////////////////////////////////////////////////////////////////////
float CycleSizeComparator::getCycleArea(const std::vector<std::string>& ik_cycle)
{

	float cycleArea = 0;

	//On fait le tour du cycle et pour chaque lien, on additionne à l'aire la valeur x0*y1 - x1*y0
	for(unsigned int i(0), cycleSize(ik_cycle.size()); i < cycleSize; ++i)
	{
		int comma1 = ik_cycle[i].find(",");
		int j = (i + 1) % ik_cycle.size();
		int comma2 = ik_cycle[j].find(",");
		float pt1x = (float)atof(ik_cycle[i].substr(0, comma1).c_str());
		float pt1y = (float)atof(ik_cycle[i].substr(comma1 + 1, ik_cycle[i].length() - comma1 - 1).c_str());
		float pt2x = (float)atof(ik_cycle[j].substr(0, comma2).c_str());
		float pt2y = (float)atof(ik_cycle[j].substr(comma2 + 1, ik_cycle[j].length() - comma2 - 1).c_str());
		cycleArea += pt1x * pt2y - pt2x * pt1y;
	}

	return abs(cycleArea) / 2;
}


////////////////////////////////////////////////////////////////////////
///
/// @fn void findCycles()
///
/// Fonction permettant de trouver tout les cycles dans la structure
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Building::findCycles()
{
	//On Cherche des cycles à partir de chaque noeud possible
	std::vector<std::string> vector;
	for (unsigned int i(0), graphSize(m_graph.size()); i < graphSize; i++)
	{
		vector.push_back(m_graph[i]->toString());
		findNewCycles(vector);
		vector.pop_back();
	}
	std::sort(m_cycles.begin(), m_cycles.end(), CycleSizeComparator());
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Camera(const Vector& ik_position, const Vector& ik_target)
///
/// Fonction récursive permettant de trouver un cycle
/// 
/// @param[in] i_path : liste de chemin visité
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Building::findNewCycles(std::vector<std::string> i_path)
{
   std::string n(i_path[0]);

    std::string x;
    std::vector<std::string> sub;
    for (int i = 0, n_nodeLinkListSize(m_nodeLinkList.size()); i < n_nodeLinkListSize; ++i)
        for (int y = 0; y < 2; ++y)
		{
			if(y == 0)
				x = m_nodeLinkList[i].first;				
			else
				x = m_nodeLinkList[i].second;

            if (x == n)
            //Le lien fait référence au noeud courant
            {
				if(y == 0)
					x = m_nodeLinkList[i].second;	
				else
					x = m_nodeLinkList[i].first;

                if (!visited(x, i_path))
                //Le noeud voisin n'a pas encore été ajouté du path
                {
					std::vector<std::string> subEmpty;
					sub.swap(subEmpty);

					sub.push_back(x);

					sub.insert(sub.end(), i_path.begin(), i_path.end());

                    //On explore le nouveau path

                    findNewCycles(sub);
                }
                else if (i_path.size() > 2 && (x == i_path[i_path.size() - 1]))
                //  cycle trouvé
                {
					std::vector<std::string> normalizedPath = i_path;
					normalize(normalizedPath);

					if(isNew(normalizedPath))
					{
						m_cycles.push_back(i_path);
						for(unsigned int i(0); i < i_path.size(); ++i)
						{
							m_nodes[i_path[i]]->setIsInCycle(true);
						}
					}
                }
            }
		}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool visited(const std::string& ik_n, const std::vector<std::string>& ik_path)
///
/// Fonction permettant de savoir si un chemin à déjà été visité
/// 
/// @param[in] ik_n : hash d'un chemin
/// @param[in] ik_path : liste des chemin visité
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Building::visited(const std::string& ik_n, const std::vector<std::string>& ik_path)
{
	bool ret = false;

	for(int i(0), pathSize(ik_path.size()); i < pathSize; ++i)
		if (ik_path[i] == ik_n)
		{
			ret = true;
			break;
		}

	return ret;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void normalize(std::vector<std::string>& i_path)
///
/// Fonction permettant de normaliser la structure pour les comparaisons
/// 
/// @param[in] i_path : chemin visité
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Building::normalize(std::vector<std::string>& i_path)
{
	std::sort(i_path.begin(), i_path.end());
	for(int n_i(i_path.size()-1); n_i > 0; --n_i)
		if(i_path[n_i] == i_path[n_i - 1])
		{
			i_path[n_i] = i_path[i_path.size()-1];
			i_path.pop_back();
		}
	std::sort(i_path.begin(), i_path.end());
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool isNew(const std::vector<std::string>& ik_path)
///
/// Fonction permetttant de vérifie si un chemin des nouveau
/// 
/// @param[in] ik_path : liste des chemins visités
///
/// @return Vrai si le chemin est nouveau
///
////////////////////////////////////////////////////////////////////////
bool Building::isNew(const std::vector<std::string>& ik_path)
{
	bool result = true;
	for(int n_i(0), n_cyclesSize(m_cycles.size()); n_i < n_cyclesSize; ++n_i)
	{
		if(m_cycles[n_i].size() == ik_path.size())
		{
			for(int n_j(0), n_pathSize(ik_path.size()); n_j < n_pathSize; ++n_j)
			{
				std::vector<std::string> vectstr_normCycle = m_cycles[n_i];
				normalize(vectstr_normCycle);

				if(vectstr_normCycle[n_j] == ik_path[n_j])
				{
					result = false;
				}
				else
				{
					result = true;
					break;
				}

			}
			if(!result)
				return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn std::string cubeToString2D(int i_x, int i_z)
///
/// Fonction transformant une position 2D de cube en hash
/// 
/// @param[in] i_x : position en x
/// @param[in] i_z : position en z
///
/// @return Hash créé
///
////////////////////////////////////////////////////////////////////////
std::string Building::cubeToString2D(int i_x, int i_z)
{
	char a_text[128];
	sprintf_s(a_text, "%i,%i", i_x, i_z);
	return std::string(a_text);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool isAdded(int i_x, int i_z)
///
/// Fonction vérifiant si un cube à déjà été ajouté
/// 
/// @param[in] i_x : position en x
/// @param[in] i_z : position en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Building::isAdded(int i_x, int i_z)
{
	char a_text[128];
	sprintf_s(a_text, "%i,%i", i_x, i_z);

	return m_nodesAdded.find(a_text) != m_nodesAdded.end();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void generateAction(int i_teamID, int i_unitID, const Vector& ik_pos)
///
/// Fonction générant les actions pour la construction de la maison
/// 
/// @param[in] i_teamID : équipe de l'unité
/// @param[in] i_unitID : index de l'unité
/// @param[in] ik_pos : position du centre inférieur de la maison
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Building::generateAction(int i_teamID, int i_unitID, const Vector& ik_pos)
{
	m_buildingPos = ik_pos;

	Terrain* p_terrain(Terrain::getInstance());
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	Vector position(0,0,0);
	Vector constructorPos;
	//On itere sur les cycles en partant de celui ayant la plus petite surface. Ils ont été ordonnés auparavant.
	for(int i(0), cyclesSize(m_cycles.size()); i < cyclesSize; ++i)
	{
		//Ajout des points contenus dans une pièce fermée
		for(int j(0), graphSize(m_graph.size()); j < graphSize; ++j)
		{
			if(m_graph[j] != 0 && !m_graph[j]->getIsInCycle())
			{
				position = m_graph[j]->getCubePosition() + ik_pos;
				if(!isAdded(position.x, position.z))
				{
					if( containsPoint(m_cycles[i], position ) )
					{
						//On test sur les 4 positions autour du noeud pour voir où l'unité peut se déplacer.
						if(p_terrain->isWalkable(position.x + 1, position.y, position.z) && !isAdded(position.x + 1, position.z))
						{
							constructorPos = Terrain::cubeToPosition(Vector3I(position.x + 1, position.y, position.z));
						}
						else if(p_terrain->isWalkable(position.x - 1, position.y, position.z) && !isAdded(position.x - 1, position.z))
						{
							constructorPos = Terrain::cubeToPosition(Vector3I(position.x - 1, position.y, position.z));
						}
						else if(p_terrain->isWalkable(position.x, position.y, position.z + 1) && !isAdded(position.x, position.z + 1))
						{
							constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z + 1));
						}
						else if(p_terrain->isWalkable(position.x, position.y, position.z - 1) && !isAdded(position.x, position.z - 1))
						{
							constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z - 1));
						}

						//Déplacement de l'unité
						p_unitHandler->moveAStarAction(i_teamID, i_unitID, Terrain::positionToCube(constructorPos));
						//Ajout des 2 noeuds pour faire un mur
						p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y, position.z));
						p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y + 1, position.z));
						m_nodesAdded.insert( cubeToString2D(position.x, position.z) );
						m_graph[j] = 0;
					}
				}
			}
		}
		//Ajout des points du cycle
		for(int k(0), n_cycleSize(m_cycles[i].size()); k < n_cycleSize; ++k)
		{
			position = m_nodes[m_cycles[i][k]]->getCubePosition() + ik_pos;
			if(!isAdded(position.x, position.z))
			{
				//Test les 4 positions autour du noeuds pour voir où l'unité peut aller.
				//Dans ce cas, l'unité ne doit pas aller à l'intérieur du cycle, sinon, il va rester pris à l'intérieur.
				if(p_terrain->isWalkable(position.x + 1, position.y, position.z) &&
					!isAdded(position.x + 1, position.z)
					&& !containsPoint(m_cycles[i], Vector(position.x + 1, position.y, position.z)))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x + 1, position.y, position.z));
				}
				else if(p_terrain->isWalkable(position.x - 1, position.y, position.z) &&
					!isAdded(position.x - 1, position.z)
					&& !containsPoint(m_cycles[i], Vector(position.x - 1, position.y, position.z)))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x - 1, position.y, position.z));
				}
				else if(p_terrain->isWalkable(position.x, position.y, position.z + 1) &&
					!isAdded(position.x, position.z + 1)
					&& !containsPoint(m_cycles[i], Vector(position.x, position.y, position.z + 1)))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z + 1));
				}
				else if(p_terrain->isWalkable(position.x, position.y, position.z - 1) &&
					!isAdded(position.x, position.z - 1)
					&& !containsPoint(m_cycles[i], Vector(position.x, position.y, position.z - 1)))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z - 1));
				}

				//Deplacement
				p_unitHandler->moveAStarAction(i_teamID, i_unitID, Terrain::positionToCube(constructorPos));
				//Ajout des 2 noeuds pour faire un mur
				p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y, position.z));
				p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y + 1, position.z));
				m_nodesAdded.insert( cubeToString2D(position.x, position.z) );
			}

		}
	}
	//On itère sur les points restants qui ne sont pas à l'intérieur ou faisant partie d'un cycle
	for(int i(0), m_graphSize(m_graph.size()); i < m_graphSize; ++i)
	{
		if(m_graph[i] != 0)
		{
			position = m_graph[i]->getCubePosition() + ik_pos;
			if(!isAdded(position.x, position.z))
			{
				//Test des 4 positions autour du noeud pour savoir où se déplacer
				if(p_terrain->isWalkable(position.x + 1, position.y, position.z) && !isAdded(position.x + 1, position.z))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x + 1, position.y, position.z));
				}
				else if(p_terrain->isWalkable(position.x - 1, position.y, position.z) && !isAdded(position.x - 1, position.z))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x - 1, position.y, position.z));
				}
				else if(p_terrain->isWalkable(position.x, position.y, position.z + 1) && !isAdded(position.x, position.z + 1))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z + 1));
				}
				else if(p_terrain->isWalkable(position.x, position.y, position.z - 1) && !isAdded(position.x, position.z - 1))
				{
					constructorPos = Terrain::cubeToPosition(Vector3I(position.x, position.y, position.z - 1));
				}

				//Déplacement
				p_unitHandler->moveAStarAction(i_teamID, i_unitID, Terrain::positionToCube(constructorPos));
				//Ajout des 2 noeuds pour faire un mur
				p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y, position.z));
				p_unitHandler->addAction(i_teamID, i_unitID, Vector3I(position.x, position.y + 1, position.z));
				m_nodesAdded.insert( cubeToString2D(position.x, position.z) );
				m_graph[i] = 0;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getContainingSquare(const std::vector<std::string>& ik_cycle, Vector& i_corner1, Vector& i_corner2)
///
/// Fonction permettant de trouver un carré englobant le cycle
/// 
/// @param[in] ik_cycle : cycle
/// @param[in] i_corner1 : premier coin du carré
/// @param[in] i_corner2 : deuxièmement coin du carré
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Building::getContainingSquare(const std::vector<std::string>& ik_cycle, Vector& i_corner1, Vector& i_corner2)
{
	Vector vector1((float)INT_MAX, 0.0f, (float)INT_MAX), x_vector2((float)INT_MIN, 0.0f, (float)INT_MIN);
	//On trouve les points extremes du cycle pour trouver le carré englobant.
	for( int i(0), cyclesSize(ik_cycle.size()); i < cyclesSize; ++i)
	{
		BuildingNode* p_node = m_nodes[ik_cycle[i]];

		vector1.x = vector1.x > p_node->getCubePosition().x?
			p_node->getCubePosition().x: vector1.x;
		vector1.z = vector1.z > p_node->getCubePosition().z?
			p_node->getCubePosition().z: vector1.z;
		x_vector2.x = x_vector2.x < p_node->getCubePosition().x?
			p_node->getCubePosition().x: x_vector2.x;
		x_vector2.z = x_vector2.z < p_node->getCubePosition().z?
			p_node->getCubePosition().z: x_vector2.z;
	}
	i_corner1 = vector1;
	i_corner2 = x_vector2;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool containsPoint(const std::vector<std::string>& ik_cycles, Vector& i_point)
///
/// Fonction permettant de savoir si des points sont compris dans un cycle
/// 
/// @param[in] ik_cycle : cycle
/// @param[in] i_point : point
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Building::containsPoint(const std::vector<std::string>& ik_cycle, Vector& i_point)
{
	Vector vector1, vector2;
	//On trouve les points extremes du cycle pour trouver le carré englobant.

	Vector point = i_point - m_buildingPos;

	getContainingSquare(ik_cycle, vector1, vector2);

	//On aligne un point d'extrémité du carré en x.
	Vector checkpoint = vector1;
	checkpoint.x = point.x;
	--checkpoint.z;

	int nbLineCollision = 0;

	//On regarde sur chaque point en z pour trouver les croisements avec les lignes du cycle.
	while( checkpoint.z != point.z )
	{
		++checkpoint.z;
		for(int i(0), cycleSize(ik_cycle.size()); i < cycleSize; ++i)
		{
			if(		checkpoint.x == m_nodes[ik_cycle[i]]->getCubePosition().x
				&&
					checkpoint.z == m_nodes[ik_cycle[i]]->getCubePosition().z)
				++nbLineCollision;
		}
	}

	if(nbLineCollision % 2 == 1)
		//Le point est dans le cycle.
		return true;
	else
		//Le point n'est pas dans le cycle.
		return false;
}