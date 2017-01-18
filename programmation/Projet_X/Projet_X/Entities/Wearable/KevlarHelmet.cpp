#include "KevlarHelmet.h"

KevlarHelmet::KevlarHelmet()
{

}

KevlarHelmet::KevlarHelmet(const KevlarHelmet& i_weapon)
:Armor(i_weapon)
{

}

KevlarHelmet::~KevlarHelmet()
{

}

Item* KevlarHelmet::getCopy()
{
	return new KevlarHelmet(*this);
}