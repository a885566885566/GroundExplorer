#!/usr/bin/env python3
import http.server
import logging
from urllib.parse import urlparse
from os import path
import json

import eventlet
import socketio

SOCKET_PORT=10417
sio = socketio.Server()
app = socketio.WSGIApp(sio)

@sio.event
def connect(sid, environ):
    print('connect ', sid)

@sio.event
def message(sid, data):
    print('message ', data)

@sio.event
def disconnect(sid):
    print('disconnect ', sid)

MODE_UPDATE = '0'

PORT = 10418
Handler = http.server.SimpleHTTPRequestHandler

# Class to handle http request
class RequestHandler(Handler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
    
    def do_GET(self):
        # Parse path and query
        querypath = urlparse(self.path)
        filepath, query = querypath.path, querypath.query 

        #logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        # Get new keyword from front-end
        if filepath.endswith('/esp_data'):
            #logging.info("Data from esp: " + query)
            self.send_response(200)
            q_list = query.split("?")
            q_dict = {}
            for q in q_list:
                q_dict[q.split("=")[0]] = q.split("=")[1]
            print(q_dict)
            if(q_dict['mode'] == MODE_UPDATE):
                print("ok")
            
            #self.send_header('Content-type', 'json')
            #self.end_headers()
            # Write back correlation json file
            #self.wfile.write(json.dumps(json_data).encode('utf-8'))

def server_run():
    #logging.getLogger('requests').setLevel(logging.WARNING)
    #logging.info('Starting httpd...\n')
    httpd = http.server.HTTPServer(("", PORT), RequestHandler)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    #logging.info('Stopping httpd...\n')

def main():
    logging.disable(50)
    print("Start_ok")
    server_run()
    eventlet.wsgi.server(eventlet.listen(('', SOCKET_PORT)), app)
    print("socket running")

if(__name__ == "__main__"):
    main()
