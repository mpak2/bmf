#!/bin/bash

mv clump/iris.sqlite /tmp

for n in {0..99}; do
	cat clump/iris.json | ./bimorph3 - clump/iris.sqlite -c -e 100;
	sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
done;

sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
