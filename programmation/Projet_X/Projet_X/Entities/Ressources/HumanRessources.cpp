#include "HumanRessources.h"
#include <assert.h>

HumanRessources::HumanRessources()
{
	for(int i(0); i < 4; ++i)
		ma_ressources[i] = 50;
}

HumanRessources::~HumanRessources()
{

}

void HumanRessources::addRessources(RessourcesType i_ressourcesType, unsigned int i_ammount)
{
	assert(i_ressourcesType >= HUMAN_CUBES && i_ressourcesType <= HUMAN_ADVANCED_PARTS);
	int newRessourcesAmmount(ma_ressources[i_ressourcesType - HUMAN_CUBES] + i_ammount);
	if(newRessourcesAmmount > MAX_RESSOURCES)
		ma_ressources[i_ressourcesType - HUMAN_CUBES] = MAX_RESSOURCES;
	else
		ma_ressources[i_ressourcesType - HUMAN_CUBES] = newRessourcesAmmount;
}

bool HumanRessources::removeRessources(RessourcesType i_ressourcesType, unsigned int i_ammount)
{
	assert(i_ressourcesType >= HUMAN_CUBES && i_ressourcesType <= HUMAN_ADVANCED_PARTS);
	int newRessourcesAmmount(ma_ressources[i_ressourcesType - HUMAN_CUBES] - i_ammount);
	if(newRessourcesAmmount < 0)
		return false;

	ma_ressources[i_ressourcesType - HUMAN_CUBES] = newRessourcesAmmount;
	return true;
}

unsigned int HumanRessources::getRessources(RessourcesType i_ressourcesType)
{
	assert(i_ressourcesType >= HUMAN_CUBES && i_ressourcesType <= HUMAN_ADVANCED_PARTS);

	return ma_ressources[i_ressourcesType - HUMAN_CUBES];
}