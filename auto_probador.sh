#!/bin/bash
cd ../p1
make
cd ../simulador
niveles=(1324 1625 1854 2356 2856 3456 3552 3728 4356 4592)
declare -i cont=1
for j in "${niveles[@]}"; do
	../simulador/simulador -level $j -verbose -time 999 | grep 'Base raided!'
	yes y | mv -i -- "game.ppm" game$cont.ppm
	let cont=cont+1
done
