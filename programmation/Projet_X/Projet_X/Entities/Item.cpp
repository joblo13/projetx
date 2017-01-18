#include "Item.h"
#include "Unit.h"

Item::Item()
:mp_unit(0), mp_itemInformation(0)
{

}

Item::Item(const Item& i_item)
:mp_itemInformation(i_item.mp_itemInformation), mp_unit(i_item.mp_unit)
{

}

Item::~Item()
{
}

const ItemInformation* Item::getItemInformation()
{
	return mp_itemInformation;
}
void Item::setItemInformation(ItemInformation* ip_itemInformation)
{
	mp_itemInformation = ip_itemInformation;
}

void Item::setUnit(Unit* ip_unit)
{
	mp_unit = ip_unit;
}
