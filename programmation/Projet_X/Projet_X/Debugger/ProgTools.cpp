#include "ProgTools.h"

//include externe
#include <cstring>

//include interne
#include "../Math/Vector.h"
#include "../Math/Matrix.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, char i_text)
///
/// Fonction permettant d'imprimer du text avec un char dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] i_text: text à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void printLog(const char* ikp_fileName, char i_text)
{
        char* p_text;
        p_text = &i_text;
        FILE* p_file(NULL);
        p_file = fopen(ikp_fileName, "a+");
        fprintf(p_file, p_text);
        fclose(p_file);

}
////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, const char* i_text)
///
/// Fonction permettant d'imprimer du text avec un const char* dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] i_text: text à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////

void printLog(const char* ikp_fileName, const char* i_text)
{
        FILE* p_file(NULL);
        p_file=fopen(ikp_fileName, "a+");
        fprintf(p_file, i_text);
        fclose(p_file);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, float i_chiffre)
///
/// Fonction permettant d'imprimer un float dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] i_chiffre: float à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void printLog(const char* ikp_fileName, float i_number)
{
    char a_converter[32];
    sprintf(a_converter,"%f\n",i_number);
        FILE* p_file(NULL);
        p_file=fopen(ikp_fileName, "a+");
        fprintf(p_file, a_converter);
        fclose(p_file);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, int i_number)
///
/// Fonction permettant d'imprimer un int dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] i_number: int à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void printLog(const char* ikp_fileName, int i_number)
{
    char a_converter[32];
    sprintf(a_converter,"%i",i_number);
        FILE* p_file(NULL);
        p_file=fopen(ikp_fileName, "a+");
        fprintf(p_file, a_converter);
        fclose(p_file);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, const Vector& ik_vector)
///
/// Fonction permettant d'imprimer la contenue d'un vecteur dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] ik_vector: vecteur à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void printLog(const char* ikp_fileName, const Vector& ik_vector)
{
    char a_converter[32];

        FILE* p_file(NULL);
        p_file=fopen(ikp_fileName, "a+");
			fprintf(p_file, "(");
			sprintf(a_converter,"%f", ik_vector.x);
			fprintf(p_file, a_converter);
			fprintf(p_file, ", ");

			sprintf(a_converter,"%f",ik_vector.y);
			fprintf(p_file, a_converter);
			fprintf(p_file, ", ");

			sprintf(a_converter,"%f",ik_vector.z);
			fprintf(p_file, a_converter);
			fprintf(p_file, ") \n");
        fclose(p_file);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void printLog(std::string ikp_fileName, const Matrix& i_matrix)
///
/// Fonction permettant d'imprimer la contenue de la matrice dans un fichier
/// 
/// @param[in] ikp_fileName: nom du fichier où la matrice doit être imprimée
/// @param[in] ik_matrix: matrice à imprimer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void printLog(std::string ikp_fileName, const Matrix& ik_matrix)
{
	//On initialise un fichier
    FILE* p_file(NULL);
	//On créer un convertisseur
	char a_converter[32];

	//On ouvre le fichier
    p_file=fopen(ikp_fileName.c_str(), "a+");

	//On récupère les valeurs
	float* p_value = ik_matrix.getValues();

	//On imprime chaques elements 
	for(int i = 0; i < 16; ++i)
	{
		sprintf(a_converter,"%f", p_value[i]);
		fprintf(p_file, a_converter);
		fprintf(p_file, " ");
		if( (i+1) % 4 == 0)
			fprintf(p_file, "\n");
	}
	fprintf(p_file, "\n");

	//On ferme le fichier 
    fclose(p_file);
}