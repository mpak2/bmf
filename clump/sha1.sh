#!/bin/bash

php ../bmf.php sha1
#while sleep 5; do sqlite3 clump/sha1.sqlite -header -column "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 19; SELECT AVG(perc) AS 'AVG(1000)' FROM mp_bmf_test WHERE id>(SELECT MAX(id)-1000 FROM mp_bmf_test)"; done
