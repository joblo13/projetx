#ifndef HITPOINT_BAR_MESH_H
#define HITPOINT_BAR_MESH_H

#include "Mesh.h"
#include "../../Definition.h"
#include <vector>

class Vector;
class Matrix;

class HitpointBarMesh: public Mesh
{
public:
	HitpointBarMesh();
	~HitpointBarMesh();
protected:
	virtual void updateAdditionnalBuffers()const;
	virtual void preTreatementDraw()const;
	virtual void postTreatementDraw()const;
private:
	void clear();
};

#endif //HITPOINT_BAR_INSTANCIABLE_H
