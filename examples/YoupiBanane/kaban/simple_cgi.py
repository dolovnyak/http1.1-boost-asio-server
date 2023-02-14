#!/usr/bin/env python3

import cgi
import cgitb
import os
import sys

cgitb.enable()  # Optional; for debugging only

print("bbbbbbb")
for i in os.environ.keys():
    print(i, os.environ[i])


data = sys.stdin.read()
print("body is: ", data)
