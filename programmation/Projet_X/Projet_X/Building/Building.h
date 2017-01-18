#ifndef BUILDING_H
#define BUILDING_H

#include <string>
#include <vector>
#include <hash_map>
#include <fstream>
#include <algorithm>
#include <hash_set>

#include "../Math/Vector.h"
class BuildingNode;

//Utilis� pour ordonner les cycles en ordre de grandeur
struct CycleSizeComparator
{
    inline bool operator()(const std::vector<std::string>& i_leftParam, const std::vector<std::string>& i_rightParam)
    {
        return getCycleArea(i_leftParam) < getCycleArea(i_rightParam);
    }

private:
	float getCycleArea(const std::vector<std::string>& i_cycles);
};


//Classe repr�sentant un b�timent � construire
class Building
{
public:
    Building(const std::string& ik_filePath);
	bool load();
	~Building();

	//Fonctions utilis�es pour chercer les pi�ces ferm�es
	void findCycles();
	void findNewCycles(std::vector<std::string> i_path);
	bool visited(const std::string& ik_n, const std::vector<std::string>& i_path);
	void normalize(std::vector<std::string>& i_path); //Met les noeuds d'un cycle en ordre
	bool isNew(const std::vector<std::string>& i_path);

	//Retourne la surface couverte par une pi�ce ferm�e
	float getCycleArea(const std::vector<std::string>& i_cycles);
	
	//Retourne vrai si un point est � l'int�rieur d'une pi�ce ferm�e
	bool containsPoint(const std::vector<std::string>& i_cycles, Vector& i_point);
	//Met dans ix_corner1 et ix_corner2 les 2 points extr�mes associ�s au carr� englobant une pi�ce ferm�e
	void getContainingSquare(const std::vector<std::string>& i_cycles, Vector& i_corner1, Vector& i_corner2);

	//bool cycleSizeComparator(const std::vector<std::string>& leftParam, const std::vector<std::string>& rightParam){return getCycleArea(leftParam) < getCycleArea(rightParam); }

	//Parcourt les noeuds associ�s au b�timent et g�n�re toutes les actions � effectuer par l'unit�
	void generateAction(int i_teamID, int i_unitID, const Vector& i_pos);
private:
	//Vrai si le noeud est d�j� associ� � un AddAction lors de la g�n�ration des actions
	bool isAdded(int i_x, int i_z);
	std::string cubeToString2D(int i_x, int i_z);

	//Vector 2D qui contient les emplacements de 1 et des 0 associ� au fichier texte
	std::vector<std::vector<int>> m_shape;
	std::string m_filePath;

	//Contient les liens d'associations entre 2 noeuds 
	std::vector<std::pair<std::string, std::string>> m_nodeLinkList;

	//Contient la liste des noeuds �crit sous la forme "x,y"
	std::vector<BuildingNode*> m_graph;

	//Vecteur contenant tous les cycles trouv�s
	std::vector<std::vector<std::string>> m_cycles;	

	//Hash_ma liant un objet BuildingNode � une cl� de la forme "x,y"
	std::hash_map<std::string, BuildingNode*> m_nodes;

	//hash_set contenant les noeuds qui ont �t� associ�s � un AddAction lors de la g�n�ration des actions
	std::hash_set<std::string> m_nodesAdded;

	//Position r�elle du b�timent
	Vector m_buildingPos;
};

#endif //BUILDING_H
