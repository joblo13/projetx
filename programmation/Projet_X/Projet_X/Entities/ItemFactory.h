#ifndef ITEM_FACTORY_H
#define ITEM_FACTORY_H

#include <hash_map>

#include "Item.h"
#include "ItemInformation.h"


class ItemFactory
{
	public:

		//Singleton
		static ItemFactory* getInstance()
		{
			if(mp_instance == 0)
			{
				mp_instance = new ItemFactory();
			}
			return mp_instance;
		}

		static void releaseInstance()
		{
			delete mp_instance;
			mp_instance = 0;
		}

		Item* createItem(const ItemInformation* i_itemInformation);
		void addItem(ItemInformation& i_itemInformation, Item* ip_item);


	private:
		ItemFactory();
		~ItemFactory();

		std::hash_map<int, Item*> m_itemList;

		//Instance du singleton
		static ItemFactory* mp_instance;
};

#endif //ITEM_FACTORY_H
