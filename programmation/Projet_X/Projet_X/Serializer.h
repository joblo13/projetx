#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <fstream>


void readFromFile(std::ifstream& i_ifstreamFile, std::string& o_value);

template<typename T> 
void readFromFile(std::ifstream& i_ifstreamFile, T& o_value)
{
	unsigned int sizeOfT(sizeof(o_value));
	char* p_buffer(new char[sizeOfT]);
	i_ifstreamFile.read (p_buffer, sizeOfT);
	memcpy(&o_value, p_buffer, sizeOfT);
	delete p_buffer;
}



void writeToFile(std::ofstream& i_ofstreamFile, const std::string& i_value);

template<typename T> 
void writeToFile(std::ofstream& i_ofstreamFile, T i_value)
{
	std::size_t sizeOfT(sizeof(i_value));
	char* p_buffer(new char[sizeOfT]);
	memcpy(p_buffer, &i_value, sizeOfT);
	i_ofstreamFile.write(p_buffer, sizeOfT);
	delete p_buffer;
}

#endif // SERIALIZER_H
