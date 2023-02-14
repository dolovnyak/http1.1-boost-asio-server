#!/usr/bin/env python3

import cgitb
import cgi
import os
# import sys


# data = sys.stdin.read()
# print("body is: ", data)

print("bbbbbbb")
for i in os.environ.keys():
    print(i, os.environ[i])

cgitb.enable()

form = cgi.FieldStorage()
print("keys: ", form.keys())

print("form: ", form)
fileitem = form["file_name"]


if fileitem.file:
    file = fileitem.file.read()
    with open('kek.txt', 'w') as f:
        f.write(str(file))
    print(file)


print(form)
