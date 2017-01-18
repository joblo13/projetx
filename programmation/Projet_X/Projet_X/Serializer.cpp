#include "Serializer.h"

void readFromFile(std::ifstream& i_ifstreamFile, std::string& o_value)
{
	unsigned int sizeOfT(o_value.length());
	char* p_buffer(new char[sizeOfT + 1]);
	i_ifstreamFile.read (p_buffer, sizeOfT);
	p_buffer[sizeOfT] = '\0';
	o_value = p_buffer;
	delete p_buffer;
}

void writeToFile(std::ofstream& i_ofstreamFile, const std::string& i_value)
{
	i_ofstreamFile.write(i_value.c_str(), sizeof(char) * i_value.length());
}