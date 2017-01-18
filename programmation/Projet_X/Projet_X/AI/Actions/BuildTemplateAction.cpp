#include "BuildTemplateAction.h"
#include "../../Building/Template.h"
#include "../../Math/Vector.h"
#include "../../Entities/Unit.h"

BuildTemplateAction::BuildTemplateAction(Template* ip_template)
:mp_template(ip_template)
{
	m_type = BUILD_ACTION;
}
BuildTemplateAction::~BuildTemplateAction()
{

}
void BuildTemplateAction::executeAction(Unit* ip_unit, Uint32 i_timestep)
{
		//On oriente l'unité vers le template à construire.
		Vector vector;
		Vector vectorAngle;
		float vectorLength;

		vector = mp_template->getBottomCenterPosition() - ip_unit->getPosition();
		vector.y = 0.f;
				
		vectorLength = vector.length();
		if(vectorLength != 0.f)
		{
			//On oriente l'unité vers le cube à détruire.
			vectorAngle = vector;
			vectorAngle.normalize();
			ip_unit->m_theta = std::acos((float)(Vector(0.f, 0.f, 1.f) * vectorAngle)) * (180.f/MATH_PI);
			
		}

		//On détruit le cube devant l'unité.
		if(mp_template->build(ip_unit, i_timestep))
		{
			//Target atteint
			ip_unit->mp_targetList.pop_back();
		}
}