#include "StdAfx.h"
#include "MVOps.h"
#include "fftw3.h"
#include <math.h>
#include <stdlib.h>
#include <complex>
#include <iostream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
Ensemble de fonctions statiques afin d'effectuer des opérations sur des vecteur ou des matrices

Auteur: Maxime Lussier
*/

// trouve le minimum et son indice. (checked working)
double MVOps::minimum(double* vector, int length, int* indice)
{
	double Ymin = vector[0];       // start with min = first element
	*indice = 0;

	for(int i = 1; i<length; i++)
	{
		if(vector[i] < Ymin) {
			Ymin = vector[i];
			*indice = i;
		}
	}
	return Ymin;
}

// trouve le maximum et son indice. (checked working)
double MVOps::maximum(double* vector, int length, int* indice)
{
	double Ymax = vector[0];       // start with max = first element
	*indice = 0;

	for(int i = 1; i<length; i++)
	{
		if(vector[i] > Ymax){
			Ymax = vector[i];
			*indice = i;
		}
	}
	return Ymax;
}

double MVOps::maximum(double** X, int M, int N, int* indice)
{
	double Ymax = 0; 

	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){ 
			if(X[i][j] > Ymax){
				Ymax = X[i][j];
				indice[0] = j;
				indice[1] = i;
			}
		}
	}
	return Ymax;
}

// Obtains a sub vector from index to index for doubles(Checked Working)
double* MVOps::subVector(double* original, int startIndex, int endIndex){

	double* subV = new double[endIndex-startIndex+1];
	for(int i = 0; i < (endIndex-startIndex+1); i++)
	{
		subV[i] = original[i+startIndex];
	}
	return subV;
}

// Obtains a sub vector from index to index for ints (Checked Working)
int* MVOps::subVector(int* original, int startIndex, int endIndex){

	int* subV = new int[endIndex-startIndex+1];
	for(int i = 0; i < (endIndex-startIndex+1); i++)
	{
		subV[i] = original[i+startIndex];
	}
	return subV;
}

// Obtain a sub matrix trom row to row and col to col (Checked Working)
double** MVOps::subMatrix(double** X, int rowI, int rowF, int colI, int colF){

	double** Y = newMatrix(rowF-rowI+1, colF-colI+1);
	for(int i = rowI; i <= rowF; i++){
		for(int j = colI; j <= colF; j++){
			Y[i-rowI][j-colI] = X[i][j];
		}
	}
	return Y;
}

// trouve les indices des points supérieurs à evaluate (Checked Working)
int* MVOps::FindPoints(double* Vecteur, double evaluate, int length, int* found){

	int* temp = new int[length];
	int foundN = 0;
	for(int i = 0; i < length; i++){
		if(Vecteur[i]>evaluate){
			temp[foundN] = i;
			foundN++;
		}
	}
	int* foundPoints = subVector(temp, 0, foundN-1);
	delete temp;
	*found = foundN;
	return foundPoints;
}

// get values from vector at indexes in given vector... (Checked Working)
double* MVOps::getValuesAt(double* Values, int* index, int indexLength){

	double* subV = new double[indexLength];
	for(int i = 0; i < indexLength; i++)
	{
		subV[i] = Values[index[i]];
	}
	return subV;
}

// génère un vecteur de zéros de longeur spécifié (Checked Working)
int* MVOps::zeros(int size){
	int* zero = new int[size];
	for(int i = 0; i < size;i++){
		zero[i] = 0;
	}
	return zero;
}

// génère une matrice de zéros de longeur spécifié (Checked Working)
double** MVOps::zeros(int sizeL, int sizeC){
	double** Z = new double*[sizeL];
	for(int i = 0; i < sizeL; i++){
		Z[i] = new double[sizeC];
		for(int j = 0; j < sizeC; j++)
			Z[i][j] = 0;
	}
	return Z;
}

// génère un vecteur de zéros de longeur spécifié (Checked Working)
double* MVOps::zerosD(int size){
	double* zero =  new double[size];
	for(int i = 0; i < size;i++){
		zero[i] = 0;
	}
	return zero;
}

double* MVOps::getColumn(double** In, int col, int nbRow)
{
	double* out = new double[nbRow];
	for(int i = 0; i < nbRow; i++){
		out[i] = In[i][col];
	}
	return out;
}

double** MVOps::vectorToMatrix(double* vct, int length){
	double** Y = new double*[length];
	for(int i = 0; i < length; i++){
		Y[i] = new double[1];
		Y[i][0] = vct[i];
	}
	return Y;
}

double* MVOps::appendVectors(double* vect1, int length1, double* vect2, int length2){
	double* Y = new double[length1+length2];
	for(int i = 0; i < length1; i++){
		Y[i] = vect1[i];
	}
	for(int i = 0; i < length2; i++){
		Y[i+length1] = vect2[i];
	}
	return Y;
}

double* MVOps::vectorPow(double* vector, int length, int exponent){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = pow(vector[i], exponent);
	}
	return Y;
}

double* MVOps::vectorAdd(double* vector1, double* vector2, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = vector1[i] + vector2[i];
	}
	return Y;
}

double* MVOps::vectorDotProduct(double* vector1, double* vector2, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = vector1[i] * vector2[i];
	}
	return Y;
}

double* MVOps::vectorProduct(double* vector1, double doub, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = doub * vector1[i];
	}
	return Y;
}

double* MVOps::vectorProduct(double* vector1, int in, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = in * vector1[i];
	}
	return Y;
}


