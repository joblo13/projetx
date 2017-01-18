#include "ItemFactory.h"
#include "Wieldable/Weapon.h"
#include "Wieldable/Mac10.h"

ItemFactory* ItemFactory::mp_instance = 0;

ItemFactory::ItemFactory()
{

}
ItemFactory::~ItemFactory()
{

}

Item* ItemFactory::createItem(const ItemInformation* i_itemInformation)
{
	std::hash_map<int, Item*>::iterator it_itemInformation = m_itemList.find(i_itemInformation->ID);
	if(it_itemInformation == m_itemList.end())
		return 0;
	else
		return it_itemInformation->second->getCopy();
}

void ItemFactory::addItem(ItemInformation& i_itemInformation, Item* ip_item)
{
	std::pair<std::hash_map<int, Item*>::iterator, bool> returnedValue = m_itemList.insert(std::pair<int, Item*>(i_itemInformation.ID,ip_item));
	returnedValue.first->second->setItemInformation(new ItemInformation(i_itemInformation));
}