#!/bin/bash

#gcc csv2json.cpp -lstdc++ -std=c++17 -lsqlite3 -o csv2json && ./csv2json mnist_train.csv label 6000

#gcc submission.cpp -lstdc++ -std=c++17 -lsqlite3 -o ./submission && ./submission test.csv overfit.sqlite target 4 1

#gcc inc/mariadb.cpp -O0 -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -lpthread -lmariadb -o inc/mariadb && ./inc/mariadb

#gcc redis.cpp -O0 -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -lpthread -lhiredis -o redis && ./redis
gcc sycl.cpp -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -o sycl && ./sycl
