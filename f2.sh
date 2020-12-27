#!/bin/bash

#apt-get install libncurses5-dev

clear
echo "=== Сборка bmf ==="
gcc bmf.cpp -O0 -lstdc++ -Wreturn-local-addr -lsqlite3 -std=c++17 -lstdc++fs -lm -lpthread -ldb_stl -lmariadb -o bimorph9 && bash bmf.sh
#gcc inc/mariadb.cpp -O0 -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -lpthread -lmariadb -o inc/mariadb && ./inc/mariadb
#for n in {1..10}; do echo "123"; done;
#for n in {1..10}; do echo "123"; done;
#pause 10; for n in {0..10}; do cat json/tri.json | ./bmf tri; done;
#cat json/iris.json | ./bmf iris

#echo "=== Запуск bmf ==="
#./bmf 7;
