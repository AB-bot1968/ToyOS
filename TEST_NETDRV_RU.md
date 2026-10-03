# Подробное тестирование NETDRV.EXE1 — Toy OS v58

## 1. Назначение

NETDRV.EXE1 — минимальное сетевое приложение Ring-3 для NE2000 ISA в QEMU.
Запуск из консоли Toy OS:

```text
exec NETDRV.EXE IP SEND FILE.TXT
exec NETDRV.EXE IP RECV FILE.TXT
```

`IP` — IPv4-адрес удалённого узла. `SEND` передаёт содержимое указанного файла HTTP POST-запросом. `RECV` выполняет HTTP GET и сохраняет тело ответа в указанный файл.

Конфигурация всегда имеет имя `NET.CFG` и хранится в корне FAT16-диска. Исходный экземпляр находится в `src/NET.CFG`; `build.sh` автоматически копирует его в `build/NET.CFG` при каждом запуске сборки.

## 2. Требования на Windows 7

Нужны:

1. 32-битный W64DevKit для сборки Toy OS.
2. QEMU для запуска `qemu-system-i386.exe`.
3. Windows 7 с разрешённым локальным TCP-портом тестового HTTP-сервера.
4. Для тестов `SEND` и `RECV` — любой простой HTTP-сервер на Windows 7. Ниже приведён готовый вариант на Python 2, который подходит для старой Windows 7.

## 3. Сборка

Откройте shell W64DevKit и перейдите в каталог проекта:

```text
cd /путь/к/v57work
```

Проверьте, что конфигурация находится именно здесь:

```text
ls src/NET.CFG
```

Запустите сборку:

```text
./build.sh
```

После начала сборки должны существовать оба файла:

```text
src/NET.CFG
build/NET.CFG
```

Проверить копирование можно командой:

```text
cmp src/NET.CFG build/NET.CFG
```

Команда не должна выводить различий.

Важно: вручную класть `NET.CFG` в `build` больше не требуется. Каждый новый `./build.sh` удаляет старый `build` и заново копирует `src/NET.CFG`.

## 4. Базовая конфигурация QEMU

Для стандартной user-mode сети QEMU используется:

```text
Локальный адрес Toy OS : 10.0.2.15
Маска                  : 255.255.255.0
Шлюз                   : 10.0.2.2
```

В `src/NET.CFG` для локального HTTP-теста рекомендуется временно установить:

```text
BASE=300
MAC=52:54:00:12:34:56
IP=10.0.2.15
MASK=255.255.255.0
GATEWAY=10.0.2.2
PORT=8080
PATH=/
TIMEOUT=10
```

После изменения конфигурации обязательно снова выполнить:

```text
./build.sh
```

## 5. Запуск QEMU

