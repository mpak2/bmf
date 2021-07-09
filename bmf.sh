#!/bin/bash
		
## sudo apt install libsqlite3-dev libdb5.3-stl-dev libdb5.3++-dev libacl1-dev # Устаревшие
# sudo apt install libmariadb3 libmariadbd-dev libhiredis-dev libboost-all-dev # Установка

#cat clump/dva.json | ./bimorph mysql://dva:dva@192.168.1.6/dva -c -
#./bimorph mysql://dva:dva@192.168.1.6/dva ds=1 epoch=100 learn #verbose=2
#./bimorph mysql://dva:dva@192.168.1.6/dva dano='[{"dano":{"2":"1", "1":"1"}}]'

#cat clump/tri.json | ./bimorph mysql://tri:tri@192.168.1.6/tri -c -
#./bimorph mysql://tri:tri@192.168.1.6/tri ds=1 epoch=100 learn #verbose
#./bimorph mysql://tri:tri@192.168.1.6/tri dano='[{"dano":{"3":"1", "2":"0", "1":"1"}}]' #Iris-virginica

#cat clump/chetire.json | ./bimorph mysql://chetire:chetire@192.168.1.6/chetire -c -
#./bimorph mysql://chetire:chetire@192.168.1.6/chetire ds=1 epoch=100 learn

#cat clump/iris_1.json | ./bimorph mysql://iris:iris@192.168.1.6/iris -c - # ?cache=shared
#cat clump/iris_2.json | ./bimorph mysql://iris:iris@192.168.1.6/iris - # ?cache=shared
#cat clump/iris_3.json | ./bimorph mysql://iris:iris@192.168.1.6/iris - # ?cache=shared

#./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared
#./bimorph mysql://iris:iris@192.168.1.6/iris ds=2 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared
#./bimorph mysql://iris:iris@192.168.1.6/iris ds=3 epoch=2 learn #data #verbose #key=+10 #-microtime # ?cache=shared

cat clump/iris.json | ./bimorph mysql://iris:iris@192.168.1.6/iris -c - # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=100 learn #itog=0 #verbose=3 #key=+10 #-microtime # ?cache=shared
./bimorph mysql://iris:iris@192.168.1.6/iris
./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"5.9","ШиринаЧашелистика":"3.0","ДлинаЛепестка":"5.1","ШиринаЛепестка":"1.8"}}]' #Iris-virginica

#./bimorph mysql://mnist:mnist@192.168.1.6/mnist
#./bimorph mysql://mnist:mnist@192.168.1.6/mnist ds=10 epoch=1 verbose=3 key=1080

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

#./bimorph mysql://binance:binance@192.168.1.6/binance
#./bimorph mysql://binance:binance@192.168.1.6/binance dano='[{"dano":{"-1":"0.169980","-10":"-2.030020","-11":"-2.980020","-12":"-4.910020","-13":"-8.520020","-14":"-7.960020","-15":"-7.110020","-16":"-10.170020","-17":"-12.830020","-18":"-5.190020","-19":"-5.570020","-2":"-1.020020","-20":"-6.560020","-21":"-4.370020","-22":"-5.430020","-23":"-2.520020","-24":"-5.450020","-25":"-0.330020","-26":"1.469980","-27":"1.609980","-28":"-0.510020","-29":"2.479980","-3":"-1.950020","-30":"3.929980","-31":"3.699980","-32":"0.749980","-33":"1.469980","-34":"-0.520020","-35":"-0.210020","-36":"-3.190020","-37":"-2.030020","-38":"-8.060020","-39":"-9.510020","-4":"-6.450020","-40":"-12.640020","-41":"-12.680020","-42":"-13.240020","-43":"-10.940020","-44":"-10.440020","-45":"-11.630020","-46":"-13.250020","-47":"-16.350020","-48":"-17.310020","-49":"-7.380020","-5":"-1.160020","-50":"-10.110020","-51":"-2.570020","-52":"-0.340020","-53":"5.889980","-54":"12.689980","-55":"13.139980","-56":"14.479980","-57":"12.259980","-58":"14.179980","-59":"10.239980","-6":"-1.150020","-60":"14.209980","-7":"-0.490020","-8":"-2.020020","-9":"-2.830020"}}]'
#date; ./bimorph mysql://mnist:mnist@192.168.1.6/mnist itog=1 verbose=3
#./bimorph mysql://binance:binance@192.168.1.6/binance ds=5 epoch=999 itog=0 learn verbose=3

