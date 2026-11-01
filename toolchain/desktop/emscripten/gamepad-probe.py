#!/usr/bin/env python3
"""Serve gamepad-probe.html over HTTPS and save the reports it sends.

Usage: gamepad-probe.py <host> <port> [report dir]
Browsers only expose gamepads to secure origins, hence HTTPS.
"""
from datetime import datetime
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import json
import ssl
import subprocess
import sys

HERE = Path(__file__).resolve().parent
PAGE = HERE / 'gamepad-probe.html'
KEY, CERT = HERE / 'key.pem', HERE / 'cert.pem'
MAX_REPORT = 1 << 20


class ProbeHandler (BaseHTTPRequestHandler):
  reports = Path('.')

  def do_GET (self):
    if self.path.split('?')[0] not in ('/', '/gamepad-probe.html'):
      self.send_error(HTTPStatus.NOT_FOUND)
      return
    body = PAGE.read_bytes()
    self.send_response(HTTPStatus.OK)
    self.send_header('Content-Type', 'text/html; charset=utf-8')
    self.send_header('Content-Length', str(len(body)))
    self.end_headers()
    self.wfile.write(body)

  def do_POST (self):
    length = int(self.headers.get('Content-Length', 0))
    if self.path != '/report' or length > MAX_REPORT:
      self.send_error(HTTPStatus.BAD_REQUEST)
      return
    try:
      report = json.loads(self.rfile.read(length))
    except ValueError:
      self.send_error(HTTPStatus.BAD_REQUEST)
      return
    out = self.reports / f'gamepad-{datetime.now():%Y%m%d-%H%M%S}.json'
    out.write_text(json.dumps(report, indent=1))
    print(f'report: {out}', flush=True)
    self.send_response(HTTPStatus.OK)
    self.send_header('Content-Length', '0')
    self.end_headers()


if __name__ == '__main__':
  host, port = sys.argv[1], int(sys.argv[2])
  ProbeHandler.reports = Path(sys.argv[3] if len(sys.argv) > 3 else '.')
  ProbeHandler.reports.mkdir(parents=True, exist_ok=True)
  if not KEY.exists():
    subprocess.run(['openssl', 'req', '-x509', '-newkey', 'rsa:2048', '-nodes',
                    '-days', '30', '-subj', f'/CN={host}',
                    '-keyout', KEY, '-out', CERT], check=True)
  context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
  context.load_cert_chain(keyfile=KEY, certfile=CERT)
  httpd = ThreadingHTTPServer((host, port), ProbeHandler)
  httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
  print(f'https://{host}:{port}/', flush=True)
  httpd.serve_forever()
