#!/usr/bin/env python3

# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2014 uralbash <root@uralbash.ru>
#
# Distributed under terms of the MIT license.
#
# 1.hello.py
# http://www.tutorialspoint.com/python/python_cgi_programming.htm


print("Set-Cookie:UserID=XYZ;")
print("Set-Cookie:Password=XYZ123;")
print("Set-Cookie:Expires=Tuesday, 31-Dec-2007 23:12:40 GMT;")
print("Set-Cookie:Domain=www.tutorialspoint.com;")
print("Set-Cookie:Path=/perl;")
print("Content-type:text/html\n\n")
print('<html>')
print('<head>')
print('<title>Hello Word - First CGI Program</title>')
print('</head>')
print('<body>')
print('<h2>Hello Word! This is my first CGI program</h2>')
print('</body>')
print('</html>')