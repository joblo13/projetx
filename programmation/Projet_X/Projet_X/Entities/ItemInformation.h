#ifndef ITEM_INFORMATION_H
#define ITEM_INFORMATION_H

#include <string.h>

enum ItemType{BACKPACK_ITEMS, HEAD_ITEMS, TORSO_ITEMS, LEGS_ITEMS, 
			WEAPON0_ITEMS, WEAPON1_ITEMS, WEAPON2_ITEMS, WEAPON3_ITEMS, WEAPON4_ITEMS,
			WEAPON5_ITEMS, WEAPON6_ITEMS, WEAPON7_ITEMS, WEAPON8_ITEMS, WEAPON9_ITEMS, ITEMLIST_SIZE, INVALID_ITEMS};

#define NUM_ARMOR 4

struct ItemInformation
{
	
	ItemInformation():name("undefined"), information(""), ID(-1), weight(0){}
	std::string name;
	std::string information;
	int price[4];
	int ID;
	int weight;
};

#endif //ITEM_INFORMATION_H