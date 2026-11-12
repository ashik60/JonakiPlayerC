import os
import sys
import ssl
import http.server
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=0)
    parser.add_argument("--cert", required=True)
    parser.add_argument("--key", required=True)
    parser.add_argument("--dir", required=True)
    args = parser.parse_args()

    class Handler(http.server.SimpleHTTPRequestHandler):
        def __init__(self, *a, **kw):
            super().__init__(*a, directory=args.dir, **kw)
        def log_message(self, format, *a):
            # Suppress normal request logging to keep output clean
            pass

    server = http.server.HTTPServer(('127.0.0.1', args.port), Handler)
    ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ctx.load_cert_chain(args.cert, args.key)
    server.socket = ctx.wrap_socket(server.socket, server_side=True)

    print(f"READY: {server.server_port}", flush=True)

    try:
        server.serve_forever()
    except (KeyboardInterrupt, Exception):
        pass
    finally:
        server.server_close()

if __name__ == "__main__":
    main()
