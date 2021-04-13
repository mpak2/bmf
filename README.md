# bmf
<p>bimorph Прототип

<p>Все действия производить в консоли линукс. Набор данных "Ирисы Фишера"

<p>$wget "https://github.com/mpak2/bmf/raw/master/bimorph" # Скачиваем исполняемый файл алгоритма
<p>$chmod a+x ./bimorph # Добавляем права доступа файлу на исполнение
<p>$wget http://биморф.рф/include/iris.json # Cкачиваем  набор данных для обучения
<p>$cat iris.json | ./bimorph - # Загружаем обучающий набор данных в модель
<p>$./bimorph mysql://iris:iris@192.168.1.6/iris ds=1 epoch=100 # Обучаем. БД для хранения модели iris.sqlite и максимального количества эпох для обучения 100
<p>$./bimorph mysql://iris:iris@192.168.1.6/iris # Отображение результатов обучения

<p>Во время обучения можно наблюдать рост размера сети и процент правильных ответов на прошлой и текущей эпохе
Сеть:318 Процент:99.333336/100.000000%
Обучение займет около минуты и закончится примерно таким сообщением

<p>2235 Сохранение списка морфов << Эпох: 26 Морфов: 318 Время: 71
В директории с алгоритмом появится файл iris.sqlite базы данных с вновь созданной моделью. После обучения можно делать прогнозы
Формат исходных данных должен быть подобным формату iris.json но без обучающих.

<p>Ответ выдается в таком формате json:

<p>$./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"5.9","ШиринаЧашелистика":"3.0","ДлинаЛепестка":"5.1","ШиринаЛепестка":"1.8"}}]' 2>/dev/null #Iris-virginica
<p>$./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"5.6","ШиринаЧашелистика":"2.7","ДлинаЛепестка":"4.2","ШиринаЛепестка":"1.3"}}]' 2>/dev/null #Iris-versicolor
<p>$./bimorph mysql://iris:iris@192.168.1.6/iris dano='[{"dano":{"ДлинаЧашелистика":"4.4","ШиринаЧашелистика":"3.2","ДлинаЛепестка":"1.3","ШиринаЛепестка":"0.2"}}]' 2>/dev/null #Iris-setosa

Примеры использования https://github.com/mpak2/bmf/blob/master/bmf.sh
