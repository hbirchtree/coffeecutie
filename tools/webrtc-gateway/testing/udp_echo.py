#!/usr/bin/env python3
"""Minimal UDP echo server — stands in for a game server during Phase 1
verification of webrtc-gateway (see WEBRTC_TRANSPORT.md). Echoes every
datagram back to its sender, with a small prefix so the test page can
distinguish "this came back through the gateway" from a coincidental
local loopback.
"""
import socket
import sys

port = int(sys.argv[1]) if len(sys.argv) > 1 else 9999

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("127.0.0.1", port))
print(f"udp_echo listening on 127.0.0.1:{port}", flush=True)

while True:
    data, addr = sock.recvfrom(65535)
    print(f"recv {len(data)} bytes from {addr}: {data!r}", flush=True)
    sock.sendto(b"echo:" + data, addr)
