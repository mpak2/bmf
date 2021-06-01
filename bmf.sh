#!/bin/bash
		
## sudo apt install libsqlite3-dev libdb5.3-stl-dev libdb5.3++-dev libacl1-dev # Устаревшие
# sudo apt install libmariadb3 libmariadbd-dev libhiredis-dev libboost-all-dev # Установка

#cat clump/dva.json | ./bimorph mysql://dva:dva@192.168.1.6/dva -c -
#./bimorph mysql://dva:dva@192.168.1.6/dva ds=1 epoch=100 learn #verbose=2

#cat clump/tri.json | ./bimorph mysql://tri:tri@192.168.1.6/tri -c -
#./bimorph mysql://tri:tri@192.168.1.6/tri ds=1 epoch=100 learn #verbose

#cat clump/chetire.json | ./bimorph mysql://chetire:chetire@192.168.1.6/chetire -c -
#./bimorph mysql://chetire:chetire@192.168.1.6/chetire ds=1 epoch=100 learn

#cat clump/iris_1.json | ./bimorph mysql://iris:iris@192.168.1.6/iris -c - # ?cache=shared
#cat clump/iris_2.json | ./bimorph mysql://iris:iris@192.168.1.6/iris - # ?cache=shared
#cat clump/iris_3.json | ./bimorph mysql://iris:iris@192.168.1.6/iris - # ?cache=shared

#./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared
#./bimorph mysql://iris:iris@192.168.1.6/iris ds=2 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared
#./bimorph mysql://iris:iris@192.168.1.6/iris ds=3 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared

cat clump/iris.json | ./bimorph mysql://iris:iris@192.168.1.6/iris -c - # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=100 learn #verbose=3 #key=+10 #-microtime # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris

#./bimorph mysql://mnist:mnist@192.168.1.6/mnist

#sudo mysql -u root mnist < ~/home/mpak/download/mnist/mnist.dump
#./bimorph mysql://mnist:mnist@192.168.1.6/mnist ds=1 epoch=100 learn

#./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"5.9","ШиринаЧашелистика":"3.0","ДлинаЛепестка":"5.1","ШиринаЛепестка":"1.8"}}]' #Iris-virginica
#./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлинаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлинаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлинаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлинаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлинаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлинаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлинаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлинаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

#./bimorph mysql://mnist:mnist@192.168.1.6/mnist ds=2 epoch=100 learn



