#!/bin/bash

#apt-get install libncurses5-dev

clear
echo "=== Сборка bmf ==="
# g++ cparse/builtin-features.o cparse/shunting-yard.o bmf.cpp -lstdc++ -Wreturn-local-addr -lsqlite3 -Wwrite-strings -std=c++11 -lm -o bmf && ./bmf
# json='{"dano":{"1":{"Один":"1","Два":"0","Три":"1"},"2":{"id":"2","name":"Проверка2"}},"itog":{"1":{"Один":"0","Два":"1"},"2":{"Один":"0","Два":"0"}}}';
gcc bmf.cpp -O0 -lstdc++ -Wreturn-local-addr -lsqlite3 -std=c++17 -lstdc++fs -lm -lpthread -ldb_stl -o bimorph7 && bash bmf.sh
#for n in {1..10}; do echo "123"; done;
#for n in {1..10}; do echo "123"; done;
#pause 10; for n in {0..10}; do cat json/tri.json | ./bmf tri; done;
#cat json/iris.json | ./bmf iris

#echo "=== Запуск bmf ==="
#./bmf 7;
