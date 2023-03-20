# http1.1-boost-asio-server

## Config:
- ### Config common fields:
  * **MaxSessionsNumber** `default: 1024` - how many simultaneous connections the server can handle. (the user must take into account the limitation on sockets in the operating system)
  * **ServerInstances** `required at least one server` - array of server instances.

- ### Server instanse fields:
  * **Host** `required` - the рщые that this server is listening on.
  * **Port** `required` - the port that this server is listening on. Few servers may listen the same host:port. First server declared in config will be default. See [default server rules](#default-server-rules))
  * **Name** `required` - the name of the server which is used in the "host" request header. If few servers are listening one port, the desired server will be selected with the host header with the correct server_name:port" 
  * **ErrorPages** `default:`[`HttpErrorPages.cpp`](https://github.com/dolovnyak/webserver-42/blob/master/srcs/http/errors/HttpErrorPages.cpp) - array like {http_error_code, path}, where path either absolute path if it begin with "/", either relative root path. 
  * **MaxBodySize_b** `default: 100 mb (100000000 b)` - max request body size (this value is checking during request reading).
  * **MaxRequestSize_b** `default: 200 mb (200000000 b)` - max request size (this value is checking during request reading).
  * **KeepAliveTimeout_s** `default: 60 s` - default keep-alive timeout if connection persistent and there is no keep-alive header with timeout value.
  * **KeepAliveMaxTimeout_s** `default: 1800 s` - max keep-alive timeout which could be set from request, if timeout from request higher than max, timeout - max.
  * **Locations** `required at least one location` - array of locations, which will be match with routes. See [locations match rules](#locations-match-rules)

- ### Location fields:
  * **location** `required` - there are two types locations: Path location - location that start with "/" (for example /images). And extension location - location that start with "." (for example .php). See [locations match rules](#locations-match-rules)
  * **Priority** `default: 0` - each location has priority and it used during matching. See [locations match rules](#locations-match-rules)
  * **Root** `required` - absolute path which will be the root location directory. if url /a/b and location root is /tmp/server/, full path will be /tmp/server/a/b/
  * **autoindex** `default: false` - on/off directory listing. Behave according to [intersected location fields rules](#intersected-location-fields-rules)
  * **methods** `default: no one` - array of available methods for this location. Other will forbidden.
  * **cgi_path** - `default: none (cgi off)` - path to cgi script which will be execute for this location. Behave according to [intersected location fields rules](#intersected-location-fields-rules)
  * **upload_path** `default: none (uploading off)` - path to the directory where to upload files when a Put request arrives. Behave according to [intersected location fields rules](#intersected-location-fields-rules)
  * **return** `default: none` - "http_error_code" or "http_redirect_code url". if url start with "/" it's local path and we need to construct correct url. In other case we consider that url is correct global url. Behave according to [intersected location fields rules](#intersected-location-fields-rules)
 
## Default server rules:
If several servers listen on the same host:port, then the first one in the config will be default. That means two things: 
 1. If host header match with no one server - default server will be used.
 2. Until the host header is processed (that is, until the host header is read), the default server will be used and this will affect error pages. If we have default server A with error_page 404 /A/404.html and server B with error_page 404 /B/404.html - in case if request invalid, like incorect header syntex we will recieve error_page /A/404.html.

## Locations match rules:
* If the priority of locations is equal, then prefix locations are searched first, then locations with a path. And locations with a path will be selected by more complete similarity (that is, by query /images/kitties/black from /images/kitties/ and /images will be selected /images/kitties/ (because it more similarity.

* If the priority is different, then locations with a higher priority will always be selected first, for example, if you set the location /images/cat.jpg with priority 1 and /images with priority 2, then location for /images/cat.jpg will never selected.

## Intersected location fields rules:
There are 3 location fields that cause different behavior and conflict with each other. It's `Index`, `Return` and `Autoindex`. For now we will never allow more than one of them to be enabled at the same location.

## Developers sections:
Only asio and json boost libraries are used, if you need to add the boost library, do this:
```
wget https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.bz2
tar --bzip2 -xf boost_1_81_0.tar.bz2
bcp --boost=/path/to/boost_1_81_0 needed_lib_name /path_to_webserver/boost_1_81_0
```

### Setup linux environment:
```
apt-get update
apt-get install build-essential
apt-get install gcc
apt-get install g++
apt-get install gdb
apt-get install clang
apt-get install make
apt-get install ninja-build
apt-get install cmake
apt-get install autoconf
apt-get install automake
apt-get install locales-all
apt-get install dos2unix
apt-get install rsync
apt-get install tar
apt-get install python
apt-get install apache2-utils
apt-get install openssl
```

