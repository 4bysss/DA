//// ###### Config options ################



// #######################################

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <utility>
#include <vector>
#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio;

bool cmp(std::pair<std::pair<int, int>,float>e1,std::pair<std::pair<int, int>,float>e2){return e1.second>e2.second;}
//Ordenado por fusion
//TODO Explicar codigo
//
void fusion(std::pair<std::pair<int,int>, float>* vect,int inicio, int fin, int mitad){
    int tamIz = mitad - inicio + 1 ;
    int tamDe = fin - mitad;

    std::pair<std::pair<int,int>, float>* vectIz = new std::pair<std::pair<int,int>, float>[tamIz];

    std::pair<std::pair<int,int>, float>* vectDer = new std::pair<std::pair<int,int>, float>[tamDe];
    for(auto i = 0; i < tamIz; i++){
        vectIz[i] = vect[inicio + i];

    }
    for(auto j = 0; j < tamDe; j++){
        vectDer[j] = vect[j + 1 + mitad];
    }
    int indiceIzquierdo = 0;
    int indiceDerecho = 0;
    int indiceFusionado = inicio;
    while(indiceIzquierdo < tamIz && indiceDerecho < tamDe){
        if(vectIz[indiceIzquierdo].second <= vectDer[indiceDerecho].second){
            vect[indiceFusionado] = vectIz[indiceIzquierdo];
            
            indiceIzquierdo++;
        }
        else{
            vect[indiceFusionado] = vectDer[indiceDerecho];
            indiceDerecho++;
        }
        indiceFusionado++;
    }

    while(indiceIzquierdo<tamIz){
        vect[indiceFusionado] = vectIz [indiceIzquierdo];
        indiceFusionado++;
        indiceIzquierdo++;
    }
    while(indiceDerecho<tamDe){
        vect[indiceFusionado] = vectDer[indiceDerecho];
        indiceFusionado++;
        indiceDerecho++;
    }
    delete[] vectIz;
    delete[] vectDer;
}
void ordenadoFusion(std::pair<std::pair<int,int>, float>* vect,int inicio, int fin){
    if(inicio>=fin){
        return;
    }
    else{
        auto mitad = inicio + (fin - inicio) / 2;
        ordenadoFusion(vect, inicio,mitad);
        ordenadoFusion(vect, mitad+1, fin);
        fusion(vect,inicio,fin,mitad);
    }
}
//====================================


int particion(std::pair<std::pair<int,int>, float>* vect,int inferior, int superior){
    float pivote = vect[superior].second;
    int i = (inferior -1);
    for (int j = inferior; j <= superior-1; j++) {
        if(vect[j].second < pivote){
            ++i;
            auto aux = vect[j];
            vect[j]= vect[i];
            vect[i]= aux;
        }
    }
    auto aux = vect[i+1];
    vect[i+1]= vect[superior];
    vect[superior]= aux;
    return (i+1);
}


void ordenadoRapido(std::pair<std::pair<int,int>, float>* vect,int inferior, int superior){
    if(inferior < superior){
        int part = particion(vect,inferior,superior);
        ordenadoRapido(vect,inferior,part-1);
        ordenadoRapido(vect,part+1,superior);
    }
}
Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight)
{

    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);

}


//Funcion de factibilidad especifica para el centro de extraccion de minerales
bool factibleExtraccion(int row, int col,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstacles,std::list<Defense*>defenses)
{
  List<Defense*>::iterator base = defenses.begin();//Seleccionamos la primera defensa que representa el CEM

  float radio = (*base)->radio;//Extraemos el radio de la base por comodidad y legibilidad en el codigo 

  Vector3 posicion = cellCenterToPosition(row,col,cellWidth,cellHeight);//Obtenemos la posicio correspondiente a las celdas
  

  if(posicion.x+radio>=mapWidth||posicion.x-radio<=0||posicion.y+radio>=mapWidth||posicion.y-radio<=0)//Comprobamos que no salga de los bordes del mapa
  {
    return false;//En caso de que salga de alguno de los bordes devolvera que la posicion no es valida
  }

  else
  {

    for(std::list<Object*>::iterator it = obstacles.begin();it!=obstacles.end();it++){//Recorreremos todos los obstaculos para comprobar que la base no colisiona con ninguno de estos
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
      //Calculamos la distancia Euclidea entre la base y el obstaculo que estamos revisando
      //Comprobamos que la suma de los radios del obstaculo y de la base es ingerior a la distancia que los separa, asegurando que sus radios no colisionan     
      if(distancia<=(*it)->radio + radio)
      {

        return false;

      }
    }

    
  }

  return true;

}


