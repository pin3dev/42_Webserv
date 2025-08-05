#!/usr/bin/env python3

import os, sys
from datetime import datetime, timezone

method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]
#payload = os.environ["PAYLOAD"]

def get_time_stamp():
    return datetime.now(timezone.utc).strftime("%a, %d %b %Y %H:%M:%S GMT")

if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

types = {
    'text/plain': '.txt',
    'image/jpeg': '.jpg',
    'image/png': '.png',
    'application/pdf': '.pdf',
    'application/json': '.json',
    'application/octet-stream': '.bin',
    'video/mp4': '.mp4',
    'audio/mpeg': '.mp3'
}

content_len = 0
time_stamp = 0
status = ""
html_content = """<!DOCTYPE html>\n<html lang="pt-BR"><head></head>"""

if method == "POST":
    if 'CONTENT_TYPE' in os.environ:
        content_type = os.environ["CONTENT_TYPE"]
        if content_type in types:
            data = sys.stdin.buffer.read()
            filename = 'curl' + types[content_type]
            filepath = os.path.join(upload_dir, filename)
            try:
                with open(filepath, "wb") as f:
                    f.write(data)
                status = "200 OK"
                html_content += f"<h3>Conteúdo gravado com sucesso no arquivo '<b>{filename}</b>' em <b>{filepath}</b>.</h3>"

            except Exception as e:
                status = "500 Internal Server Error"
                html_content += f"<h3>Erro ao gravar no arquivo: {e}</h3>"
        else:
            status = "400 Bad Request"
            html_content += f"<h3>Erro: Tipo de arquivo não suportado pelo CGI</h3>"
    else:
        status = "400 Bad Request"
        html_content += f"<h3>Erro: Cabeçalho CONTENT_TYPE ausente</h3>"
else:
    status = "401 Unauthorized"
    html_content += "<html><body><p>Erro: método HTTP não suportado.</p>"

html_content += """
        </div>
        <footer>
            <p>Copyright © 2024 Clara Franco & Ívany Pinheiro.</p>
        </footer>
    </div>
</body>
</html>
"""

time_stamp = get_time_stamp()
content_len = len(html_content.encode('utf-8'))

response_headers = (
    f"HTTP/1.1 {status}\r\n"
    f"Date: {time_stamp}\r\n"
    "Server: Webserv/1.0.0 (Linux)\r\n"
    "Connection: keep-alive\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    f"Content-Length: {content_len}\r\n"
    "\r\n"
)

sys.stdout.write(response_headers)
sys.stdout.write(html_content)
sys.stdout.flush()

sys.exit(0)
