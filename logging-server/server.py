import http.server
import socketserver
import datetime
import urllib.parse
import socket


class RequestHandler(http.server.BaseHTTPRequestHandler):
    def log_request(self, code='-', size='-'):
        pass  # Deaktiviert das Standard-Logging, da wir unsere eigene Ausgabe verwenden möchten

    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/plain')
        self.end_headers()

        url_path = self.path
        url_path_components = urllib.parse.urlsplit(url_path)
        path_after_log = x = urllib.parse.unquote(url_path_components.path.split("/log/", 1)[-1])
        
        if path_after_log and url_path.startswith("/log/"):
            timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            ip_address = self.client_address[0]
            try:
                hostname = socket.gethostbyaddr(ip_address)[0]
            except socket.herror:
                hostname = "Hostname not found"
            
            output = f"{hostname}@{ip_address} ({timestamp}): {path_after_log}"
            output = f"{timestamp}> {path_after_log}"
            print(output)
            #self.wfile.write(output.encode('utf-8'))
        else:
            self.wfile.write("Invalid URL Path".encode('utf-8'))

def run_server(port):
    with socketserver.TCPServer(("", port), RequestHandler) as httpd:
        print(f"Server läuft auf Port {port}")
        httpd.serve_forever()

if __name__ == "__main__":
    run_server(8032)
