#ifndef HUMAN_RESSOURCES_H
#define HUMAN_RESSOURCES_H

#include "Ressources.h"

class HumanRessources : public Ressources
{
	public:
		HumanRessources();
		~HumanRessources();
		virtual void addRessources(RessourcesType i_ressourcesType, unsigned int i_ammount);
		virtual bool removeRessources(RessourcesType i_ressourcesType, unsigned int i_ammount);
		virtual unsigned int getRessources(RessourcesType i_ressourcesType);
	private:
		unsigned int ma_ressources[4];	
};


#endif //HUMAN_RESSOURCES_H
