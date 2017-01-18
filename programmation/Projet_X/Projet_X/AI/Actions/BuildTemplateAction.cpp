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
		//On oriente l'unit� vers le template � construire.
		Vector vector;
		Vector vectorAngle;
		float vectorLength;

		vector = mp_template->getBottomCenterPosition() - ip_unit->getPosition();
		vector.y = 0.f;
				
		vectorLength = vector.length();
		if(vectorLength != 0.f)
		{
			//On oriente l'unit� vers le cube � d�truire.
			vectorAngle = vector;
			vectorAngle.normalize();
			ip_unit->m_theta = std::acos((float)(Vector(0.f, 0.f, 1.f) * vectorAngle)) * (180.f/MATH_PI);
			
		}

		//On d�truit le cube devant l'unit�.
		if(mp_template->build(ip_unit, i_timestep))
		{
			//Target atteint
			ip_unit->mp_targetList.pop_back();
		}
}