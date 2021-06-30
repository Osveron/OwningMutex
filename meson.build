project('OwningMutex', 'cpp')

src = ['src/main.cpp']

test = executable('main', sources: src, link_args : ['-ldl', '-pthread'])

test('test', test)
