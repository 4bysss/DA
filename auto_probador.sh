#!/bin/bash
cd ../p2
make
cd ../simulador
niveles=(3124 3125 3054 3056 3056 3056 3052 3028 4056 4092)
declare -a pases=(0.5 0.6 0.4 0.5 0.7 1 0.5 0.6 0.3 0.5)
declare -a defensas=(30 40 40 50 40 50 40 60 70 80)
declare -a tipos=(30 40 30 40 20 50 40 50 70 70)
declare -a aleatoriedad=(0.5 0.6 0.4 0.5 0.7 0.5 0.5 0.6 0.3 0.5)
length=${#niveles[@]}
declare -i cont=0
for (( j=0; j<length; j++ ));
 do
	../simulador/simulador -level ${niveles[$j]} -pases ${pases[$j]} -defenses ${defensas[$j]} -dt ${tipos[$j]} -dr ${aleatoriedad[$j]}  -verbose -time 999 | grep 'Base raided!'

	let cont=cont+1
done



#Quitar comentarios para probar P1
#niveles=(1324 1625 1854 2356 2856 3456 3552 3728 4356 4592)
#declare -i cont=1
#for j in "${niveles[@]}"; do
#	../simulador/simulador -level $j -verbose -time 999 | grep 'Base raided!'
#	yes y | mv -i -- "game.ppm" game$cont.ppm
#	let cont=cont+1
#done
