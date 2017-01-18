#ifndef OBSERVER_H
#define OBSERVER_H

//Interface de l'observeur
class Observer
{
	public:
		Observer();
		virtual ~Observer();
		virtual void update() = 0;
	private:
};

#endif //OBSERVER_H
