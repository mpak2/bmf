#!/bin/bash

for n in {0..1000}
do
	sqlite3 clump/calc_test.sqlite "DELETE FROM mp_bmf_index;"
	cat clump/tri.json | ./bmf clump/calc_test.sqlite 50;
	sqlite3 clump/calc_test.sqlite -header -column "SELECT * FROM mp_bmf_test;"
done;
