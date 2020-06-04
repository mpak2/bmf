#!/bin/bash

for n in {0..100}
do
	cat clump/airfoil.json | ./bimorph clump/airfoil.sqlite
	sqlite3 clump/airfoil.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10";
	play -n synth 0.1 sine 440 vol 0.5
done

sqlite3 clump/airfoil.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC"
