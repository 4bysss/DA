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

bool factibleExtraccion(int fila, int col,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstaculos,std::list<Defense*>defensas){
  List<Defense*>::iterator base = defensas.begin();
  Vector3 posicion = cellCenterToPosition(fila,col,cellWidth,cellHeight);
  float radio = (*base)->radio;
  if(posicion.x+radio>mapWidth||posicion.x-radio<0||posicion.y+radio>mapHeight||posicion.y-radio<0){
    return false;
  }
  else{
    for(std::list<Object*>::iterator it = obstaculos.begin();it!=obstaculos.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
      if(distancia<(*it)->radio || distancia <(*base)->radio){
        return false;
      }
    }
  }
  return true;
}

bool factibleDefensa(int fila, int col,float rad,float cellWidth,float cellHeight,float mapWidth,float mapHeight,std::list<Object*>obstaculos,std::list<Defense*>defensas){
  List<Defense*>::iterator base = defensas.begin();
  Vector3 posicion = cellCenterToPosition(fila,col,cellWidth,cellHeight);
  
  if(posicion.x+rad>mapWidth||posicion.x-rad<0||posicion.y+rad>mapHeight||posicion.y-rad<0){
    return false;
  }
  else{
    for(std::list<Object*>::iterator it = obstaculos.begin();it!=obstaculos.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
      if(distancia<(*it)->radio || distancia <(*base)->radio){
        return false;
      }
    }
    for(std::list<Defense*>::iterator it = defensas.begin();it!=defensas.end();it++){
      float distancia = sqrt(pow((posicion.x-(*it)->position.x),2)+pow((posicion.y-(*it)->position.y),2));
      if(distancia<(*it)->radio || distancia <(*base)->radio){
        return false;
      }
    }
  }
  return true;
}


float distanciaEuclidea(float primerx,float primery,float segundox,float segundoy){return sqrt(pow((primerx-segundox),2)+pow((primery-segundoy),2)); }
float cellValueBase(int row, int col, bool **freeCells, int nCellsWidth,int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) {
  float mitadX = mapWidth/2;
  float mitadY = mapHeight/2;
  float ditanciaCentro = distanciaEuclidea(row,col,mitadX,mitadY);
  float distanciaTotalObs = 0;
  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);
  for(auto it = obstacles.begin();it!=obstacles.end();it++){
    distanciaTotalObs+=distanciaEuclidea(v.x,v.y,(*it)->position.x,(*it)->position.y);
  }
  return ((1/ditanciaCentro)+distanciaTotalObs); 
}
float cellValueDefensa(int row, int col, bool **freeCells, int nCellsWidth,int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) {
  float posxBase = (*defenses.begin())->position.x;
  float posyBase = (*defenses.begin())->position.y;
  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);
  return 1/distanciaEuclidea(v.x,v.y,posxBase,posyBase);

}
void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth,float mapHeight,std::list<Object *> obstacles,std::list<Defense *> defenses) {

  float cellWidth = mapWidth / nCellsWidth;
  float cellHeight = mapHeight / nCellsHeight;
  float puntacionMaxima = -1;
  int imax,jmax;
  imax = 0;
  jmax = 0;
  int maxAttemps = 1000;
  List<Defense *>::iterator currentDefense = defenses.begin();
  for (int i = 0; i < nCellsWidth; i++)
  {
    for (int j = 0; j < nCellsHeight; j++)
    {
      if(factibleExtraccion(i,j,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)){
        float valorCelda = cellValueBase(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
        if(valorCelda>puntacionMaxima){
          imax = i;
          jmax = j;
          puntacionMaxima = valorCelda;
        }
      }
    }
    
  }
  (*currentDefense)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);
  ++currentDefense;
  float radius = (*currentDefense)->radio;
  while(currentDefense!=defenses.end()){
    for (size_t i = 0; i < nCellsWidth; i++){
      for (int j = 0; j < nCellsHeight; j++){
        if(factibleDefensa(i,j,radius,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)){
          float valorCelda = cellValueDefensa(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
            if(valorCelda>puntacionMaxima){
              imax = i;
              jmax = j;
              puntacionMaxima = valorCelda;
            }
        }
      }
    }
    (*currentDefense)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);
    imax = 0;
    jmax = 0;
    puntacionMaxima = -1;
    ++currentDefense;
    radius = (*currentDefense)->radio;
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
