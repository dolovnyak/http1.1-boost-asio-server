# Webserver-42

### Nginx style webserver working with poll (TODO epoll), which can manage static content, upload files, run cgi-scripts. Supported autoindex, nginx style locations, partial support of http/1.1 and http/1.0

* TODO build status
* TODO tests coverage

## Config rules:
- ### Config common fields:
  * MaxSessionsNumber `default: 1024` - how many simultaneous connections the server can handle. (the user must take into account the limitation on sockets in the operating system)
  * ReadBufferSize `default: 4096` - buffer size when server is reading from socket.
  * CoreTimeous_s `default: 1s` - timeout for poll or epoll when event queue is empty. (if queue isn't empty, timeout is 0) 
  * SessionsKillerDelay_s `default: 2s` - delay for task who kills hung or keep-alive sessions by timeout. It's recommended to set it larger and multiple of CoreTimeout_s because otherwise delay may be longer.
  * ServerInstances `required at least one server` - array of server instances.

- ### Server instanse fields:
  * Port `required` - the port that this server is listening on. (few servers may listen the same port and first server declared in config will be default, [see below](#few-servers-on-the-same-port-rules))
  * Name `required` - the name of the server which is used in the "host" request header. If few servers are listening one port, the desired server will be selected with the host header with the correct server name:port" 
  * Root `required` - absolute path which will be the directory that will be returned by the "/" route (if there is no location). Also if location doesn't have root, or if it has root that doesn't start with "/" it will use server root.
  * DefaultErrorPages [`default: HttpErrorPages.cpp`](https://github.com/dolovnyak/webserver-42/blob/master/srcs/http/errors/HttpErrorPages.cpp) - array like {http_error_code, path}, where path either absolute path if it begin with "/", either relative root path.
  * MaxBodySize_b `default: 100 mb (100000000 b)` - max request body size (this value is checking during request reading).
  * MaxRequestSize_b `default: 200 mb (200000000 b)` - max request size (this value is checking during request reading).
 
## Few servers on the same port rules:

### Config example:
```
{
  "MaxSessionsNumber": 2048,
  "ReadBufferSize": 4096,
  "SessionsKillerDelay_s": 2,
  "HangSessionTimeout_s": 10,
  "CoreTimeout_s": 1,
  "ServerInstances": [
    {
      "Port": 80,
      "Name": "test1",
      "Root": "/tmp/my_server/",
      "MaxBodySize" : 8192,
      "MaxRequestSize" : 16384,
      "Locations": [
        {
          "MaxClientBodySize": 4096,
          "Location": "/",
          "Root": "/",
          "Autoindex": 0,
          "Index": "index.html",
          "AvailableMethods": [
            "GET",
            "POST"
          ]
        }
      ]
    },
    {
      "Port": 80,
      "Name": "kabun2",
      "Root": "/Users/sbecker/Desktop/projects/webserver-42/examples/cgi_checker2",
      "MaxBodySize": 8096,
      "MaxRequestSize": 16384,
      "DefaultErrorPages": {
        "404": "/error_pages/404.html",
        "405": "/error_pages/405.html",
        "500": "/error_pages/500.html"
      },
      "CgiExtensions": [
        ".py",
        ".php"
      ],
      "Locations": [
        {
          "Location": "/",
          "Root": "/",
          "Autoindex": 1,
          "Index": "index.html",
          "AvailableMethods": [
            "GET",
            "POST"
          ]
        },
        {
          "Location": "/upload",
          "Root": "/cgi-bin/upload.py",
          "AvailableMethods": [
            "GET",
            "POST",
            "DELETE"
          ]
        },
        {
          "Location": "/images/kitty.jpg",
          "Redirect": "/images/kitty2.jpg"
        }
      ]
    }
  ]
}
```

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
