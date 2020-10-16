#!/bin/bash

gcc csv2json.cpp -lstdc++ -std=c++17 -lsqlite3 -o csv2json && ./csv2json mnist_train.csv label 0
#gcc submission.cpp -lstdc++ -std=c++17 -lsqlite3 -o ./submission && ./submission test.csv overfit.sqlite target 4 1
