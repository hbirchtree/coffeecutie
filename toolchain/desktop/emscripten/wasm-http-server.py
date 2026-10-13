#!/usr/bin/env python3
from http import HTTPStatus
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from os.path import dirname
import os
import re
import ssl
import sys

context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(
    keyfile=f"{dirname(__file__)}/key.pem",
    certfile=f"{dirname(__file__)}/cert.pem")

class RangeReader:
  """Reads at most `length` bytes of `f`"""
  def __init__ (self, f, length):
    self.f = f
    self.left = length

  def read (self, n=-1):
    if self.left <= 0:
      return b''
    n = self.left if n < 0 else min(n, self.left)
    data = self.f.read(n)
    self.left -= len(data)
    return data

  def close (self):
    self.f.close()

class COOPRequestHandler (SimpleHTTPRequestHandler):
  def end_headers (self):
    self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
    self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
    self.send_header('Cross-Origin-Resource-Policy', 'same-site')
    self.send_header('Accept-Ranges', 'bytes')
    SimpleHTTPRequestHandler.end_headers(self)

  # Single "bytes=a-b", "bytes=a-" or "bytes=-n" ranges; anything else is served whole
  def send_head (self):
    match = re.fullmatch(r'bytes=(\d*)-(\d*)', self.headers.get('Range', '').strip())
    path = self.translate_path(self.path)
    if not match or match.groups() == ('', '') or not os.path.isfile(path):
      return SimpleHTTPRequestHandler.send_head(self)
    size = os.path.getsize(path)
    first, last = match.groups()
    if first:
      start, end = int(first), min(int(last), size - 1) if last else size - 1
    else:
      start, end = max(size - int(last), 0), size - 1
    if start >= size or start > end:
      self.send_response(HTTPStatus.REQUESTED_RANGE_NOT_SATISFIABLE)
      self.send_header('Content-Range', f'bytes */{size}')
      self.send_header('Content-Length', '0')
      self.end_headers()
      return None
    f = open(path, 'rb')
    f.seek(start)
    self.send_response(HTTPStatus.PARTIAL_CONTENT)
    self.send_header('Content-Type', self.guess_type(path))
    self.send_header('Content-Range', f'bytes {start}-{end}/{size}')
    self.send_header('Content-Length', str(end - start + 1))
    self.end_headers()
    return RangeReader(f, end - start + 1)

if __name__ == '__main__':
  httpd = ThreadingHTTPServer((sys.argv[1], int(sys.argv[2])), COOPRequestHandler)
  httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
  httpd.serve_forever()
