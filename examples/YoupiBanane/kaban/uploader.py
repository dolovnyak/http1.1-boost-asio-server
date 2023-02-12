#!/usr/bin/env python3

import sys
import os
import cgitb


cgitb.enable()

path = os.environ['PATH_TO_ACTION']

exist_flag = os.path.exists(path)

with open(path, 'w') as file:
    file.write(sys.stdin.read())

if os.path.isfile(path):
    if not exist_flag:
        print('Status: 201 Created\r\n\r\n')
    else:
        print('Status: 202 Accepted\r\n\r\n')
else:
    print('Status: 500 Internal Server Error\r\n\r\n')
