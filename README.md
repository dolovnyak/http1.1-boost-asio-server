Глобально, что осталось до конца:
* запуск cgi скриптов
* Перед тем как делать загрузку и удаление файлов, посмотреть это работает в nginx.
* загрузка файлов через Put, если это разрешено в конфиге
* удаление файлов через Delete, если это разрешено в конфиге
* directory listing и возможность отключения и включения его из конфига
* Http redirection
* Тесты на ReadFile, проверить что кастомные ерор старинцы работают.
* Проверить что locations работают, отдельно написать тесты на функцию, которая их матчит с Route.
* Написать мок конфига.
* Делать ли обработку trailer-part при chanked запросе?


Завтра: сделать мок (а лучше допилить или чтобы Оля допилила парсер и просто настроить) конфига и пример как это сказано в чекере.
Проверить как работают put и delete в nginx.
Погуглить как работает http redirection.
Проверить, если в nginx задать location /images, а задать запрос /images123/ - сматчит ли он это.

Сегодня за ночь мне надо полностью закончить парсер чтобы он подходил под сабжект и больше не думать о нем.

- Download the cgi_test executable on the host
- Create a directory YoupiBanane with:
  * file name youpi.bad_extension 
  * file name youpi.bla 
  * sub directory called nop 
  * file name youpi.bad_extension in nop 
  * file name other.pouic in nop 
  * sub directory called Yeah 
  * file name not_happy.bad_extension in Yeah 

Setup the configuration file as follow:
- / must answer to GET request ONLY
- /put_test/* must answer to PUT request and save files to a directory of your choice
- any file with .bla as extension must answer to POST request by calling the cgi_test executable
- /post_body must answer anything to POST request with a maxBody of 100
- /directory/ must answer to GET request and the root of it would be the repository YoupiBanane and if no file are requested, it should search for youpi.bad_extension files


Хедеры поддерживаемые на данный момент:
* Host (not supported ipv6 and ipvFuture)
* Content-Length
* Transfer-Encoding (only chunked supported)
* Connection
* Keep-Alive (only timeout supported)

Краткое описание хедеров:
* Host - хост и порт сервера, к которому обращается клиент
* Content-Type - тип контента, который отправляется клиенту (например text/html)
* Content-Length - длина тела запроса
* Transfer-Encoding - способ передачи тела запроса (chunked, compress, deflate, gzip, identity) - только chunked поддерживается
* Connection - управление соединением (keep-alive, close)
* Keep-Alive - параметры keep-alive соединения (timeout, max) - только timeout поддерживается
* Authorization - авторизация пользователя (Basic, Digest, Bearer, HOBA, Mutual, AWS4-HMAC-SHA256) - только Basic поддерживается
* Cookie - куки пользователя, которые передаются в запросе
* Set-Cookie - куки, которые передаются в ответе сервера и хранятся у клиента и передаются в следующие запросы через Cookie
* Server - имя сервера
* Date - дата и время запроса

В конфиге должно быть:
* "Turn on or off directory listing" (не до конца понял че это значит)
* "Define a list of accepted HTTP methods for the path"
* "Define a HTTP redirection." (один чел говорил, что тестер не может норм в redirection, проверить это)
* "Execute CGI based on certain file extension (for example .php)" (это не оч, у меня сейчас все через директории)

Сgi нужно запускать, чтобы его относительные пути сохранялись.

Events interaction:
Core inner read event -> HttpSessionReadEvent -> HttpSessionProcessRequestEvent -> 
Core inner read event -> HttpFileReadEvent -> Core inner read zero bytes event -> 
FileReadZeroByteEvent -> Core inner write event (response to client) -> 
Core inner after write event -> HttpSessionAfterResponseEvent

написать cgi (нужно корректно прокинуть энвайронмент аргументы и тело реквеста, и поместить stdout fd в poll)
после cgi состояние сервера будет минимально рабочим
поэтому после cgi убрать моки на парсер и подключить реальный парсер
после заранить с школьными тестами и начать реализовывать нюансы с методами и хедерами

Парсер:
* autoindex по умолчанию выключен.
* Если autoindex включен и задан index.html, то игнорируется autoindex и отдается index.html.
* Для каждого сервера прописывается его рут директория в абсолютном пути.
* Для каждого location прописывается его рут директория в относительном пути от рут дирректории сервера.
* Для каждой default_error_page прописывается путь к файлу в относительном пути от рут дирректории сервера.
* SessionsKillDelay должен быть больше или равен CoreTimeout. (так как в ином случае он будет вызываться по времени CoreTimeout, а не SessionsKillDelay)
* AvailableMethods - если на задано, то ограничений нет.

Есть locations:
Может быть location /bin
Может быть location /bin/upload
Может быть location /bin/upload/statistics.py
Среди всех этих locations должен быть подобран самый оптимальный.
То есть для /bin/upload/statistics.py должен быть выбран location /bin/upload/statistics.py.
а для /bin/upload/"что угодно кроме statistics.py" должен быть выбран location /bin/upload.



