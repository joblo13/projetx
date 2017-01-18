#include "ShopGUI.h"

#include "../Terrain/Terrain.h"
#include "../Entities/UnitHandler.h"
#include "GUIHandler.h"
#include "../Camera/Camera.h"
#include "../Entities/ItemFactory.h"
#include "../Entities/Wieldable/Mac10.h"
#include "../Entities/Wieldable/ConstructorWeapon.h"
#include "../Entities/Wearable/KevlarHelmet.h"
#include "../Rendering/Texture.h"
 
#include "../Entities/Unit.h"

#define MAX_CHAT_ENTRIES 100
#define CHAR_PIXEL_LENGTH 7

ShopGUI::ShopGUI()
:m_currentSelectedItemID(-1), m_currentItemType(INVALID_ITEMS), mp_unit(0)
{
	m_onTop = true;
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "WeaponShop.layout", "layouts");
	setVisible(false);

	init();
}

ShopGUI::~ShopGUI()
{

}
int getTroncateItemType(ItemType i_itemType)
{
	if(i_itemType < WEAPON0_ITEMS)
		return i_itemType;
	else
		return WEAPON0_ITEMS;
}
void ShopGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	mp_unit = p_unitHandler->getUnit(i_teamID, ip_camera->getUnitID());

	int cost;
	int currentRessources;

	bool isBuyable(true);

	if(m_currentSelectedItemID == -1)
		cost = 0;
	else
		cost = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_CUBES];

	currentRessources = p_unitHandler->getRessources(i_teamID, HUMAN_CUBES);

	m_layout->getChild("WeaponShop/information/informationList_label/cost_label/res_cube")->setText(
		convertToString(currentRessources) + "/" + convertToString(cost));

	isBuyable &= currentRessources >= cost;
	

	if(m_currentSelectedItemID == -1)
		cost = 0;
	else
		cost = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_GUN_PARTS];

	currentRessources = p_unitHandler->getRessources(i_teamID, HUMAN_GUN_PARTS);

	m_layout->getChild("WeaponShop/information/informationList_label/cost_label/res_weaponPart")->setText(
		convertToString(currentRessources) + "/" + convertToString(cost));

	isBuyable &= currentRessources >= cost;


	if(m_currentSelectedItemID == -1)
		cost = 0;
	else
		cost = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_VEHICULE_PARTS];

	currentRessources = p_unitHandler->getRessources(i_teamID, HUMAN_VEHICULE_PARTS);

	m_layout->getChild("WeaponShop/information/informationList_label/cost_label/res_vehiculePart")->setText(
		convertToString(currentRessources) + "/" + convertToString(cost));

	isBuyable &= currentRessources >= cost;

	if(m_currentSelectedItemID == -1)
		cost = 0;
	else
		cost = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_ADVANCED_PARTS];

	currentRessources = p_unitHandler->getRessources(i_teamID, HUMAN_ADVANCED_PARTS);

	m_layout->getChild("WeaponShop/information/informationList_label/cost_label/res_advancedPart")->setText( 
		convertToString(currentRessources) + "/" + convertToString(cost));

	isBuyable &= currentRessources >= cost;
	isBuyable &= m_currentSelectedItemID != -1;

	if(m_currentItemType != INVALID_ITEMS)
	{
		const ItemInformation* p_item(mp_unit->getItem(m_currentItemType));

		if(p_item != 0)
			isBuyable &= m_currentSelectedItemID != p_item->ID;
	}


	if(isBuyable)
		m_layout->getChild("WeaponShop/information/buy_but")->enable();
	else
		m_layout->getChild("WeaponShop/information/buy_but")->disable();
}

