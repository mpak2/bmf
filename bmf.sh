#!/bin/bash

#sudo apt install libsqlite3-dev

#cat clump/dva.json | ./bimorph - ./2 -c -epoch 10 -r 0
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"

#cat clump/tri.json | ./bimorph - ./3 -c -rand 1 -epoch 100
#sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"
##./bimorph ./3 -j '[{"dano":{"Один":"0", "Два":"0", "Три":"0"}}]'

cat clump/iris.json | ./bimorph - clump/iris.sqlite -c -epoch 100
sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; SELECT COUNT(*) AS count, MIN(duration) AS min_duration, AVG(duration) as avg_duration, MAX(duration) AS max_duration, AVG(bmf) as avg_bmf FROM mp_bmf_test;"

#	#sqlite3 clump/iris.sqlite -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"1.4","ШиринаЛепестка":"0.2"}}]'
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"5.7","ШиринаЧашелистика":"2.8","ДлиннаЛепестка":"4.1","ШиринаЛепестка":"1.3"}}]'
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"5.9","ШиринаЧашелистика":"3.0","ДлиннаЛепестка":"5.1","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"5.7","ШиринаЧашелистика":"2.8","ДлиннаЛепестка":"4.1","ШиринаЛепестка":"1.3"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"1.4","ШиринаЛепестка":"0.2"}},
#											{"dano":{"ДлиннаЧашелистика":"5.1","ШиринаЧашелистика":"3.8","ДлиннаЛепестка":"1.5","ШиринаЛепестка":"0.3"}}]'

#cat clump/crc8.json | ./bimorph - clump/crc8.sqlite -c -r 0 -t 8
#sqlite clump/crc8.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#cat clump/house.json | ./bimorph - clump/house.sqlite -c -r 0 -ocl 1
#sqlite clump/house.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#mv ./3 /tmp
#echo '[{"dano":{"Один":"0", "Два":"0", "Три":"0"},"itog":{"Два":"0"}}]' | ./bmf - ./3
#echo '[{"dano":{"Один":"1", "Два":"0", "Три":"0"},"itog":{"Два":"1"}}]' | ./bmf - ./3
#echo '[{"dano":{"Один":"0", "Два":"1", "Три":"0"},"itog":{"Два":"1"}}]' | ./bmf - ./3
#echo '[{"dano":{"Один":"0", "Два":"0", "Три":"1"},"itog":{"Два":"1"}}]' | ./bmf - ./3
#echo '[{"dano":{"Один":"0", "Два":"0", "Три":"0"},"itog":{"Два":"0"}}]' | ./bmf - ./3

#./bimorph -j '[{"dano":{"Один":"0", "Два":"0"}}]' ./2
#rm /tmp/3; cat /tmp/tri.json | ./bmf - -r 0 /tmp/3

<< EOF
if [ -f "./3" ]; then
	sqlite3 ./3 "DELETE FROM mp_bmf_dano; DELETE FROM mp_bmf_dano_values; DELETE FROM mp_bmf_dano_titles;"
	sqlite3 ./3 "UPDATE mp_bmf_itog SET index_id=NULL";
	sqlite3 ./3 "DELETE FROM mp_bmf_index;"
fi
	cat clump/tri.json | ./bimorph - 3 -r 0 -epoch 100
if [ -f "./3" ]; then
	sqlite3 3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
fi
EOF

<< EOF
if [ -f "./2" ]; then
	sqlite3 ./2 "DELETE FROM mp_bmf_index;"
	sqlite3 ./2 "DELETE FROM mp_bmf_dano; DELETE FROM mp_bmf_dano_values; DELETE FROM mp_bmf_dano_titles;"
	sqlite3 ./2 "UPDATE SQLITE_SEQUENCE SET SEQ=0 WHERE NAME='mp_bmf_dano';"
	sqlite3 ./2 "DELETE FROM mp_bmf_itog; DELETE FROM mp_bmf_itog_values; DELETE FROM mp_bmf_itog_titles;"
	sqlite3 ./2 "UPDATE SQLITE_SEQUENCE SET SEQ=0 WHERE NAME='mp_bmf_index';"
fi
cat clump/dva.json | ./bimorph - ./2 -epoch 5;
if [ -f "./2" ]; then
	sqlite3 2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as change, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
fi
EOF

