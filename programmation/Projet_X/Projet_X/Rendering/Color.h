#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
	Color(unsigned char i_color[3]):red(i_color[0]), green(i_color[1]), blue(i_color[2]){}
	unsigned char red;
	unsigned char green;
	unsigned char blue;
};

#endif // COLOR_H