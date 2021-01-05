#!/bin/bash

#sudo apt install libsqlite3-dev
#sudo apt install libdb5.3-stl-dev
#sudo apt install libdb5.3++-dev
#sudo apt install libmariadbd-dev
#sudo apt install libhiredis-dev
#sudo apt install libacl1-dev

#./bimorph numeric dano={"Прогноз":{"dano":{"num1_0":"3","num1_1":"9","num1_10":"0","num1_11":"2","num1_12":"3","num1_13":"7","num1_14":"5","num1_15":"6","num1_16":"6","num1_17":"9","num1_18":"2","num1_19":"7","num1_2":"7","num1_20":"3","num1_21":"0","num1_22":"1","num1_23":"1","num1_24":"9","num1_25":"9","num1_26":"7","num1_27":"8","num1_28":"3","num1_29":"5","num1_3":"4","num1_30":"4","num1_31":"4","num1_32":"3","num1_33":"6","num1_34":"1","num1_35":"6","num1_36":"8","num1_37":"3","num1_38":"6","num1_39":"2","num1_4":"3","num1_40":"8","num1_41":"8","num1_42":"6","num1_43":"4","num1_44":"0","num1_45":"4","num1_46":"7","num1_47":"2","num1_48":"4","num1_49":"1","num1_5":"8","num1_50":"1","num1_51":"3","num1_52":"1","num1_53":"4","num1_54":"4","num1_55":"7","num1_56":"2","num1_57":"1","num1_58":"6","num1_59":"2","num1_6":"3","num1_60":"0","num1_61":"6","num1_62":"8","num1_63":"9","num1_64":"7","num1_65":"1","num1_66":"8","num1_67":"0","num1_68":"8","num1_69":"5","num1_7":"1","num1_70":"6","num1_71":"0","num1_72":"0","num1_73":"0","num1_74":"0","num1_75":"0","num1_76":"5","num1_77":"0","num1_78":"0","num1_8":"9","num1_9":"7","num2_0":"3","num2_1":"0","num2_10":"9","num2_11":"2","num2_12":"9","num2_13":"7","num2_14":"6","num2_15":"3","num2_16":"5","num2_17":"6","num2_18":"2","num2_19":"0","num2_2":"1","num2_20":"1","num2_21":"1","num2_22":"1","num2_23":"7","num2_24":"3","num2_25":"5","num2_26":"5","num2_27":"6","num2_28":"4","num2_29":"3","num2_3":"6","num2_30":"5","num2_31":"8","num2_32":"7","num2_33":"7","num2_34":"3","num2_35":"2","num2_36":"3","num2_37":"0","num2_38":"3","num2_39":"4","num2_4":"6","num2_40":"0","num2_41":"5","num2_42":"4","num2_43":"9","num2_44":"1","num2_45":"6","num2_46":"4","num2_47":"2","num2_48":"9","num2_49":"6","num2_5":"6","num2_50":"5","num2_51":"4","num2_52":"7","num2_53":"7","num2_54":"9","num2_55":"8","num2_56":"2","num2_57":"6","num2_58":"7","num2_59":"1","num2_6":"5","num2_60":"0","num2_61":"7","num2_62":"7","num2_63":"2","num2_64":"6","num2_65":"7","num2_66":"1","num2_67":"1","num2_68":"8","num2_69":"5","num2_7":"6","num2_70":"6","num2_71":"1","num2_72":"2","num2_73":"1","num2_74":"8","num2_75":"9","num2_76":"7","num2_77":"0","num2_78":"0","num2_8":"2","num2_9":"8"}}}
#cat clump/crc8.json | ./bimorph - clump/crc8
#valgrind --leak-check=full --show-leak-kinds=all ./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]';
#valgrind --leak-check=full --show-leak-kinds=all cat clump/iris.json | ./bimorph - clump/iris -c -epoch 100;
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]';
#./bimorph clump/iris -epoch 1

