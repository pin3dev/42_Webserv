#!/usr/bin/env python3

import os, sys, cgi, cgitb
from datetime import datetime, timezone

cgitb.enable()

form = cgi.FieldStorage()
method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]
#agent = os.environ["USER_AGENT"]
#ischunked = os.environ["CHUNKED_CONTENT"]

def get_time_stamp():
    return datetime.now(timezone.utc).strftime("%a, %d %b %Y %H:%M:%S GMT")

if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

#if 'USER_AGENT' in os.environ and 'curl' in os.environ['USER_AGENT']:
#    print("Content-Type: text/html\n")
#    print("<html><body><p>Erro: para POST com curl usar o arquivo curl.py</p></body></html>")
#    sys.exit(0)

#if 'CHUNKED_CONTENT' in os.environ and '100-continue' in os.environ['CHUNKED_CONTENT']:
#    print("Content-Type: text/html\n")
#    print("<html><body><p>Erro: para POST chunked usar o arquivo curl.py</p></body></html>")
#    sys.exit(0)
content_len = 0
time_stamp = 0
html_content = ""
status = "200 OK"

if 'USER_AGENT' in os.environ and 'curl' in os.environ['USER_AGENT']:
    status = "401 Unauthorized"
    html_content += """<!DOCTYPE html><html lang="pt-BR"><head><body><h3>Erro: para POST com curl usar o arquivo curl.py</h3>"""
elif method == "POST":
    html_content = """
    <!DOCTYPE html>
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
        <link rel="canonical" href="/index.html" />
        <link rel="shortcut icon" href="/favicon.ico">
        <title>Cliva Website - Carregar</title>
        <link rel="stylesheet" href="/styles.css">
        <link href="https://fonts.googleapis.com/css2?family=Exo:wght@400;700&family=Ubuntu:wght@400;700&display=swap"
            rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Major+Mono+Display&display=swap" rel="stylesheet">
        <link
            href="https://fonts.googleapis.com/css2?family=DM+Mono:ital,wght@0,300;0,400;0,500;1,300;1,400;1,500&family=Major+Mono+Display&display=swap"
            rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Libre+Barcode+39+Text&display=swap" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Foldit:wght@100..900&family=Libre+Barcode+39+Text&display=swap"
            rel="stylesheet">
    </head>
    <body>
        <div class="container-top">
            <header>
                <h2 class="site-logo">Cliva Webserv</h2>
                <h5 class="site-slogan">A única coisa especial aqui é sua avaliação</h5>
                <nav>
                    <ul>
                        <li><a href="/index.html">Início</a></li>
                        <li><a href="/tutorial.html">Tutorial</a></li>
                        <li><a href="/carregar.html">Carregar</a></li>
                        <li><a href="/apagar.html">Apagar</a></li>
                        <li><a href="/timestamp.html">Tempo</a></li>
                    </ul>
                </nav>
            </header>
            <div class="main-content">
    """

    if "fileUpload" in form:
        file_item = form["fileUpload"]

        if isinstance(file_item, list):
            file_item = file_item[0]
        if file_item.filename:
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(upload_dir, filename)
            try:
                with open(filepath, "wb") as f:
                    while True:
                        chunk = file_item.file.read(1024)
                        if not chunk:
                            break
                        f.write(chunk)
                status = "200 OK"
                html_content += f"<h3>File '<b>{filename}</b>' carregado com sucesso para <b>{filepath}</b>.</h3>"
            except Exception as e:
                status = "500 Internal Server Error"
                html_content += f"<h3>Erro ao carregar o arquivo: {e}</h3>"
        else:
            status = "400 Bad Request"
            html_content += "<h3>Erro ao carregar o arquivo: nenhum arquivo foi fornecido.</h3>"
    else:
        status = "400 Bad Request"
        html_content += "<h3>Nenhum arquivo foi carregado.</h3>"
else:
    status = "401 Unauthorized"
    html_content += """<!DOCTYPE html><html lang="pt-BR"><head><body><h3>Erro: método HTTP não suportado.</h3>"""

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

#print(response_headers)
#print(html_content)

sys.exit(0)