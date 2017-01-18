#ifndef KEVLAR_HELMET_H
#define KEVLAR_HELMET_H

#include "Armor.h"

class KevlarHelmet:public Armor
{
	public:
		KevlarHelmet();
		KevlarHelmet(const KevlarHelmet& i_weapon);
		virtual ~KevlarHelmet();

		virtual Item* getCopy();

	protected:
};

#endif //KEVLAR_HELMET_H
