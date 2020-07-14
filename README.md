# bmf
bimorph Прототип

Все действия производить в консоли линукс. Набор данных "Ирисы Фишера"

$wget http://биморф.рф/include/bimorph # Скачиваем исполняемый файл алгоритма
$chmod a+x ./bimorph # Добавляем права доступа файлу на исполнение
$wget http://биморф.рф/include/iris.json # Cкачиваем  набор данных для обучения
$cat iris.json | ./bimorph - iris.sqlite -e 100 # Обучаем. БД для хранения модели iris.sqlite и максимального количества эпох для обучения 100

Во время обучения можно наблюдать рост размера сети и процент правильных ответов на прошлой и текущей эпохе
Сеть:318 Процент:99.333336/100.000000%
Обучение займет около минуты и закончится примерно таким сообщением

2235 Сохранение списка морфов << Эпох: 26 Морфов: 318 Время: 71
В директории с алгоритмом появится файл iris.sqlite базы данных с вновь созданной моделью. После обучения можно делать прогнозы
Формат исходных данных должен быть подобным формату iris.json но без обучающих.

Ответ выдается в таком формате json:
$./bimorph iris.sqlite -j '{"Прогноз1":{"dano":{"ДлиннаЧашелистика":"5.9","ШиринаЧашелистика":"3.0","ДлиннаЛепестка":"5.1","ШиринаЛепестка":"1.8"}}}' 2>/dev/null
{"Прогноз1": {"ВидИриса": "Iris-virginica"}}

$./bimorph iris.sqlite -j '{"Прогноз2":{"dano":{"ДлиннаЧашелистика":"5.6","ШиринаЧашелистика":"2.7","ДлиннаЛепестка":"4.2","ШиринаЛепестка":"1.3"}}}' 2>/dev/null
{"Прогноз2": {"ВидИриса": "Iris-versicolor"}}

$./bimorph iris.sqlite -j '{"Прогноз3":{"dano":{"ДлиннаЧашелистика":"4.4","ШиринаЧашелистика":"3.2","ДлиннаЛепестка":"1.3","ШиринаЛепестка":"0.2"}}}' 2>/dev/null
{"Прогноз3": {"ВидИриса": "Iris-setosa"}}