// Inverse d'une matrice
double** MVOps::matrixInversion(double** X, int ordre)
{
	double** Y = newMatrix(ordre, ordre);

	// calculer le déterminant
	double det = 1.0/determinant(X,ordre);

	double* temp = new double[(ordre-1)*(ordre-1)];
	double** mineures = new double*[ordre-1];
	for(int i=0;i<ordre-1;i++)
		mineures[i] = temp+(i*(ordre-1));

	for(int j=0;j<ordre;j++){
		for(int i=0;i<ordre;i++){
			mineure(X,mineures,j,i,ordre);
			Y[i][j] = det*determinant(mineures,ordre-1);
			if(((i+j)%2) == 1)
				Y[i][j] = -Y[i][j];
		}
	}

	delete [] temp;
	delete [] mineures;
	return Y;
}

// Calculer la mineure des éléments (row,col)
void MVOps::mineure(double** X, double** Y, int row, int col, int ordre)
{
	// indique quelle colonne et ligne est copiée
	int colCount=0,rowCount=0;

	for(int i = 0; i < ordre; i++ ){
		if( i != row ){
			colCount = 0;
			for(int j = 0; j < ordre; j++ ){
				// quand j n'est pas l'élément
				if( j != col ){
					Y[rowCount][colCount] = X[i][j];
					colCount++;
				}
			}
			rowCount++;
		}
	}
}

// Calculer le determinant récursivement.
double MVOps::determinant(double **X, int ordre)
{
	// arrèter quand l'ordre est de 1
	if(ordre == 1)
		return X[0][0];

	double det = 0;

	// réserver la mémoire pour les mineures
	double** mineures = new double*[ordre-1];
	for(int i=0;i<ordre-1;i++)
		mineures[i] = new double[ordre-1];

	for(int i = 0; i < ordre; i++ ){
		mineure(X, mineures, 0, i , ordre);
		det += pow(-1.0, i) * X[0][i] * determinant(mineures,ordre-1 );
	}

	// libérer la mémoire
	for(int i=0;i<ordre-1;i++)
		delete [] mineures[i];
	delete [] mineures;

	return det;
}

// Calculer le produit matriciel
double** MVOps::matrixProduct(double** X1, int M1, int N1, double** X2, int M2, int N2){
	double** Y = newMatrix(M1,N2);
	if(N1!=M2)
		return NULL;

	for(int i = 0; i < M1; i++){
		for(int j = 0; j < N2; j++){
			double temp = 0;
			for(int k = 0; k < N1; k++){
				temp += (X1[i][k])*(X2[k][j]);				
			}
			Y[i][j] = temp;
		}
	}
	return Y;
}

// Calculer le produit matriciel (avec un vecteur)
double** MVOps::matrixProduct(double** X1, int M1, int N1, double* X2, int M2){
	double** Y = newMatrix(M1, 1);
	if(N1!=M2)
		return NULL;

	for(int i = 0; i < M1; i++){
			double temp = 0;
			for(int k = 0; k < N1; k++){
				temp += (X1[i][k])*(X2[k]);				
			}
			Y[i][0] = temp;
	}
	return Y;
}

void MVOps::deleteMatrix(double** X, int M){
	for(int i = 0; i < M; i++)	// C'est comme ca qu'on efface une matrice.
		delete [] X[i];			//
	delete [] X;				//
}

double** MVOps::newMatrix(int M, int N){
	double** A = new double*[M];
	for(int i = 0; i < M; i++){
		A[i] = new double[N];
	}
	return A;
}

double MVOps::mean(double** X, int M, int N){
	double temp = 0;
	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			temp += X[i][j];				
		}
	}
	return temp/((double)M*(double)N);
}

double MVOps::mean(double** X, int M1, int M2, int N1, int N2){
	double temp = 0;
	for(int i = M1; i < M2; i++){
		for(int j = N1; j < N2; j++){
			temp += (X[i][j]);				
		}
	}
	return temp/((double)(M2-M1)*(double)(N2-N1));
}

double MVOps::pythagore(double A, double B){
	return sqrt(pow(A, 2)+pow(B,2));
}

double** MVOps::garderPerpendiculaires(double** points, int Nb){
	double** bons = MVOps::newMatrix(3, 2);
	double A[2] = {0, 0};
	double B[2] = {0, 0};
	double Courant = 0;
	int LA, LB;
	int a = 0, b = 0, c = 0, p = 1;
	for(int i = 0; i < Nb; i++){
		for(int j = 0; j < Nb; j++){
			if(j == i)
				continue;
			for(int k = 0; k < Nb; k++){
				if(k == i || k == j)
					continue;

				A[0] = points[j][0]-points[i][0];
				A[1] = points[j][1]-points[i][1];
				B[0] = points[k][0]-points[i][0];
				B[1] = points[k][1]-points[i][1];

				LA = MVOps::pythagore(A[0], A[1]);
				LB = MVOps::pythagore(B[0], B[1]);

				double VProd = (A[0]*B[1]) - (A[1]*B[0]);

				if((VProd/(LA*LB)-1) < p && VProd > Courant){
					Courant = VProd;
					p = (VProd/(LA*LB)-1);
					a = i;
					b = j;
					c = k;
				}

			}
		}
	}

	bons[0][0] = points[a][0];
	bons[0][1] = points[a][1];
	bons[1][0] = points[b][0];
	bons[1][1] = points[b][1];
	bons[2][0] = points[c][0];
	bons[2][1] = points[c][1];
	return bons;

}