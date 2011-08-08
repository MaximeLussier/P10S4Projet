// Classe comprenant le code pour de multiples opérations sur les vecteurs et les matrices

#include "resource.h"       // symboles principaux

class MVOps
{
public:
	static double maximum(double* vector, int length, int* indice);
	static double MVOps::maximum(double** X, int M, int N, int* indice);
	static double minimum(double* vector, int length, int* indice);
	static double* subVector(double* original, int startIndex, int endIndex);
	static int* subVector(int* original, int startIndex, int endIndex);
	static double** subMatrix(double** In, int rowI, int rowF, int colI, int colF);
	static int* FindPoints(double* Vecteur, double evaluate, int length, int* found);
	static double* getValuesAt(double* Values, int* index, int indexLength);
	static int* zeros(int size);
	static double* zerosD(int size);
	static double** zeros(int sizeL, int sizeC);
	static double* getColumn(double** In, int col, int nbRow);
	static double** vectorToMatrix(double* vct, int length);
	static double* appendVectors(double* vect1, int length1, double* vect2, int length2);
	static double* vectorPow(double* vector, int length, int exponent);
	static double* vectorAdd(double* vector1, double* vector2, int length);
	static double* vectorDotProduct(double* vector1, double* vector2, int length);
	static double* vectorProduct(double* vector1, double doub, int length);
	static double* vectorProduct(double* vector1, int in, int length);
	static void deleteMatrix(double** X, int M);
	static double** newMatrix(int M, int N);
	static double** matrixInversion(double** X, int ordre);
	static double determinant(double **X, int ordre);
	static double** matrixProduct(double** X1, int M1, int N1, double** X2, int M2, int N2);
	static double** matrixProduct(double** X1, int M1, int N1, double* X2, int M2);
	static double mean(double** X, int M, int N);
	static double mean(double** X, int M1, int M2, int N1, int N2);
	static double pythagore(double A, double B);
	static double** MVOps::garderPerpendiculaires(double** points, int Nb);

private:
	static void mineure(double** X, double** Y, int row, int col, int ordre);
};