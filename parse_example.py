def get_key_value(string):
    chanks = [x.strip() for x in string.split(':') if x]
    if len(chanks) < 2:
        raise ValueError('oops')
    return chanks[0], ' : '.join(chanks[1:])

handlers = {
    'content-length': lambda x: int(x),
    'autorization': lambda x: (x.split(' ')[0], x.split(' ')[1]),
}

header = 'autorization: 500oijoij 12431242'

key, value = get_key_value(header)

content = handlers.get(key.lower(), lambda x: x)(value)
print(content)
