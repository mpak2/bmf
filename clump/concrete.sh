#!/bin/bash

for n in {0..100}
do
	cat clump/concrete.json | ./bimorph clump/concrete.sqlite
	sqlite3 clump/concrete.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10";
	play -n synth 0.1 sine 880 vol 0.5
done

sqlite3 clump/concrete.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 22"
