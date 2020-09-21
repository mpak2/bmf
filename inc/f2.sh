#!/bin/bash

gcc mnist.cpp -lstdc++ -std=c++17 -lsqlite3 -o ./mnist && ./mnist mnist_test.csv 10
