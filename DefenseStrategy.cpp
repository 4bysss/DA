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
//Convertir celdas en posiciones del mapa
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
  
  if((posicion.x+radio)>=mapWidth||(posicion.x)<=0||(posicion.y+radio)>=mapHeight||(posicion.y)<=0){//===

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


//Funcion que calcula el valor de un par de celdas dadas para el centro de extraccion
float cellValueBase(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) 
{

  float mitadX = mapWidth/2;//Dividimos el tamaño del mapa a fin de localiza el centro del mismo
  float mitadY = mapHeight/2;

  float distanciaAObs = 0;
  float distanciaTotalObs = 0;

  float cellWidth = mapWidth / nCellsWidth;//Calculamos grosor y altura de las casillas 
  float cellHeight = mapHeight / nCellsHeight;

  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);//===

  float ditanciaCentro = distanciaEuclidea(v.x,v.y,mitadX,mitadY);//Calculamos la distancia que existe desde las coordenadas a el centro

  float radMin =distanciaEuclidea(v.x,v.y,(*obstacles.begin())->position.x,(*obstacles.begin())->position.y);//Variable usada a futuro

  for(auto it = obstacles.begin();it!=obstacles.end();it++){

    distanciaAObs=distanciaEuclidea(v.x,v.y,(*it)->position.x,(*it)->position.y);//==

    distanciaTotalObs+=distanciaAObs;//Calculamos las distancia total que separa esas coordenadas de todos los obstaculos por medio de un sumatorios

    radMin = std::min(distanciaAObs,radMin);//Obtenemos la distancia al obstaculo mas cercano 
  }

  return (0.7f*(1000/ditanciaCentro)+0.1f*(radMin)+0.2f*(1000/distanciaAObs));//Devolvemos el valor de la celda obtenido por medio del calculo de los pesos obtenidos mediante pruebas empiricas 
}



//Funcion que devuelve el valor de una celda dado un par de coordenadas para una defensa
float cellValueDefensa(int row, int col, bool **freeCells, int nCellsWidth,int nCellsHeight, float mapWidth, float mapHeight,List<Object *> obstacles, List<Defense *> defenses) {

  float posxBase = (*defenses.begin())->position.x;//Extraemos las coordenadas de la base
  float posyBase = (*defenses.begin())->position.y;//para mejorar la legibilidad del codigo

  float cellWidth = mapWidth / nCellsWidth;//===
  float cellHeight = mapHeight / nCellsHeight;

  Vector3 v = cellCenterToPosition(row,col,cellWidth,cellHeight);//===

  return 1000/distanciaEuclidea(v.x,v.y,posxBase,posyBase);//Dividimos mil entre la distancia a la base a fin de acercas las defensas lo mas posible a la base
  //NOTA:Se opto por diviidir entre 1000 para no perder decimales a la hora del calculo

}



void DEF_LIB_EXPORTED placeDefenses(bool **freeCells, int nCellsWidth, int nCellsHeight, float mapWidth,float mapHeight,std::list<Object *> obstacles,std::list<Defense *> defenses) {

  float cellWidth = mapWidth / nCellsWidth;//===
  float cellHeight = mapHeight / nCellsHeight;

  float puntacionMaxima = -1;//Valor virtualmente imposible

  float puntuaciones[nCellsHeight][nCellsWidth];//Matriz de valores para la base
  float puntuaciones2[nCellsHeight][nCellsWidth];//Matriz de valores para las defensas

  int imax,jmax;
  imax = 0;
  jmax = 0;
  int maxAttemps = 1000;

  List<Defense *>::iterator currentDefense = defenses.begin();//===

//Hacemos la primera mitad del algoritmo devorardor que se encargara de encontrar, bajo los criterios que hemos establecido
//la mejor posicion

//Rellenamos matriz de puntuacion para la base
  for (int i = 0; i < nCellsHeight; i++)
  {

    for (int j = 0; j < nCellsWidth; j++){

        puntuaciones[i][j] = cellValueBase(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);

      }

  }
bool colocado = false;//Semaforo que determinara si se ha colocado la base o no
while(!colocado){//Bucle que iterara hasta encontrar la mejor posicion para la base

  for (int i = 0; i < nCellsHeight; i++)
  {

    for (int j = 0; j < nCellsWidth; j++)
    {

        if(puntuaciones[i][j]>puntuaciones[imax][jmax]){//Localizamos el par de coordenadas cuya puntuacion es mas alta

          imax = i;
          jmax = j;

        }

    }

  }
  
  //Comprobamos si este par es valido para poner la base
  if(factibleExtraccion(imax,jmax,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)){

    (*currentDefense)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);//En caso de ser valido le asignamos a la base las coordenadas del par

    colocado = true;//Indicamos que la base ha sido colocada y que podemos salir del bucle

  }

  else{puntuaciones[imax][jmax]=-1;}//En caso de no ser factible le damos un valor virtualmente imposible de modo que no se valore de nuevo como opcion

}

//Comienza ahora la segunda parte del algoritmo devorador que en esencia tiene el mismo funcionamiento salvando pequeños cambios con respecto a la primera parte

currentDefense++;//Avanzamos a la primera defensa
  
  for (int i = 0; i < nCellsHeight; i++)//===
  {
    for (int j = 0; j < nCellsWidth; j++){
        puntuaciones2[i][j] = cellValueDefensa(i,j,freeCells,nCellsWidth,nCellsHeight,mapWidth,mapHeight,obstacles,defenses);
      }
    }




  for (auto it = currentDefense; it != defenses.end(); it++)//===
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

    if(factibleDefensa(imax,jmax,radius,cellWidth,cellHeight,mapWidth,mapHeight,obstacles,defenses)&&puntuaciones2[imax][jmax]!=-1)//===
    {

      (*it)->position = cellCenterToPosition(imax,jmax,cellWidth,cellHeight);

    }

  else{puntuaciones2[imax][jmax]=-1;it--;}//Ademas de lo que hacia en la parte uno esta ademas evita que el bucle for avance a la siguiente dado que la actual aun no se ha colocado
  
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
