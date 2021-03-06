# Реверc протокола обмена магнитоллы Blaupunkt Volga GAZ 31105 и CDC

## ИНИЦИАЛИЗАЦИЯ

После включения магнитола шлет в уарт 4ре раза  0x180 на скорости 4800 и если нет ответа - отключает поддержку  CDC

Вот эти импульсы http://www.bestfilessharing.com/23UCptioxqU9H27
Принцип работы - эхо ответы двух сторон. 

Например

|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180  | --> | 0x180 |

говорит о том, что сперва голова шлет 0x180 в CDC, а  CDC отвечает 0x180.
Знак -->  говорит о том, что передачу начала  голова.
Знак <-- означает, что передачу начал CDC


## Описание команд
0x180 = начало передачи
0x14F = конец передачи. Ченджер не отвечает на эту команду, а магнитола всегда отвечет.

### Начало обмена на скорости 4800 бод, порт 9 бит,  без проверки четности

|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|	-->|0x180|
|0x0AD|	-->|0x0AD|
|0x14F|	-->|нет ответа|
||**задержка 14мс**||
|0x180|-->|0x180|
|0x048|-->|0x048 (H)|
|0x001|-->|0x001 (1)|
|0x14F|-->|нет ответа|
||**задержка 30 мс**||
|0x10F|<--|0x10F  *ченджер отправил тоже самое, что пришло ему в предыдущем запросе* |
|0x048|<--|0x048 (H)|
|0x001|<--|0x001 (1)|
|0x14F|<--|0x14F|
||**задержка 18 мс**||
|0x180|-->|0x180|
|0x048|-->|0x048 (H)|
|0x002|-->|0x002 (2)|
|0x14F|-->|нет ответа|

### Переключаемся на 9600 бод, порт 9 бит,  без проверки четности

**Задержка около 40 мс**

|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x0A7|-->|0x0A7|
|0x14F|-->|нет ответа|

Ок, теперь магнитолла увидила ченджер.
Иницализация завершена.
Далее идет рабочий режим.
Мы можем передать в магнитолу статусы дисков, количество треков. 


### Выключение
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x021|-->|0x021|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x00A|<--|0x00A|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

### Сканирование диска
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x001|<--|0x001 *номер диска*|
|0x001|<--|0x001|
|0x14F|<--|0x14F|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x009|<--|0x009|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

#### если диска нет, то
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x021|<--|0x021 *номер диска от 0x021 (диск 1) до 0x02A( диск 10), команда говорит о том, что диска нет* |
|0x001|<--|0x001|
|0x14F|<--|0x14F|

#### Если диск есть, но поврежден или нечитаем
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x0F1|<--|0x0F1 *номер диска от 0x0F1 (диск 1) до 0x0FA(диск 10), команда говорит о том, что диска поврежден или не читается*|
|0x001|<--|0x001|
|0x14F|<--|0x14F|

#### если есть, то
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x001|<--|0x001 *номер диска от 0x001 (диск 1) до 0x000 (диск 10), команда говорит о том, что диск ок*|
|0x001|<--|0x001|
|0x14F|<--|0x14F|

#### дальше не знаю что, похоже на название трека. Но всегда 0x020, 8 раз
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x10B|<--|0x10B|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x020|<--|0x020|
|0x14F|<--|0x14F|

#### Инфо о диске
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x10D|			<--|0x10D|
|0x001|			<--|0x001|
|0x018|			<--|0x018 *количество треков на диске 18*|
|0x077|			<--|0x077 *минут всего*|
|0x043|			<--|0x043 *секунд всего*|
|0x14F|			<--|0x14F|

#### Потом магнитолла пытается попросить сдченджер дать другой диск
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x084|-->|0x084|
|0x14F|-->|нет ответа|
|0x180|-->|0x180|
|0x002|-->|0x002 *номер диска*|
|0x14F|-->|нет ответа|

#### После сканирования дисков ченджер начинает воспроизводить первый найденный диск
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x009|<--|0x009|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|
|0x101|<--|0x101|
|0x001|<--|0x001 *номер диска от 0x001 (диск 1) до 0x00A (диск 10), команда говорит о том, что диск ок*|
|0x001|<--|0x001 *номер трека*|
|0x14F|<--|0x14F|