void ShopGUI::registerHandlers()
{
	m_layout->getChild("WeaponShop/information/buy_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::buyItem, this));

	m_layout->getChild("WeaponShop/gear/gearTree/backpack_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectBackpackHandler, this));

    m_layout->getChild("WeaponShop/gear/gearTree/head_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectHeadHandler,this));

	m_layout->getChild("WeaponShop/gear/gearTree/torso_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectTorsoHandler, this));

    m_layout->getChild("WeaponShop/gear/gearTree/legs_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectLegsHandler,this));

	m_layout->getChild("WeaponShop/information/itemList")->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
                        CEGUI::Event::Subscriber(&ShopGUI::selectionChanged,this));


	m_layout->getChild("WeaponShop/gear/weapons_label/weapon0_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectWeapon0Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon1_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectWeapon1Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon2_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectWeapon2Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon3_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ShopGUI::selectWeapon3Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon4_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon4Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon5_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon5Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon6_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon6Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon7_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon7Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon8_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon8Handler,this));

	m_layout->getChild("WeaponShop/gear/weapons_label/weapon9_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&ShopGUI::selectWeapon9Handler,this));
}

bool ShopGUI::selectWeapon0Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON0_ITEMS);
}
bool ShopGUI::selectWeapon1Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON1_ITEMS);
}
bool ShopGUI::selectWeapon2Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON2_ITEMS);
}
bool ShopGUI::selectWeapon3Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON3_ITEMS);
}
bool ShopGUI::selectWeapon4Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON4_ITEMS);
}
bool ShopGUI::selectWeapon5Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON5_ITEMS);
}
bool ShopGUI::selectWeapon6Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON6_ITEMS);
}
bool ShopGUI::selectWeapon7Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON7_ITEMS);
}
bool ShopGUI::selectWeapon8Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON8_ITEMS);
}
bool ShopGUI::selectWeapon9Handler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(WEAPON9_ITEMS);
}

bool ShopGUI::loadItemChoice(ItemType i_itemType)
{
	const ItemInformation* currentItem(mp_unit->getItem(i_itemType));

	if(currentItem == 0)
		m_currentSelectedItemID = -1;
	else
		m_currentSelectedItemID = currentItem->ID;

	m_currentItemType = i_itemType;

	updateItemInfo();

	return true;
}

void ShopGUI::updateItemInfo()
{
	CEGUI::Listbox* p_listBox((CEGUI::Listbox*)m_layout->getChild("WeaponShop/information/itemList"));
	p_listBox->resetList();
	CEGUI::ListboxTextItem* listBoxTextItem;

	if(m_currentSelectedItemID == -1)
		m_layout->getChild("WeaponShop/information/informationList_label")->setText("");
	else
		m_layout->getChild("WeaponShop/information/informationList_label")->setText(m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].information);

	int totalWeight(0);
	if(mp_unit != 0)
	{
		for(unsigned int i(0); i < ITEMLIST_SIZE; ++i)
		{
			const ItemInformation* p_item(mp_unit->getItem((ItemType)i));
			if(p_item != 0)
				totalWeight += p_item->weight;
		}
	}
	m_layout->getChild("WeaponShop/gear/weight_label")->setText("Weight: " + convertToString(totalWeight));

	if(m_currentItemType != INVALID_ITEMS)
	{
		int unitItemID;
		{
			const ItemInformation* itemInformation = mp_unit->getItem(m_currentItemType);
			if(itemInformation == 0)
				unitItemID = -1;
			else
				unitItemID = itemInformation->ID;
		}

		//TODO: enlever Wepon0 a unitItemID cest cest un weapon ou partir les weapon id a zero aussi
		for(int i(0), itemSize(m_itemList[getTroncateItemType(m_currentItemType)].size()); i < itemSize; ++i)
		{
			listBoxTextItem = new CEGUI::ListboxTextItem(m_itemList[getTroncateItemType(m_currentItemType)][i].name);
			listBoxTextItem->setID(i);
			listBoxTextItem->setSelectionBrushImage("FPS/listBox");

			if(i == m_currentSelectedItemID)
				listBoxTextItem->setSelected(true);

			if(i == unitItemID)
				listBoxTextItem->setTextColours(CEGUI::Colour(0,1,0));

			p_listBox->addItem(listBoxTextItem);
		}
	}
}

bool ShopGUI::selectBackpackHandler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(BACKPACK_ITEMS);
}

bool ShopGUI::selectHeadHandler(const CEGUI::EventArgs& i_event)
{ 
    return loadItemChoice(HEAD_ITEMS);
}

bool ShopGUI::selectTorsoHandler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(TORSO_ITEMS);
}

bool ShopGUI::selectLegsHandler(const CEGUI::EventArgs& i_event)
{
    return loadItemChoice(LEGS_ITEMS);
}

bool ShopGUI::selectionChanged(const CEGUI::EventArgs& i_event)
{
	CEGUI::ListboxItem* p_listBoxItem(((CEGUI::Listbox*)m_layout->getChild("WeaponShop/information/itemList"))->getFirstSelectedItem());

	if(p_listBoxItem != 0)
	{
		m_currentSelectedItemID = p_listBoxItem->getID();
	}
	else
		m_currentSelectedItemID = -1;

	updateItemInfo();

	return true;
}

