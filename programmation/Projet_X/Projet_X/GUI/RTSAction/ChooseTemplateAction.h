#ifndef CHOOSE_TEMPLATE_ACTION_H
#define CHOOSE_TEMPLATE_ACTION_H

#include "RTSAction.h"
#include <map>

class Chunk;

class ChooseTemplateAction: public RTSAction
{
	public:
		ChooseTemplateAction(RTSGUI* ip_GUIPointer, const std::string& i_name);
		~ChooseTemplateAction();
		
		virtual void doAction(const std::string& i_param);
		void addTemplate(const std::string& i_fileName);
		Chunk* getTemplateShape(int i_index);
		virtual void createButton();
	protected:

	private:
		void loadAllTemplate();
		
		void getTemplateDetails(const std::string& i_fileName, std::string& o_imageName, std::vector<SDLKey>& o_shortcut);

		std::vector<Chunk*> m_templateList;

		
		std::map<std::string, int> m_templateIDs;
};


#endif //CHOOSE_TEMPLATE_ACTION_H
