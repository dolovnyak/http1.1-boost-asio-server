#!/usr/bin/env python3

import cgi
import cgitb
import os

cgitb.enable()  # Optional; for debugging only

print("aaaaaaa")
arguments = cgi.FieldStorage()
for i in arguments.keys():
    print(arguments[i].value)

print("bbbbbbb")
for i in os.environ.keys():
    print(i, os.environ[i])
