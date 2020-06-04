#!/bin/bash

#for n in {0..20}
#do
	php binance/api.php candles;
	php binance/api.php prediction;
	php binance/api.php json
	cat binance/binance.json |	./bimorph clump/binance.sqlite
#done
#cat binance/binance.json | ./bmf clump/binance.sqlite
