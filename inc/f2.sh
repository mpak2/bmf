#!/bin/bash

#gcc csv2json.cpp -lstdc++ -std=c++17 -o csv2json && ./csv2json orange_small_churn_train_data.csv labels
#gcc submission.cpp -lstdc++ -std=c++17 -lsqlite3 -o ./submission && ./submission test.csv overfit.sqlite target 4 1
#gcc inc/mariadb.cpp -O0 -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -lpthread -lmariadb -o inc/mariadb && ./inc/mariadb
#gcc redis.cpp -O0 -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -lpthread -lhiredis -o redis && ./redis
#gcc sycl.cpp -lstdc++ -Wreturn-local-addr -std=c++17 -lstdc++fs -lm -o sycl && ./sycl
#gcc sycl.cpp -lstdc++ -std=c++17 -o sycl && ./sycl
#screen -c iris.screen
gcc -Wdeprecated-declarations opencl.cpp -std=c++17 -lstdc++ -lOpenCL -o opencl && ./opencl
