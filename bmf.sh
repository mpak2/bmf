#!/bin/bash

#sudo apt install libsqlite3-dev
#cat clump/mnist.json | ./bimorph - clump/mnist.sqlite

#./bimorph ~/download/house-prices-advanced-regression-techniques/house.sqlite -dano '{"1461":{"dano":{"1stFlrSF(Первый этаж квадратных футов)":"896","2ndFlrSF(Второй этаж квадратных футов)":"0","3SsnPorch(три сезона крыльцо в квадратных футах)":"0","Alley(Тип доступа к аллее)":"NA","BedroomAbvGr(Количество спален выше уровня подвала)":"2","BldgType(Тип жилья)":"1Fam","BsmtCond(общее состояние подвала)":"TA","BsmtExposure(Подвал или подвал на уровне сада)":"No","BsmtFinSF1(тип 1 готовые квадратные футы)":"468","BsmtFinSF2(Тип 2 готовые квадратные футы)":"144","BsmtFinType1(качество подвального помещения)":"Rec","BsmtFinType2(качество второй готовой области)":"LwQ","BsmtFullBath(Подвал полные ванные комнаты)":"0","BsmtHalfBath(Подвальная половина ванных комнат)":"0","BsmtQual(высота подвала)":"TA","BsmtUnfSF(Незаконченные квадратные футы подвального помещения)":"270","CentralAir(центральное кондиционирование)":"Y","Condition1(Близость к главной дороге или железной дороге)":"Feedr","Condition2(Близость к главной дороге или железной дороге)":"Norm","Electrical(Электрическая система)":"SBrkr","EnclosedPorch(Закрытая площадь крыльца в квадратных футах)":"0","ExterCond(текущее состояние материала снаружи)":"TA","ExterQual(качество наружных материалов)":"TA","Exterior1st(Внешнее покрытие дома)":"VinylSd","Exterior2nd(Внешнее покрытие дома)":"VinylSd","Fence(качество забора)":"MnPrv","FireplaceQu(качество камина)":"NA","Fireplaces(Количество каминов)":"0","Foundation(Тип основания)":"CBlock","FullBath(полные ванные комнаты выше класса)":"1","Functional(Домашний рейтинг функциональности)":"Typ","GarageArea(размер гаража в квадратных футах)":"730","GarageCars(размер гаража в вместимости автомобиля)":"1","GarageCond(Состояние гаража)":"TA","GarageFinish(внутренняя отделка гаража)":"Unf","GarageQual(качество гаража)":"TA","GarageType(расположение гаража)":"Attchd","GarageYrBlt(Год гараж был построен)":"1961","GrLivArea(Квадратные футы жилой площади над землей)":"896","HalfBath(половина ванны выше класса)":"0","Heating(Тип отопления)":"GasA","HeatingQC(качество и состояние отопления)":"TA","HouseStyle(Стиль жилья)":"1Story","KitchenAbvGr(Количество кухонь)":"1","KitchenQual(качество кухни)":"TA","LandContour(плоскостность недвижимости)":"Lvl","LandSlope(Склон собственности)":"Gtl","LotArea(размер лота в квадратных футах)":"11622","LotConfig(конфигурация лота)":"Inside","LotFrontage(Линейные ноги улицы связанные с собственностью)":"80","LotShape(общая форма собственности)":"Reg","LowQualFinSF(Низкое качество готовых квадратных футов)":"0","MSSubClass(класс здания)":"20","MSZoning(общая классификация зонирования)":"RH","MasVnrArea(площадь шпона в квадратных футах)":"0","MasVnrType(Кирпичный тип шпона)":"None","MiscFeature(Разная особенность не включенная в другие категории)":"NA","MiscVal(Значение разных функций)":"0","MoSold(месяц продан)":"6","Neighborhood(Физические местоположения в пределах города Эймса)":"NAmes","OpenPorchSF(площадь открытого крыльца в квадратных футах)":"0","OverallCond(Общий рейтинг состояния)":"6","OverallQual(общий материал и качество отделки)":"5","PavedDrive(асфальтированная дорога)":"Y","PoolArea(площадь бассейна в квадратных футах)":"0","PoolQC(качество бассейна)":"NA","RoofMatl(материал крыши)":"CompShg","RoofStyle(Тип крыши)":"Gable","SaleCondition(условия продажи)":"Normal","SaleType(Тип продажи)":"WD","ScreenPorch(площадь крыльца экрана в квадратных футах)":"120","Street(Тип подъездной дороги)":"Pave","TotRmsAbvGrd(Общее количество комнат выше класса)":"5","TotalBsmtSF(Общая площадь подвала)":"882","Utilities(Тип доступных утилит)":"AllPub","WoodDeckSF(площадь деревянного настила в квадратных футах)":"140","YearBuilt(первоначальная дата строительства)":"1961","YearRemodAdd(Изменить дату)":"1961","YrSold(год продан)":"2010"}}}'
#cat clump/house.json | ./bimorph - clump/house.sqlite -epoch 100 #2>/dev/null

#mv ./2 /tmp
#cat clump/dva.json | ./bimorph - ./2
#sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./2 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"

#cat clump/tri.json | ./bimorph - ./3
#sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"
##sqlite3 ./3 -column -header "SELECT * FROM mp_bmf_index ORDER BY id DESC"
##./bimorph ./3 -j '[{"dano":{"Один":"0", "Два":"0", "Три":"0"}}]'

cat clump/iris.json | ./bimorph - clump/iris.sqlite -c -rand 1 -epoch 100 # ?cache=shared
sqlite3 clump/iris.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; SELECT COUNT(*) AS count, MIN(duration) AS min_duration, AVG(duration) as avg_duration, MAX(duration) AS max_duration, AVG('index') as avg_bmf FROM mp_bmf_test;"

#	#sqlite3 clump/iris.sqlite -column -header "SELECT MAX(depth) FROM mp_bmf_index"
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"2.0","ШиринаЧашелистика":"4.0","ДлиннаЛепестка":"1.8","ШиринаЛепестка":"1.8"}}]'
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}}]' # Iris-virginica
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}}]' # Iris-versicolor
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]' # Iris-setosa
#./bimorph clump/iris.sqlite -dano '[{"dano":{"ДлиннаЧашелистика":"6.3","ШиринаЧашелистика":"3.3","ДлиннаЛепестка":"6.0","ШиринаЛепестка":"2.5"}},
#											{"dano":{"ДлиннаЧашелистика":"7.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"4.7","ШиринаЛепестка":"1.4"}},
#											{"dano":{"ДлиннаЧашелистика":"5.0","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.2","ШиринаЛепестка":"0.2"}}]'

#cat clump/crc8.json | ./bimorph - clump/crc8.sqlite -c -rand 0 -thread 8
#sqlite clump/crc8.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#cat clump/house.json | ./bimorph - clump/house.sqlite -c -rand 0 -ocl 1
#sqlite clump/house.sqlite -column -header "SELECT * FROM mp_bmf_test ORDER BY id DESC LIMIT 10; -- SELECT COUNT(*) as step, SUM(change) as SumChange, MAX(bmf) as bmf FROM mp_bmf_test WHERE perc<10;"

#./bimorph -dano '[{"dano":{"Один":"0", "Два":"0"}}]' ./2
#rm /tmp/3; cat /tmp/tri.json | ./bmf - -rand 0 /tmp/3

