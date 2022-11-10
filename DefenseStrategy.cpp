// ###### Config options ################

// #define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

bool factibleExtraccion(int row, int col,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstacles,std::list<Defense*>defenses){
  List<Defense*>::iterator base = defenses.begin();
  float radio = (*base)->radio;
  Vector3 posicion = cellCenterToPosition(row,col,cellWidth,cellHeight);
  

  if(posicion.x+radio>=mapWidth||posicion.x-radio<=0||posicion.y+radio>=mapWidth||posicion.y-radio<=0){
    return false;
  }

  else{

    for(std::list<Object*>::iterator it = obstacles.begin();it!=obstacles.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
    

      if(distancia<=(*it)->radio +radio){
        //std::cout<<"Para la corrdenada: "<<posicion.x<<", "<<posicion.y<<" No se puede poner \n";
        return false;
      }
    }

    
  }
  return true;
}

bool factibleDefensa(int row, int col,float radio,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstacles,std::list<Defense*>defenses){
  List<Defense*>::iterator base = defenses.begin();
  Vector3 posicion = cellCenterToPosition(row,col,cellWidth,cellHeight);
  
  if((posicion.x+radio)>=mapWidth||(posicion.x)<=0||(posicion.y+radio)>=mapHeight||(posicion.y)<=0){
    //std::cout<<"No cale pra"<<posicion.x<<" y "<<posicion.y<<"\n";
    return false;
  }

  else{

    for(std::list<Object*>::iterator it = obstacles.begin();it!=obstacles.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));

      if(distancia<(*it)->radio + radio){
        return false;
      }

    }

    for(std::list<Defense*>::iterator it = defenses.begin();it!=defenses.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
      if(distancia<(*it)->radio + radio){
        return false;
      }
    }
  }
  return true;
}


float distanciaEuclidea(float primerx,float primery,float segundox,float segundoy){return sqrt(pow((primerx-segundox),2)+pow((primery-segundoy),2)); }



float cellValueBase(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) {
  float mitadX = mapWidth/2;
  float mitadY = mapHeight/2;

  float distanciaAObs = 0;
  float distanciaTotalObs = 0;
  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);
  float ditanciaCentro = distanciaEuclidea(v.x,v.y,mitadX,mitadY);
  float radMin =distanciaEuclidea(v.x,v.y,(*obstacles.begin())->position.x,(*obstacles.begin())->position.y);

  for(auto it = obstacles.begin();it!=obstacles.end();it++){
    distanciaAObs=distanciaEuclidea(v.x,v.y,(*it)->position.x,(*it)->position.y);
    distanciaTotalObs+=distanciaAObs;
    radMin = std::min(distanciaTotalObs,radMin);
  }
  return (0.7f*(1000/ditanciaCentro)+0.1f*(radMin)+0.2f*(1000/distanciaAObs)); 
}




float cellValueDefensa(int row, int col, bool **freeCells, int nCellsWidth,int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) {
  float posxBase = (*defenses.begin())->position.x;
  float posyBase = (*defenses.begin())->position.y;
  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);
  return 1000/distanciaEuclidea(v.x,v.y,posxBase,posyBase);

}



void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth,float mapHeight,std::list<Object *> obstacles,std::list<Defense *> defenses) {
  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  float puntacionMaxima = -1;
  float puntuaciones[nCellsHeight][nCellsWidth];
  float puntuaciones2[nCellsHeight][nCellsWidth];
  int imax,jmax;
  imax = 0;
  jmax = 0;
  int maxAttemps = 1000;
  List<Defense *>::iterator currentDefense = defenses.begin();

//Rellenamos matriz de puntuacion para la base
  for (int i = 0; i < nCellsHeight; i++)
  {
    for (int j = 0; j < nCellsWidth; j++){
        puntuaciones[i][j] = cellValueBase(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
      }
  }
bool colocado = false;
while(!colocado){
  for (int i = 0; i < nCellsHeight; i++)
  {
    for (int j = 0; j < nCellsWidth; j++)
    {
        if(puntuaciones[i][j]>puntuaciones[imax][jmax]){
          imax = i;
          jmax = j;
        }
    }
  }
  
  if(factibleExtraccion(imax,jmax,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)){
    (*currentDefense)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);
    colocado = true;
  }else{puntuaciones[imax][jmax]=-1;}
}
  currentDefense++;
  int defensa = 1;
  

//Segunda parte
  for (int i = 0; i < nCellsHeight; i++)
  {
    for (int j = 0; j < nCellsWidth; j++){
        puntuaciones2[i][j] = cellValueDefensa(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
      }
    }




  for (auto it = currentDefense; it != defenses.end(); it++)
  {
    float radius = (*it)->radio;
  for (int i = 0; i < nCellsHeight; i++)
  {
    for (int j = 0; j < nCellsWidth; j++)
    {
        if(puntuaciones2[i][j]>puntuaciones2[imax][jmax]){
          imax = i;
          jmax = j;
        }
    }
  }
    if(factibleDefensa(imax,jmax,radius,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)&&puntuaciones2[imax][jmax]!=-1)
    {
    (*it)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);
    }
  else{puntuaciones2[imax][jmax]=-1;it--;}
    defensa++;
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
