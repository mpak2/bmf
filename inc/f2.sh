#!/bin/bash

gcc csv2json.cpp -lstdc++ -std=c++17 -lsqlite3 -o ./csv2json && ./csv2json train_features.csv cp_time,cp_dose 0 sig_id,cp_type
