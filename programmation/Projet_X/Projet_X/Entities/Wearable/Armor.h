#ifndef ARMOR_H
#define ARMOR_H

#include "../Item.h"

class Armor:public Item
{
	public:
		Armor();
		Armor(const Armor& i_weapon);
		virtual ~Armor() = 0;
	protected:
};

#endif //ARMOR_H