Из каталога проекта:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -netdev user,id=n0 -device ne2k_isa,netdev=n0,mac=52:54:00:12:34:56
```

Для старого QEMU допустим вариант:

```text
qemu-system-i386 -hda build/toy_os.img -m 16M -net user -net nic,model=ne2k_isa,macaddr=52:54:00:12:34:56
```

Дождитесь загрузки Toy OS и появления приглашения:

```text
toy0>
```

## 6. Проверка наличия NET.CFG внутри диска

В консоли Toy OS:

```text
ls
```

В списке должен присутствовать:

```text
NETDRV.EXE
NET.CFG
```

Затем:

```text
cat NET.CFG
```

Должны отображаться параметры из `src/NET.CFG`.

Это одновременно проверяет, что `build.sh` не только скопировал конфигурацию в `build`, но и поместил её в FAT16-образ.

## 7. Подготовка HTTP-сервера для RECV

На Windows 7 создайте каталог, например:

```text
C:\toyhttp
```

Создайте в нём файл `index.html`:

```html
<html><body><h1>Toy OS NETDRV TEST</h1><p>RECV OK</p></body></html>
```

Если установлен Python 2, откройте `cmd.exe` Windows 7 и выполните:

```text
cd C:\toyhttp
python -m SimpleHTTPServer 8080
```

Сервер должен сообщить, что слушает TCP-порт 8080.

В Windows Firewall при необходимости разрешите входящие TCP-соединения на порт 8080 для Python.

Проверка непосредственно в Windows 7:

```text
http://127.0.0.1:8080/
```

Если страница открывается, HTTP-сервер работает.

## 8. Тест RECV

После запуска QEMU выполните в Toy OS:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

Ожидаемая последовательность сообщений примерно такая:

```text
NETDRV: ...
NETDRV: ARP ...
NETDRV: TCP connected
NETDRV: RECV page saved
```

После завершения:

```text
ls
```

Должен появиться `PAGE.TXT`.

Проверка содержимого:

```text
cat PAGE.TXT
```

Должно быть содержимое `index.html`.

Проверка размера:

```text
filesize PAGE.TXT
```

## 9. Тест RECV конкретного пути

Если в `src/NET.CFG` указать:

```text
PATH=/index.html
```

снова собрать:

```text
./build.sh
```

и запустить:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

NETDRV выполнит GET для `/index.html`.

## 10. Подготовка файла для SEND

В Toy OS создайте небольшой тестовый файл:

```text
write SEND.TXT Hello from Toy OS NETDRV v58
```

Проверьте:

```text
cat SEND.TXT
```

Для проверки передачи нужен HTTP-сервер, который принимает POST. Обычный `SimpleHTTPServer` Python 2 принимает GET, но не является полноценным POST-приёмником. Поэтому для `SEND` рекомендуется использовать небольшой тестовый сервер, который отвечает на POST кодом `200` и сохраняет тело запроса.

Пример сервера Python 2:

```python
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

class H(BaseHTTPRequestHandler):
    def do_POST(self):
        n = int(self.headers.getheader('Content-Length', '0'))
        data = self.rfile.read(n)
        open('received.bin', 'wb').write(data)
        self.send_response(200)
        self.send_header('Content-Length', '2')
        self.end_headers()
        self.wfile.write('OK')

    def log_message(self, fmt, *args):
        print(fmt % args)

HTTPServer(('0.0.0.0', 8080), H).serve_forever()
```

Сохраните его как `post_server.py`, запустите из отдельного каталога:

```text
python post_server.py
```

После этого выполните в Toy OS:

```text
exec NETDRV.EXE 10.0.2.2 SEND SEND.TXT
```

После успешного завершения на Windows 7 должен появиться файл `received.bin`.

Сравнить его с исходным `SEND.TXT` можно средствами Windows:

```text
fc /b received.bin SEND.TXT
```

Для текстового файла также можно выполнить:

```text
type received.bin
```

## 11. Тест ошибки отсутствующего файла

Выполните:

```text
exec NETDRV.EXE 10.0.2.2 SEND NOFILE.TXT
```

Ожидается сообщение об ошибке открытия/чтения файла SEND.

Сетевой обмен при этом не должен считаться успешным.

## 12. Тест ошибки IP

Проверить обработку неправильного адреса:

```text
exec NETDRV.EXE 999.999.999.999 RECV BAD.TXT
```

Ожидается:

```text
NETDRV: bad destination IP
```

## 13. Тест неправильного режима

```text
exec NETDRV.EXE 10.0.2.2 ABC FILE.TXT
```

Ожидается сообщение:

```text
NETDRV: direction must be SEND or RECV
```

## 14. Тест неправильного количества аргументов

```text
exec NETDRV.EXE
```

Ожидается строка использования:

```text
NETDRV: usage IP SEND|RECV FILE.TXT
```

## 15. Тест недоступного узла

Например:

```text
exec NETDRV.EXE 10.0.2.123 RECV PAGE.TXT
```

Если такого узла в сети нет, NETDRV должен завершиться ошибкой ARP/TCP timeout, а не зависнуть навсегда.

## 15.1. Диагностика ошибки `connect failed`

Если ранее команда:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

заканчивалась сообщением `connect failed`, это могло быть вызвано ошибкой начальной позиции кольцевого RX-буфера NE2000. В исправленной версии `BNRY=PSTART-1` (`0x45` при `PSTART=0x46`). Без этого ARP/TCP кадры, физически поступившие от QEMU, не извлекались из кольца при polling.

После обновления обязательно пересоберите образ:

```text
./build.sh
```

и заново запустите QEMU. Старый `toy_os.img` использовать нельзя.

## 16. Рекомендуемый порядок полного теста

Используйте именно такой порядок:

1. Проверить `src/NET.CFG`.
2. Выполнить `./build.sh`.
3. Проверить `cmp src/NET.CFG build/NET.CFG`.
4. Запустить QEMU с `ne2k_isa`.
5. В Toy OS выполнить `ls` и убедиться в наличии `NETDRV.EXE` и `NET.CFG`.
6. Выполнить `cat NET.CFG`.
7. На Windows 7 запустить HTTP GET-сервер на `0.0.0.0:8080`.
8. Выполнить `exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT`.
9. Проверить `cat PAGE.TXT`.
10. Создать `SEND.TXT`.
11. Запустить POST-сервер.
12. Выполнить `exec NETDRV.EXE 10.0.2.2 SEND SEND.TXT`.
13. Сравнить `received.bin` с `SEND.TXT`.
14. Проверить ошибки неправильного IP, режима и имени файла.

## 17. Важное ограничение первой версии

Это минимальный учебный TCP/HTTP драйвер, а не полноценный TCP/IP стек. В частности, он рассчитан на простые HTTP/1.0 обмены и тестовую среду QEMU. Для диагностики проблем сначала следует проверять цепочку:

```text
QEMU NIC
  -> NE2000
  -> Ethernet
  -> ARP
  -> IPv4
  -> TCP
  -> HTTP