//Funcion de factibilidad dedicada a las defensas, las subfunciones cuyo comportamiento ya se haya explicado en funciones anteriores sera marcado con "==="
bool factibleDefensa(int row, int col,float radio,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstacles,std::list<Defense*>defenses){

  List<Defense*>::iterator base = defenses.begin();//Extraemos la base para su posterior uso 

  Vector3 posicion = cellCenterToPosition(row,col,cellWidth,cellHeight);//===
  
  if((posicion.x+radio)>=mapWidth||(posicion.x-radio)<=0||(posicion.y+radio)>=mapHeight||(posicion.y-radio)<=0){//===

    return false;

  }

  else{

    for(std::list<Object*>::iterator it = obstacles.begin();it!=obstacles.end();it++){//===

      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));

      if(distancia<(*it)->radio + radio){

        return false;

      }

    }

    for(std::list<Defense*>::iterator it = defenses.begin();it!=defenses.end();it++){//Tiene el mismo comportamiento que ya mencionamos con los obstaculos pero aplicado a las defensas

      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));

      if(distancia<(*it)->radio + radio){

        return false;

      }

    }

  }

  return true;

}

//Funcion que devuelve la distancia euclidea entre dos pares de coordenadas
float distanciaEuclidea(float primerx,float primery,float segundox,float segundoy){return sqrt(pow((primerx-segundox),2)+pow((primery-segundoy),2)); }





float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight               
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    	
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);
    	
    float val = 0;
    for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
	    val += _distance(cellPosition, (*it)->position);
    }

    return val;
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

  float cellWidth = mapWidth / nCellsWidth;//===
  float cellHeight = mapHeight / nCellsHeight;

  int imax,jmax;
  imax = 0;
  jmax = 0;
  int maxAttemps = 1000;

  List<Defense *>::iterator currentDefense = defenses.begin();//===


bool colocado = false;//Semaforo que determinara si se ha colocado la base o no