#./bimorph ~/download/house-prices-advanced-regression-techniques/house -dano '{"1461":{"dano":{"1stFlrSF(Первый этаж квадратных футов)":"896","2ndFlrSF(Второй этаж квадратных футов)":"0","3SsnPorch(три сезона крыльцо в квадратных футах)":"0","Alley(Тип доступа к аллее)":"NA","BedroomAbvGr(Количество спален выше уровня подвала)":"2","BldgType(Тип жилья)":"1Fam","BsmtCond(общее состояние подвала)":"TA","BsmtExposure(Подвал или подвал на уровне сада)":"No","BsmtFinSF1(тип 1 готовые квадратные футы)":"468","BsmtFinSF2(Тип 2 готовые квадратные футы)":"144","BsmtFinType1(качество подвального помещения)":"Rec","BsmtFinType2(качество второй готовой области)":"LwQ","BsmtFullBath(Подвал полные ванные комнаты)":"0","BsmtHalfBath(Подвальная половина ванных комнат)":"0","BsmtQual(высота подвала)":"TA","BsmtUnfSF(Незаконченные квадратные футы подвального помещения)":"270","CentralAir(центральное кондиционирование)":"Y","Condition1(Близость к главной дороге или железной дороге)":"Feedr","Condition2(Близость к главной дороге или железной дороге)":"Norm","Electrical(Электрическая система)":"SBrkr","EnclosedPorch(Закрытая площадь крыльца в квадратных футах)":"0","ExterCond(текущее состояние материала снаружи)":"TA","ExterQual(качество наружных материалов)":"TA","Exterior1st(Внешнее покрытие дома)":"VinylSd","Exterior2nd(Внешнее покрытие дома)":"VinylSd","Fence(качество забора)":"MnPrv","FireplaceQu(качество камина)":"NA","Fireplaces(Количество каминов)":"0","Foundation(Тип основания)":"CBlock","FullBath(полные ванные комнаты выше класса)":"1","Functional(Домашний рейтинг функциональности)":"Typ","GarageArea(размер гаража в квадратных футах)":"730","GarageCars(размер гаража в вместимости автомобиля)":"1","GarageCond(Состояние гаража)":"TA","GarageFinish(внутренняя отделка гаража)":"Unf","GarageQual(качество гаража)":"TA","GarageType(расположение гаража)":"Attchd","GarageYrBlt(Год гараж был построен)":"1961","GrLivArea(Квадратные футы жилой площади над землей)":"896","HalfBath(половина ванны выше класса)":"0","Heating(Тип отопления)":"GasA","HeatingQC(качество и состояние отопления)":"TA","HouseStyle(Стиль жилья)":"1Story","KitchenAbvGr(Количество кухонь)":"1","KitchenQual(качество кухни)":"TA","LandContour(плоскостность недвижимости)":"Lvl","LandSlope(Склон собственности)":"Gtl","LotArea(размер лота в квадратных футах)":"11622","LotConfig(конфигурация лота)":"Inside","LotFrontage(Линейные ноги улицы связанные с собственностью)":"80","LotShape(общая форма собственности)":"Reg","LowQualFinSF(Низкое качество готовых квадратных футов)":"0","MSSubClass(класс здания)":"20","MSZoning(общая классификация зонирования)":"RH","MasVnrArea(площадь шпона в квадратных футах)":"0","MasVnrType(Кирпичный тип шпона)":"None","MiscFeature(Разная особенность не включенная в другие категории)":"NA","MiscVal(Значение разных функций)":"0","MoSold(месяц продан)":"6","Neighborhood(Физические местоположения в пределах города Эймса)":"NAmes","OpenPorchSF(площадь открытого крыльца в квадратных футах)":"0","OverallCond(Общий рейтинг состояния)":"6","OverallQual(общий материал и качество отделки)":"5","PavedDrive(асфальтированная дорога)":"Y","PoolArea(площадь бассейна в квадратных футах)":"0","PoolQC(качество бассейна)":"NA","RoofMatl(материал крыши)":"CompShg","RoofStyle(Тип крыши)":"Gable","SaleCondition(условия продажи)":"Normal","SaleType(Тип продажи)":"WD","ScreenPorch(площадь крыльца экрана в квадратных футах)":"120","Street(Тип подъездной дороги)":"Pave","TotRmsAbvGrd(Общее количество комнат выше класса)":"5","TotalBsmtSF(Общая площадь подвала)":"882","Utilities(Тип доступных утилит)":"AllPub","WoodDeckSF(площадь деревянного настила в квадратных футах)":"140","YearBuilt(первоначальная дата строительства)":"1961","YearRemodAdd(Изменить дату)":"1961","YrSold(год продан)":"2010"}}}'
#cat clump/house.json | ./bimorph - clump/house -epoch 100 #2>/dev/null

