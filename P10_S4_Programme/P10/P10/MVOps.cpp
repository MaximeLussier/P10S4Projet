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

//------------------------------------------
//	Fonction pour trouver le minimum d'un vecteur et son indice.
//	Entrées: le vecteur, sa longueur, un pointeur pour sortir l'index
//	Retourne: la valeur du minimum
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour trouver le maximum d'un vecteur et son indice.
//	Entrées: le vecteur, sa longueur, un pointeur pour sortir l'index
//	Retourne: la valeur du maximum
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour trouver le maximum d'une matrice et son indice.
//	Entrées: la matrice, le nombre de lignes, de colonnes, un pointeur vers un array pour sortir l'index
//	Retourne: la valeur du maximum
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour obtenir un sous vecteur de doubles
//	Entrées: le vecteur, l'index de départ, de fin
//	Retourne: le sous vecteur de doubles
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::subVector(double* original, int startIndex, int endIndex){

	double* subV = new double[endIndex-startIndex+1];
	for(int i = 0; i < (endIndex-startIndex+1); i++)
	{
		subV[i] = original[i+startIndex];
	}
	return subV;
}

//------------------------------------------
//	Fonction pour obtenir un sous vecteur d'entiers
//	Entrées: le vecteur, l'index de départ, de fin
//	Retourne: le sous vecteur d'entiers
//	Auteur: Maxime Lussier
//------------------------------------------
int* MVOps::subVector(int* original, int startIndex, int endIndex){

	int* subV = new int[endIndex-startIndex+1];
	for(int i = 0; i < (endIndex-startIndex+1); i++)
	{
		subV[i] = original[i+startIndex];
	}
	return subV;
}

//------------------------------------------
//	Fonction pour obtenir une sous-matrice de doubles
//	Entrées: la matrice, la ligne minimum, la ligne finale,
//           la colonne minimum, la colonne finale
//	Retourne: la sous-matrice de doubles
//	Auteur: Maxime Lussier
//------------------------------------------
double** MVOps::subMatrix(double** X, int rowI, int rowF, int colI, int colF){

	double** Y = newMatrix(rowF-rowI+1, colF-colI+1);
	for(int i = rowI; i <= rowF; i++){
		for(int j = colI; j <= colF; j++){
			Y[i-rowI][j-colI] = X[i][j];
		}
	}
	return Y;
}

//------------------------------------------
//	Fonction pour trouver les indices des points supérieurs à evaluate (Checked Working)
//	Entrées: le vecteur, la valeur a evaluer, un pointeur vers le nombre de points trouves
//	Retourne: le vecteur des indices
//	Auteur: Matlab : Marc-Antoine Veilleux
//             C++ : Maxime Lussier
//------------------------------------------
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


//------------------------------------------
//	Fonction pour trouver les valeurs d'un vecteur aux indices donnés (Checked Working)
//	Entrées: le vecteur de doubles contenant les valeurs, le vecteur des indices,
//  le nombre d'indices
//	Retourne: le vecteur des valeurs
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::getValuesAt(double* Values, int* index, int indexLength){

	double* subV = new double[indexLength];
	for(int i = 0; i < indexLength; i++)
	{
		subV[i] = Values[index[i]];
	}
	return subV;
}

//------------------------------------------
//	Fonction qui génère un vecteur zéros de longeur spécifiée (entiers) (Checked Working)
//	Entrées: la longueur voulue
//	Retourne: le vecteur des zéros
//	Auteur: Maxime Lussier
//------------------------------------------
int* MVOps::zeros(int size){
	int* zero = new int[size];
	for(int i = 0; i < size;i++){
		zero[i] = 0;
	}
	return zero;
}

//------------------------------------------
// 	génère une matrice de zéros de grandeur spécifiée (Checked Working)
//	Entrées: le nombre de lignes, le nombre de colonnes
//	Retourne: le vecteur des zéros
//	Auteur: Maxime Lussier
//------------------------------------------
double** MVOps::zeros(int sizeL, int sizeC){
	double** Z = new double*[sizeL];
	for(int i = 0; i < sizeL; i++){
		Z[i] = new double[sizeC];
		for(int j = 0; j < sizeC; j++)
			Z[i][j] = 0;
	}
	return Z;
}

//------------------------------------------
//	Fonction qui génère un vecteur zéros de longeur spécifiée (double) (Checked Working)
//	Entrées: la longueur voulue
//	Retourne: le vecteur des zéros
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::zerosD(int size){
	double* zero =  new double[size];
	for(int i = 0; i < size;i++){
		zero[i] = 0;
	}
	return zero;
}

//------------------------------------------
//	Fonction qui retourne la colonne d'une matrice (Checked Working)
//	Entrées: la matrice, la colonne voulue, le nombre de lignes
//	Retourne: le vecteur colonne
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::getColumn(double** In, int col, int nbRow)
{
	double* out = new double[nbRow];
	for(int i = 0; i < nbRow; i++){
		out[i] = In[i][col];
	}
	return out;
}

//------------------------------------------
//	Fonction qui retourne une matrice à partir d'un vecteur (Checked Working)
//	Entrées: le vecteur, sa longueur
//	Retourne: la matrice contenant le vecteur
//	Auteur: Maxime Lussier
//------------------------------------------
double** MVOps::vectorToMatrix(double* vct, int length){
	double** Y = new double*[length];
	for(int i = 0; i < length; i++){
		Y[i] = new double[1];
		Y[i][0] = vct[i];
	}
	return Y;
}

