#ifndef PROGTOOLS_H
#define PROGTOOLS_H

class Vector;
class Matrix;

//Fonction global de debug
void printLog(const char* ikp_fileName, char i_text);
void printLog(const char* ikp_fileName, const char* ik_text);
void printLog(const char* ikp_fileName, float i_chiffre);
void printLog(const char* ikp_fileName, int i_chiffre);
void printLog(const char* ikp_fileName, const Vector& ik_vector);
void printLog(const char* ikp_fileName, const Matrix& ik_matrix);

#endif // PROGTOOLS_H
