Глобально, что осталось до конца:
* запуск cgi скриптов
* загрузку файлов через Post + cgi-скрипт загрузчик файла
* ставить timeout на connection
* поддержать HTTP/1.0

* Делать ли обработку trailer-part при chanked запросе?
* Добавить лимиты для body size и для хедеров (первая строка будет входить в лимит хедеров) и общий лимит на raw_request чтобы в него нельзя было пихать мусор

Хедеры, поддерживаемые в реквесте:
* Accept - говорит какие типы данных можно возвращать в респонсе.
* Accept-Encoding - говорит как можно сжимать/кодировать данные в респонсе.
* Connection - говорит закрывать ли конекшен сразу или keep-alive (keep-alive стоит по умолчанию).
* Keep-Alive - имеет timeout (как должго не закрывать конекшен) и max (максимальное кол-во реквестов послк которого конекшен закроется)
* Authorization - авторизация пользователя
* Cookie
* Connection (keep-alive or close)

Хедеры, поддерживаемые в респонсе:
* Date
* Connection (keep-alive or close)
* Content-Type
* Server
* Set-Cookie (каждый куки это отдельный хедер)

Надо распарсить URI, вычленить из него:
* путь до дирректории
* путь до файла
* имя файла
* расширение файла
* квери строку

* Сделать проверку что если нет хедера Host - то реквест некорректный

* resource объект будет состоять из пути к файлу, пути к дерриктории, расширения файла и квери параметров

Где-то нужно проверять что конекшен пора закрыть по таймауту

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

написать cgi (нужно корректно прокинуть энвайронмент аргументы и тело реквеста, сделать execve и поместить stdout fd в poll)
после cgi состояние сервера будет минимально рабочим
поэтому после cgi убрать моки на парсер и подключить реальный парсер
после заранить с школьными тестами и начать реализовывать неансы с методами и хедерами
