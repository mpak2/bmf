#!/bin/bash

#sudo apt install libsqlite3-dev
#sudo apt install libdb5.3-stl-dev
#sudo apt install libdb5.3++-dev
#sudo apt install libhiredis-dev
#sudo apt install libacl1-dev
#sudo apt install libmariadbd-dev

#cat clump/dva.json | ./bimorph mysql://localhost/dva -c -
#./bimorph mysql://localhost/dva ds=f43e8e4b2864a93704048a1eaef6c374 epoch=1000 learn

#cat clump/tri.json | ./bimorph mysql://localhost/tri -c -
#./bimorph mysql://localhost/tri ds=83989155d7b38929265afecb9f2188df epoch=100 learn

cat clump/iris.json | ./bimorph mysql://localhost/iris -c - # ?cache=shared
./bimorph mysql://localhost/iris ds=495e7de50abef9a66166cc5ea4b6927e epoch=0 learn #-microtime # ?cache=shared

#	#sqlite3 clump/iris -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