#mv ./2 /tmp
#cat clump/dva.json | ./bimorph - -c ./2 -epoch 10
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"

#cat clump/tri.json | ./bimorph - ./3
#sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"
##./bimorph ./3 -j '[{"dano":{"Один":"0", "Два":"0", "Три":"0"}}]'


#mv clump/iris /tmp

#mv -f clump/iris/* /tmp/
#cat clump/iris.json | ./bimorph iris - -c
#./bimorph iris ds=495e7de50abef9a66166cc5ea4b6927e epoch=100 # ?cache=shared

./bimorph redis://localhost/iris -c
#./bimorph redis://localhost/iris
#cat clump/iris_1.json | ./bimorph redis://localhost/iris -
#cat clump/iris_2.json | ./bimorph redis://localhost/iris -
#cat clump/iris_3.json | ./bimorph redis://localhost/iris -

cat clump/iris.json | ./bimorph redis://localhost/iris - # ?cache=shared
./bimorph redis://localhost/iris ds=495e7de50abef9a66166cc5ea4b6927e epoch=100 # ?cache=shared

#for loop in {1..10}; do
#	echo "Поток $loop"
#	./bimorph redis://localhost/iris ds=21e18b82ea50b431847639eeac729651 epoch=100 # ?cache=shared
#	./bimorph redis://localhost/iris ds=fc8c9622e85a712e109eb6993bdfff95 epoch=100 # ?cache=shared
#	./bimorph redis://localhost/iris ds=425146b12b837f025e1f7ebdd78b8561 epoch=100 # ?cache=shared
#done
./bimorph redis://localhost/iris; echo "\n" # ?cache=shared

#./bimorph clump/iris ds=1 mem=:memory: itog=2 epoch=1 # ?cache=shared
#./bimorph clump/iris ds=1 mem=:memory: itog=3 epoch=1 # ?cache=shared
#./bimorph clump/iris ds=1 mem=:memory: itog=1 epoch=1 # ?cache=shared
#./bimorph clump/iris ds=1 mem=:memory: itog=2 epoch=1 # ?cache=shared
#./bimorph clump/iris ds=1 mem=:memory: itog=3 epoch=1 # ?cache=shared
#./bimorph clump/iris ds=2 mem=:memory: itog=2 epoch=1 # ?cache=shared
#./bimorph iris # ?cache=shared

#sqlite3 clump/iris "DELETE FROM mp_bmf_dataset";
#sqlite3 clump/iris "SELECT COUNT(*) FROM mp_bmf_dataset";
#sqlite3 clump/iris "SELECT COUNT(*) FROM mp_bmf_dataset_map";
#./bimorph clump/iris ds=2 mem=:memory: epoch=100 # ?cache=shared
#./bimorph clump/iris ds=2 mem=:memory: epoch=100 # ?cache=shared

#./bimorph clump/iris ds=2 itog=1 epoch=100 # ?cache=shared
#./bimorph clump/iris ds=2 itog=2 epoch=100 # ?cache=shared
#./bimorph clump/iris ds=2 itog=3 epoch=100 # ?cache=shared
#./bimorph clump/iris -ds 3 -epoch 100 # ?cache=shared
#./bimorph clump/iris -ds 1 -epoch 100 # ?cache=shared
#./bimorph clump/iris -ds 2 -epoch 100 # ?cache=shared
#./bimorph clump/iris -ds 3 -epoch 100 # ?cache=shared
#cat clump/iris.json | ./bimorph - clump/iris -c -rand 1 -epoch 100 # ?cache=shared
#sqlite3 clump/iris -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; SELECT COUNT(*) AS count, MIN(duration) AS min_duration, AVG(duration) as avg_duration, MAX(duration) AS max_duration, AVG('index') as avg_bmf FROM mp_bmf_test;"

#	#sqlite3 clump/iris -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph clump/iris -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

#cat clump/crc8.json | ./bimorph - clump/crc8 -c -rand 0 -thread 8
#sqlite clump/crc8 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#cat clump/house.json | ./bimorph - clump/house -c -rand 0 -ocl 1
#sqlite clump/house -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#./bimorph -dano '[{"dano":{"Один":"0", "Два":"0"}}]' ./2
#rm /tmp/3; cat /tmp/tri.json | ./bmf - -rand 0 /tmp/3

