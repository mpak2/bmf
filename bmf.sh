#!/bin/bash

#sudo apt install libsqlite3-dev

#./bimorph -server 53000
#cat clump/tri.json | ./bimorph - ./3; sqlite3 ./3 "SELECT * FROM mp_bmf_index"

#mv ./2 /tmp
#cat clump/dva.json | ./bimorph - ./2
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"

#cat clump/tri.json | ./bimorph - ./3
#sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"
##./bimorph ./3 -j '[{"dano":{"Один":"0", "Два":"0", "Три":"0"}}]'

#mv clump/iris.sqlite /tmp/
cat clump/iris.json | ./bimorph - clump/iris.sqlite -c -rand 1 -epoch 100
#cat clump/iris.json | ./bimorph - clump/iris.sqlite
sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; SELECT COUNT(*) AS count, MIN(duration) AS min_duration, AVG(duration) as avg_duration, MAX(duration) AS max_duration, AVG(bmf) as avg_bmf FROM mp_bmf_test;"

#	#sqlite3 clump/iris.sqlite -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

#cat clump/crc8.json | ./bimorph - clump/crc8.sqlite -c -rand 0 -thread 8
#sqlite clump/crc8.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#cat clump/house.json | ./bimorph - clump/house.sqlite -c -rand 0 -ocl 1
#sqlite clump/house.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#./bimorph -dano '[{"dano":{"Один":"0", "Два":"0"}}]' ./2
#rm /tmp/3; cat /tmp/tri.json | ./bmf - -rand 0 /tmp/3

