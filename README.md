Глобально, что осталось до конца:
* запуск cgi скриптов
* загрузка файлов через Post + cgi-скрипт загрузчик файла
* удаление файлов через Delete + cgi-скрипт удаления файла
* возможность задавать для каждого файла какие методы к нему можно применять
* метод put
* directory listing и возможность отключения и включения его из конфига
* Http redirection
* Добавить лимиты для body size и для хедеров (первая строка будет входить в лимит хедеров) и общий лимит на raw_request чтобы в него нельзя было пихать мусор


* Делать ли обработку trailer-part при chanked запросе?

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
* "Define a list of accepted HTTP methods for the route"
* "Define a HTTP redirection." (один чел говорил, что тестер не может норм в redirection, проверить это)
* "Execute CGI based on certain file extension (for example .php)" (это не оч, у меня сейчас все через директории)

Сgi нужно запускать, чтобы его относительные пути сохранялись.

Events interaction:
Core inner read event -> HttpReadRequestEvent -> HttpProcessRequestEvent -> 
Core inner read event -> HttpFileReadEvent -> Core inner read zero bytes event -> 
FileReadZeroByteEvent -> Core inner write event (response to client) -> 
Core inner after write event -> HttpAfterResponseEvent

написать cgi (нужно корректно прокинуть энвайронмент аргументы и тело реквеста, и поместить stdout fd в poll)
после cgi состояние сервера будет минимально рабочим
поэтому после cgi убрать моки на парсер и подключить реальный парсер
после заранить с школьными тестами и начать реализовывать нюансы с методами и хедерами
