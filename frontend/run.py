#!/usr/bin/env python3

import os
# serve frontend folder regardless of initial cwd
os.chdir(os.path.dirname(__file__))

import http.server
# mimetype patch https://stackoverflow.com/a/63167571
# fixed in 3.12+backports? https://bugs.python.org/issue31715
HandlerClass = http.server.SimpleHTTPRequestHandler
HandlerClass.extensions_map['.js'] = 'text/javascript'
HandlerClass.extensions_map['.mjs'] = 'text/javascript'

http.server.test(HandlerClass, port=7073)
