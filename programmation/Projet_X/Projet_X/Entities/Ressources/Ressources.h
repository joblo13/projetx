#ifndef RESSOURCES_H
#define RESSOURCES_H

#include "../../Definition.h"

enum RessourcesType{HUMAN_CUBES, HUMAN_GUN_PARTS, HUMAN_VEHICULE_PARTS, HUMAN_ADVANCED_PARTS};

class Ressources
{
	public:
		Ressources();
		virtual ~Ressources() = 0;

		virtual void addRessources(RessourcesType i_ressourcesType, unsigned int i_ammount) = 0;
		virtual bool removeRessources(RessourcesType i_ressourcesType, unsigned int i_ammount) = 0;
		virtual unsigned int getRessources(RessourcesType i_ressourcesType) = 0;

	private:
		
};


#endif //RESSOURCES_H
