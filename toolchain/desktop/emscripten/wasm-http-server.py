#!/usr/bin/env python3
from http.server import HTTPServer, SimpleHTTPRequestHandler, test
from os.path import dirname
import ssl
import sys

context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(
    keyfile=f"{dirname(__file__)}/key.pem",
    certfile=f"{dirname(__file__)}/cert.pem")

class COOPRequestHandler (SimpleHTTPRequestHandler):
  def end_headers (self):
    self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
    self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
    self.send_header('Cross-Origin-Resource-Policy', 'same-site')
    SimpleHTTPRequestHandler.end_headers(self)

if __name__ == '__main__':
  httpd = HTTPServer((sys.argv[1], int(sys.argv[2])), COOPRequestHandler)
  httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
  httpd.serve_forever() 