```

Если `RECV` не проходит, не следует сразу проверять `SEND`: сначала необходимо добиться успешного GET.

## Исправление v58.2 — TCP connect failed и корректный приём ответа

В версии v58.1 оставалась критическая ошибка в формировании IPv4-заголовка.
Переменная `total` уже содержала полный размер TCP-сегмента вместе с IPv4-заголовком:
`20 + 20 + payload`. Однако в поле IPv4 Total Length дополнительно прибавлялись ещё 20 байт.
В результате SYN передавался с неверным Total Length, а сервер мог отбросить пакет до TCP.

Исправлено:

```text
старое: put_be16(ip+2,20+total)
новое:  put_be16(ip+2,total)
```

Также исправлен RCR NE2000: значение `0x01` означало только сохранение ошибочных кадров,
а не разрешение broadcast. Для ARP broadcast должен быть установлен AB=0x04.

Дополнительно исправлена обработка ситуации, когда HTTP-ответ приходит одновременно с ACK
на отправленный TCP-сегмент. Такой кадр теперь откладывается без преждевременного увеличения
`peer_seq`; затем `RECV` обрабатывает его и подтверждает в обычном порядке.

### Проверка RECV

При стандартном QEMU user networking и HTTP-сервере Windows 7:

```text
exec NETDRV.EXE 10.0.2.2 RECV PAGE.TXT
```

Ожидается:

```text
NETDRV: NE2000 init
NETDRV: ARP 10.0.2.2
NETDRV: TCP connect 10.0.2.2
NETDRV: RECV page saved
NETDRV: OK
```

После этого:

```text
ls
cat PAGE.TXT
```

### Проверка SEND

Создать в Toy OS небольшой файл:

```text
create TEST.TXT
```

или использовать уже существующий текстовый файл. Затем:

```text
exec NETDRV.EXE 10.0.2.2 SEND TEST.TXT
```

Ожидается:

```text
NETDRV: NE2000 init
NETDRV: ARP 10.0.2.2
NETDRV: TCP connect 10.0.2.2
NETDRV: SEND body complete
NETDRV: OK
```

Для проверки HTTP POST сервер должен иметь обработчик POST. Обычный
`python -m SimpleHTTPServer` на Windows 7 предназначен главным образом для GET
и может вернуть 501 на POST; это не является ошибкой TCP-драйвера.

### Минимальный независимый HTTP POST тест на Windows 7

Если нужен именно тест SEND, используйте небольшой Python-скрипт с обработчиком POST
или любой HTTP-сервер, который сохраняет тело POST. Важный критерий теста драйвера —
сервер должен увидеть POST с указанным `Content-Length` и полностью получить тело файла.