#### далее ,каждую секунду ченджер отправляет в магнитолу время проигрывания файла
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x109|<--|0x109|
|0x001|<--|0x001 *номер минуты*|
|0x015|<--|0x015 *номер секунды*|
|0x14F|<--|0x14F|


### Воспроизведение / Вход в режим сдченджера
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x0A5|-->|0x0A5|
|0x14F|-->|нет ответа|
|0x101|<--|0x101|
|0x001|<--|0x001 *номер диска от 0x001 (диск 1) до 0x000 (диск 10) , команда говорит о том, что диск ок*|
|0x001|<--|0x001 *номер трека*|
|0x14F|<--|0x14F|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x009|<--|0x009|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

после этого продолжаем воспроизведение, отправляем тайминги каждую секунду.

### Пауза / выход из режима CD ченджера
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x021|-->|0x021|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x00A|<--|0x00A|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|
После этого появляется надпись PAUSE на дисплее

### Повтор трека вкл
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x093|-->|0x093|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x009|<--|0x009|
|0x008|<--|0x008|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

после этого появляется надпись RPT TRCK

### Повтор трека Выкл
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x093|-->|0x093|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x020|<--|0x020|
|0x009|<--|0x009|
|0x020|<--|0x020|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

После этого появляется надпись RPT OFF

### Случайное воспроизведение треков с диска вкл
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x0A3|-->|0x0A3|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x022|<--|0x022|
|0x009|<--|0x009|
|0x021|<--|0x021|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

После этого появляется надпись MIX CD

### Случайное воспроизведение треков с диска выкл
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x0A3|-->|0x0A3|
|0x14F|-->|нет ответа|
|0x103|<--|0x103|
|0x022|<--|0x022|
|0x009|<--|0x009|
|0x021|<--|0x021|
|0x000|<--|0x000|
|0x14F|<--|0x14F|

После этого появляется надпись MIX OFF

### Следующий трек
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x082|-->|0x082|
|0x14F|-->|нет ответа|
|0x180|-->|0x180|
|0x002|-->|0x002 *номер требуемого трека*|
|0x14F|-->|нет ответа|

### Предыдуший  трек
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x081|-->|0x081|
|0x14F|-->|нет ответа|
|0x180|-->|0x180|
|0x002|-->|0x002 *номер требуемого трека*|
|0x14F|-->|нет ответа|

### Следующий диск
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x084|-->|0x084|
|0x14F|-->|нет ответа|

### Предыдущий диск
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x083|-->|0x083|
|0x14F|-->|нет ответа|

### Перемотка трека вперед ( длительное нажате кнопки вправо)
#### Начало нажатия
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x091|-->|0x091|
|0x14F|-->|нет ответа|
#### Конец нажатия
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x011|-->|0x011|
|0x14F|-->|нет ответа|

### Перемотка трека назад ( длительное нажате кнопки влево)
#### Начало нажатия
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x092|-->|0x092|
|0x14F|-->|нет ответа|
#### Конец нажатия
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x180|-->|0x180|
|0x012|-->|0x012|
|0x14F|-->|нет ответа|


***

## Ощибки инициализации
### Инициализация без бокса с дисками
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x021|<--|0x021 (!)|
|0x001|<--|0x001 (1)|
|0x14F|<--|0x14F|
||задержка 30 мс||
|0x10С|<--|0x10С|
|0x002|<--|0x002 (2)|
|0x14F|<--|0x14F|
||задержка 30 мс||
|0x10E|<--|0x10E|
|0x008|<--|0x008 (8)|
|0x001|<--|0x001 (1)|
|0x14F|<--|0x14F|

### Инициализация с боксом, но без дисков
|ГОЛОВА | НАПРАВЛЕНИЕ | ЧЕНЖЕР|
|:-----:|:-----------:|:-----:|
|0x101|<--|0x101|
|0x001|<--|0x001 (1)|
|0x001|<--|0x001 (1)|
|0x14F|<--|0x14F|



