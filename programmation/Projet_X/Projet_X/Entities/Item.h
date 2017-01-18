#ifndef ITEM_H
#define ITEM_H

struct ItemInformation;
class Unit;

class Item
{
	public:
		Item();
		Item(const Item& i_item);
		virtual ~Item() = 0;

		virtual Item* getCopy() = 0;

		const ItemInformation* getItemInformation();
		void setItemInformation(ItemInformation* ip_itemInformation);

		virtual void setUnit(Unit* ip_unit);

	protected:
		ItemInformation* mp_itemInformation;
		Unit* mp_unit;
};

#endif //ITEM_H
