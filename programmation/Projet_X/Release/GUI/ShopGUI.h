#ifndef SHOP_GUI_H
#define SHOP_GUI_H

#include "GUI.h"
#include "../Entities/ItemInformation.h"

class Camera;
class Unit;

class ShopGUI : public GUI
{
	public:
		ShopGUI();
		virtual ~ShopGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();	

		virtual void setVisible(bool i_isVisible);

	protected:
		void init();

		bool selectBackpackHandler(const CEGUI::EventArgs& i_event);
		bool selectHeadHandler(const CEGUI::EventArgs& i_event);
		bool selectTorsoHandler(const CEGUI::EventArgs& i_event);
		bool selectLegsHandler(const CEGUI::EventArgs& i_event);
		bool selectionChanged(const CEGUI::EventArgs& i_event);

		bool selectWeapon0Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon1Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon2Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon3Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon4Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon5Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon6Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon7Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon8Handler(const CEGUI::EventArgs& i_event);
		bool selectWeapon9Handler(const CEGUI::EventArgs& i_event);

		void updateItemInfo();

		bool buyItem(const CEGUI::EventArgs& i_event);

		bool loadItemChoice(ItemType i_itemType);

		std::vector<std::vector<ItemInformation>> m_itemList;

		ItemType m_currentItemType;
		int m_currentSelectedItemID;

		Unit* mp_unit;
};


#endif //SHOP_GUI_H
