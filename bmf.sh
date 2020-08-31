#!/bin/bash

#sudo apt install libsqlite3-dev

#cat clump/dva.json | ./bimorph - -c -e 10 -r 0 ./2
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"

cat clump/tri.json | ./bimorph - -c -e 100 -r 0 ./3
sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"
#./bimorph ./3 -j '[{"dano":{"Один":"0", "Два":"0", "Три":"0"}}]'

#cat clump/iris.json | ./bimorph - clump/iris.sqlite -c -e 100
#sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
#sqlite3 clump/iris.sqlite -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris.sqlite -j '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'

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
	cat clump/tri.json | ./bimorph - 3 -r 0 -e 100
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
cat clump/dva.json | ./bimorph - ./2 -e 5;
if [ -f "./2" ]; then
	sqlite3 2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as change, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
fi
EOF