//Comienza ahora la segunda parte del algoritmo devorador que en esencia tiene el mismo funcionamiento salvando pequeños cambios con respecto a la primera parte
std::pair<std::pair<int, int>,float> Ordenacion[nCellsHeight*nCellsWidth];
std::pair<std::pair<int, int>,float> t1[nCellsHeight*nCellsWidth];
std::pair<std::pair<int, int>,float> t2[nCellsHeight*nCellsWidth];
std::vector<std::pair<std::pair<int, int>,float>> t3(nCellsWidth*nCellsHeight);

  for (int i = 0; i < nCellsHeight; i++)//===
  {
    for (int j = 0; j < nCellsWidth; j++){
        Ordenacion[i*nCellsHeight+j]= (std::make_pair(std::make_pair(i, j),  defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses)));
        t1[i*nCellsHeight+j]= (std::make_pair(std::make_pair(i, j),  defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses)));
        t2[i*nCellsHeight+j]= (std::make_pair(std::make_pair(i, j),  defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses)));
        t3[i*nCellsHeight+j]= (std::make_pair(std::make_pair(i, j),  defaultCellValue(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses)));
       }
   }
    cronometro tiempito;
    tiempito.activar();
   ordenadoRapido(Ordenacion, 0, nCellsHeight*nCellsWidth-1);
   for (int i =0 ; i<nCellsHeight*nCellsWidth; i++) {
    int aux1 = t1[i].first.first;
    int aux2 = t1[i].first.second;
  }
    tiempito.parar();
    double tiempoRapido = tiempito.tiempo();
    tiempito.activar();
   ordenadoFusion(Ordenacion, 0, nCellsHeight*nCellsWidth-1);
   for (int i =0 ; i<nCellsHeight*nCellsWidth; i++) {
    int aux1 = t1[i].first.first;
    int aux2 = t1[i].first.second;
  }
    tiempito.parar();
    double tiempoFusion = tiempito.tiempo();
    tiempito.activar();
    std::make_heap(t3.begin(),t3.end(),cmp);
   for (int i = 0; i<nCellsHeight*nCellsHeight; i++) {
        std::pop_heap(t3.begin(),t3.end(),cmp);
        t3.pop_back();
   }
    tiempito.parar();
    double tiempoMonti = tiempito.tiempo();
    std::cout<<"Tiempo: "<<tiempoRapido <<" "<<tiempoFusion<<" "<<tiempoMonti<<std::endl;
    for (int i = 0; i<nCellsHeight*nCellsHeight; i++) {
        std::cout << "["<<Ordenacion[i].first.first<<"]["<<Ordenacion[i].first.second<<"] ==>"<<Ordenacion[i].second<< std::endl; 
   }
   ordenadoRapido(Ordenacion, 0, nCellsHeight*nCellsWidth-1);


   int indiceVect = nCellsHeight*nCellsWidth-1;
   while(!colocado){//Bucle que iterara hasta encontrar la mejor posicion para la base

    imax = Ordenacion[indiceVect].first.first;
    jmax = Ordenacion[indiceVect].first.second;

    if(factibleExtraccion(imax,jmax,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)){
      (*currentDefense)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);//En caso de ser valido le asignamos a la base las coordenadas del par

      colocado = true;//Indicamos que la base ha sido colocada y que podemos salir del bucle
      indiceVect--;
    }
    else{indiceVect--;}//En caso de no ser factible le damos un valor virtualmente imposible de modo que no se valore de nuevo como opcion
  }
 // currentDefense++;
  for (auto it = currentDefense; it != defenses.end(); it++)//===
  {
    
    float radius = (*it)->radio;
    imax = Ordenacion[indiceVect].first.first;
    jmax = Ordenacion[indiceVect].first.second;
    indiceVect--;

    if(factibleDefensa(imax,jmax,radius,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses))//===
    {
      (*it)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);

    }
    else{it--;}
  }
  

#ifdef PRINT_DEFENSE_STRATEGY

  float **cellValues = new float *[nCellsHeight];
  for (int i = 0; i < nCellsHeight; ++i) {
    cellValues[i] = new float[nCellsWidth];
    for (int j = 0; j < nCellsWidth; ++j) {
      cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
    }
  }
  dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth,
            freeCells, cellValues, std::list<Defense *>(), true);

  for (int i = 0; i < nCellsHeight; ++i)
    delete[] cellValues[i];
  delete[] cellValues;
  cellValues = NULL;

#endif
}
/*
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 

	cronometro c;
    long int r = 0;
    c.activar();
    do {	
		List<Defense*>::iterator currentDefense = defenses.begin();
		while(currentDefense != defenses.end() && maxAttemps > 0) {

			(*currentDefense)->position.x = ((int)(_RAND2(nCellsWidth))) * cellWidth + cellWidth * 0.5f;
			(*currentDefense)->position.y = ((int)(_RAND2(nCellsHeight))) * cellHeight + cellHeight * 0.5f;
			(*currentDefense)->position.z = 0; 
			++currentDefense;
		}
		
		++r;
    } while(c.tiempo() < 1.0);
    c.parar();
    std::cout << (nCellsWidth * nCellsHeight) << '\t' << c.tiempo() / r << '\t' << c.tiempo()*2 / r << '\t' << c.tiempo()*3 / r << '\t' << c.tiempo()*4 / r << std::endl;*/