//------------------------------------------
//	Fonction qui concatène des vecteurs (Checked Working)
//	Entrées: le vecteur 1, sa longueur, le vecteur 2, sa longueur
//	Retourne: le vecteur concaténé
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction qui fait la puissance d'un vecteur (Checked Working)
//	Entrées: le vecteur, sa longueur, l'exposant
//	Retourne: le vecteur calculé
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::vectorPow(double* vector, int length, int exponent){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = pow(vector[i], exponent);
	}
	return Y;
}

//------------------------------------------
//	Fonction qui additionne deux vecteurs (Checked Working)
//	Entrées: le vecteur 1, le vecteur 2, leurs longueurs
//	Retourne: le vecteur calculé
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::vectorAdd(double* vector1, double* vector2, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = vector1[i] + vector2[i];
	}
	return Y;
}

//------------------------------------------
//	Fonction qui calcule le produit point par point d'un vecteur (Checked Working)
//	Entrées: le vecteur 1, le vecteur 2, leurs longueurs
//	Retourne: le vecteur calculé
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::vectorDotProduct(double* vector1, double* vector2, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = vector1[i] * vector2[i];
	}
	return Y;
}

//------------------------------------------
//	Fonction qui calcule le produit d'un scalaire (double) par un vecteur (Checked Working)
//	Entrées: le vecteur, le scalaire, la longueur du vecteur
//	Retourne: le vecteur calculé
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::vectorProduct(double* vector1, double doub, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = doub * vector1[i];
	}
	return Y;
}

//------------------------------------------
//	Fonction qui calcule le produit d'un scalaire (entier) par un vecteur (Checked Working)
//	Entrées: le vecteur, le scalaire, la longueur du vecteur
//	Retourne: le vecteur calculé
//	Auteur: Maxime Lussier
//------------------------------------------
double* MVOps::vectorProduct(double* vector1, int in, int length){
	double* Y = new double[length];
	for(int i = 0; i < length; i++){
		Y[i] = in * vector1[i];
	}
	return Y;
}

//------------------------------------------
//	Fonction qui calcule l'inverse d'une matrice (Checked Working)
//	Entrées: la matrice, son ordre (N = M)
//	Retourne: la matrice inverse
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour Calculer la mineure des éléments (row, col) (Checked Working)
//	Entrées: la matrice d'entrée, la matrice de sortie,
//  la ligne et la colonne de la mineure à calciler, son ordre (N = M)
//	Retourne: rien (en référence)
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour calculer le déterminant (récursivement) (Checked Working)
//	Entrées: la matrice d'entrée, l'ordre (M=N)
//	Retourne: le déterminant
//	Auteur: Maxime Lussier
//------------------------------------------
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

//------------------------------------------
//	Fonction pour calculer le produit matriciel (Checked Working)
//	Entrées: la matrice 1, son nombre de lignes, de colonnes,
//   		 la matrice 2, son nombre de lignes, de colonnes.
//	Retourne: la matrice calculée
//	Auteur: Maxime Lussier
//------------------------------------------
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


//------------------------------------------
//	Fonction pour calculer le produit matriciel avec un vecteur (Checked Working)
//	Entrées: la matrice 1, son nombre de lignes, de colonnes,
//   		 le vecteur  , son nombre de lignes.
//	Retourne: la matrice calculée
//	Auteur: Maxime Lussier
//------------------------------------------
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


//------------------------------------------
//	Fonction pour supprimer une matrice de la mémoire (Checked Working)
//	Entrées: la matrice, son nombre de lignes.
//	Retourne: rien.
//	Auteur: Maxime Lussier
//------------------------------------------
void MVOps::deleteMatrix(double** X, int M){
	for(int i = 0; i < M; i++)	// C'est comme ca qu'on efface une matrice.
		delete [] X[i];			//
	delete [] X;				//
}

//------------------------------------------
//	Fonction pour allouer une matrice d'ordre MxN (Checked Working)
//	Entrées: le nombre de lignes, de colonnes.
//	Retourne: la matrice créée.
//	Auteur: Maxime Lussier
//------------------------------------------
double** MVOps::newMatrix(int M, int N){
	double** A = new double*[M];
	for(int i = 0; i < M; i++){
		A[i] = new double[N];
	}
	return A;
}

//------------------------------------------
//	Fonction pour calculer la moyenne d'une matrice (Checked Working)
//	Entrées: la matrice, le nombre de lignes, de colonnes.
//	Retourne: la moyenne.
//	Auteur: Maxime Lussier
//------------------------------------------
double MVOps::mean(double** X, int M, int N){
	double temp = 0;
	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			temp += X[i][j];				
		}
	}
	return temp/((double)M*(double)N);
}

//------------------------------------------
//	Fonction pour calculer la moyenne d'une sous-matrice (Checked Working)
//	Entrées: la matrice, la ligne initiale, finale, la colonne initiale, finale.
//	Retourne: la moyenne.
//	Auteur: Maxime Lussier
//------------------------------------------
double MVOps::mean(double** X, int M1, int M2, int N1, int N2){
	double temp = 0;
	for(int i = M1; i < M2; i++){
		for(int j = N1; j < N2; j++){
			temp += (X[i][j]);				
		}
	}
	return temp/((double)(M2-M1)*(double)(N2-N1));
}

//------------------------------------------
//	Fonction pour calculer pythagore (Checked Working)
//	Entrées: la valeur 1, la valeur 2
//	Retourne: le résultat calculé.
//	Auteur: Maxime Lussier
//------------------------------------------
double MVOps::pythagore(double A, double B){
	return sqrt(pow(A, 2)+pow(B,2));
}

//------------------------------------------
//	Fonction pour garder les trois point les plus perpendiculaires
//	Entrées: matrice des points, nombre à filtrer
//	Retourne: matrice des trois points
//	Auteur: Maxime Lussier
//------------------------------------------
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