bool ShopGUI::buyItem(const CEGUI::EventArgs& i_event)
{
	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	int cost[4];
	int currentRessources[4];


	//On test si l'on a assez de ressources
	if(m_currentSelectedItemID == -1)
	{
		cost[HUMAN_CUBES] = 0;
		cost[HUMAN_GUN_PARTS] = 0;
		cost[HUMAN_VEHICULE_PARTS] = 0;
		cost[HUMAN_ADVANCED_PARTS] = 0;
	}
	else
	{
		cost[HUMAN_CUBES] = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_CUBES];
		cost[HUMAN_GUN_PARTS] = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_GUN_PARTS];
		cost[HUMAN_VEHICULE_PARTS] = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_VEHICULE_PARTS];
		cost[HUMAN_ADVANCED_PARTS] = m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID].price[HUMAN_ADVANCED_PARTS];
	}

	currentRessources[HUMAN_CUBES] = p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES);
	currentRessources[HUMAN_GUN_PARTS] = p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_GUN_PARTS);
	currentRessources[HUMAN_VEHICULE_PARTS] = p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_VEHICULE_PARTS);
	currentRessources[HUMAN_ADVANCED_PARTS] = p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_ADVANCED_PARTS);

	for(int i(0); i < 4; ++i)
	{
		if(currentRessources[i] < cost[i])
			return true;
	}

	//On retire les ressources
	for(int i(0); i < 4; ++i)
		p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, cost[i]);

	//On ajoute l'item à l'unité
	mp_unit->setItem(m_currentItemType, &m_itemList[getTroncateItemType(m_currentItemType)][m_currentSelectedItemID]);

	updateItemInfo();
	return true;
}
int generateID()
{
	static int currentID(-1);
	return ++currentID;
}
void ShopGUI::init()
{
	m_itemList.resize(ITEMLIST_SIZE);

	ItemFactory* p_itemFactory(ItemFactory::getInstance());
	ItemInformation itemInformation;

	itemInformation.information = "Defensive helmet";
	itemInformation.name = "Kevlar helmet";
	itemInformation.price[0] = 100;
	itemInformation.price[1] = 0;
	itemInformation.price[2] = 0;
	itemInformation.price[3] = 0;
	itemInformation.weight = 2;
	itemInformation.ID = generateID();
	m_itemList[HEAD_ITEMS].push_back(itemInformation);
	p_itemFactory->addItem(m_itemList[HEAD_ITEMS][m_itemList[HEAD_ITEMS].size()-1], new KevlarHelmet());

	itemInformation.information = "Offensive helmet";
	itemInformation.name = "Bandana";
	itemInformation.price[0] = 200;
	itemInformation.price[1] = 0;
	itemInformation.price[2] = 0;
	itemInformation.price[3] = 0;
	itemInformation.weight = 1;
	itemInformation.ID = generateID();
	m_itemList[HEAD_ITEMS].push_back(itemInformation);	
	p_itemFactory->addItem(m_itemList[HEAD_ITEMS][m_itemList[HEAD_ITEMS].size()-1], new KevlarHelmet());
	
	itemInformation.information = "Gun";
	itemInformation.name = "Mac10";
	itemInformation.price[0] = 1;
	itemInformation.price[1] = 0;
	itemInformation.price[2] = 0;
	itemInformation.price[3] = 0;
	itemInformation.weight = 1;
	itemInformation.ID = generateID();
	m_itemList[WEAPON0_ITEMS].push_back(itemInformation);
	p_itemFactory->addItem(m_itemList[WEAPON0_ITEMS][m_itemList[WEAPON0_ITEMS].size()-1], new Mac10());

	itemInformation.information = "A tool";
	itemInformation.name = "Spade";
	itemInformation.price[0] = 1;
	itemInformation.price[1] = 0;
	itemInformation.price[2] = 0;
	itemInformation.price[3] = 0;
	itemInformation.weight = 1;
	itemInformation.ID = generateID();
	m_itemList[WEAPON0_ITEMS].push_back(itemInformation);
	p_itemFactory->addItem(m_itemList[WEAPON0_ITEMS][m_itemList[WEAPON0_ITEMS].size()-1], new ConstructorWeapon());
}

void ShopGUI::setVisible(bool i_isVisible)
{
	if(!i_isVisible)
	{
		m_currentSelectedItemID = -1;
		m_currentItemType = INVALID_ITEMS;
		updateItemInfo();
	}
	GUI::setVisible(i_isVisible);
}

void ShopGUI::updateImage()
{
	Texture::chooseTexture(m_texturesIndex[0]);
	Texture::bindTexture();

	GUIHandler::getInstance()->updateOpenGLImage(m_texturesIndex[0], MAP_LAYOUT, "map/map_img", m_scaling, m_offsetX, m_offsetY);
}