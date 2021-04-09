#!/bin/bash

## sudo apt install libsqlite3-dev
## sudo apt install libdb5.3-stl-dev
## sudo apt install libdb5.3++-dev
## sudo apt install libacl1-dev

# sudo apt install libmariadb3
# sudo apt install libmariadbd-dev
# sudo apt install libhiredis-dev
# sudo apt install libboost-all-dev

#cat clump/dva.json | ./bimorph mysql://dva:dva@localhost/dva -c -
#./bimorph mysql://dva:dva@localhost/dva ds=1 epoch=100 learn

#cat clump/tri.json | ./bimorph mysql://tri:tri@localhost/tri -c -
#./bimorph mysql://tri:tri@localhost/tri ds=1 epoch=1000 learn

./bimorph mysql://iris:iris@192.168.1.6/iris -c
#cat clump/iris_1.json | ./bimorph mysql://localhost/iris - # ?cache=shared
#cat clump/iris_2.json | ./bimorph mysql://localhost/iris - # ?cache=shared
#cat clump/iris_3.json | ./bimorph mysql://localhost/iris - # ?cache=shared
cat clump/iris.json | ./bimorph mysql://iris:iris@192.168.1.6/iris - # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=100 learn #verbose=3 #-microtime # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris

#	#sqlite3 clump/iris -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

