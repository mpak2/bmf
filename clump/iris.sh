#!/bin/bash

rm clump/iris/*

cat clump/iris_1.json | ./bimorph clump/iris -
cat clump/iris_2.json | ./bimorph clump/iris -
cat clump/iris_3.json | ./bimorph clump/iris -

#mv clump/iris.sqlite /tmp;
#sqlite3 clump/iris.sqlite "DELETE FROM mp_bmf_index";
#sqlite3 clump/iris.sqlite "DELETE FROM mp_bmf_test";
#sqlite3 clump/iris.sqlite "UPDATE mp_bmf_itog SET index_id=NULL";
#for n in {0..50};
#do
	#cp ./bmf ./bimorph
#for n in {0..100}; do
#	cat clump/iris.json | ./bmf - clump/iris.sqlite -r 0 -e 100;
#	sqlite3 clump/iris.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10";
#done;
#done;
