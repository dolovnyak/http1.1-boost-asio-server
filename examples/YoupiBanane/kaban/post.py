#!/usr/bin/env python3

import requests


files={'file_name': b'kek_check_cheburek_matreshka'}
responce = requests.post('http://localhost:1025/kaban/script.py', files=files)

print(responce.text)

