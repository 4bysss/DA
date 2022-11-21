// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1               
#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include <map>
using namespace Asedio;
float valueGenerator(Defense* defensa){return (1/(defensa->cost))*0.5f+
                    (defensa->damage+(1/defensa->dispersion)+defensa->attacksPerSecond+defensa->range+defensa->health);}
void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {

    unsigned int cost = 0;


//No picha *Introduza escusa generica*

    std::list<Defense*>::iterator it = defenses.begin();

    int costeBase = (*it)->cost;
    float valoresMax[defenses.size()][ases];



    ++it;
    Defense* refsDef[defenses.size()-1];
    int i = 0;
    while(it!=defenses.end()){
        refsDef[i]=*it;
        ++i;
        ++it;
    }
    it = defenses.begin();
    ++it;
    int numDefensas = defenses.size()-1;
    int costeSinB = ases - costeBase;


    //Llenamos la primera columna
    for ( i = 0; i < numDefensas; i++)
    {
        valoresMax[i][0] = 0;
    }


    //Llenamos la primera fila
    for (int i = 0; i < costeSinB; i++)
    {
        if(i<refsDef[0]->cost){
            valoresMax[0][i] = 0;
        }
        else{
            valoresMax[0][i] = valueGenerator(refsDef[0]);
        }
    }
    

    //Llenamos el resto de la matriz
    for (int i = 1; i < numDefensas ; i++)
    {
        for (int j = 1; j < costeSinB; j++)
        {
            if(j<refsDef[i]->cost){
                valoresMax[i][j]=valoresMax[i-1][j];
            }
            else
            {
              valoresMax[i][j]  = std::max(valoresMax[i-1][j],valoresMax[i-1][j-refsDef[i]->cost] + valueGenerator(refsDef[i]));
            }
        }
        
    }
    selectedIDs.push_back((*defenses.begin())->id);
    int pos = costeSinB -1;
    for (int i = numDefensas-1; i > 0; i--)
    {
        if(valoresMax[i][pos]!=valoresMax[i-1][pos]){
            selectedIDs.push_back(refsDef[i]->id);
            pos = pos - refsDef[i]->cost;
        }
        
    }
    
    
    /*while(it != defenses.end()) {
        if(cost + (*it)->cost <= ases) {
            selectedIDs.push_back((*it)->id);
            cost += (*it)->cost;
        }
        ++it;
    }*/
}
