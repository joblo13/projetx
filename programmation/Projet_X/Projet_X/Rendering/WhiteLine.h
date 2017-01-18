#ifndef WHITE_LINE_H
#define WHITE_LINE_H

#include "Drawable.h"
#include "../Definition.h"
#include <vector>

class Vector;

class WhiteLine: public Drawable
{
public:
	WhiteLine();
	~WhiteLine();
	void initBuffer();
	virtual void fillBuffer();
protected:
	virtual void generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices) = 0;
	virtual void preTreatementDraw()const;
	virtual void postTreatementDraw()const;
private:
	void clear();
};

#endif //WHITE_LINE_H
