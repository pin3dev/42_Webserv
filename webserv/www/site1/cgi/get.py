#!/usr/bin/env python3

import os, sys, cgi
from datetime import datetime, timezone


method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]

def get_time_stamp():
    return datetime.now(timezone.utc).strftime("%a, %d %b %Y %H:%M:%S GMT")
if not upload_dir:
    current_time = datetime.now().strftime("%H%M%S")

content_len = 0
time_stamp = 0
status = ""
html_content = ""

if method == "GET":
    html_content = f"""
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
                <h3>TimeStamp</h3>
                <p>Abaixo esta o horario do request feito, aproveite para acabar essa avaliacao, e ir comer e relaxar,
                e nos deixar fazer o mesmo, obrigada!</p>
                <p>{upload_dir}</p>"""
    status = "200 OK"
else:
    status = "401 Unauthorized"
    html_content = """<!DOCTYPE html>\n<html lang="pt-BR">\n<body><p>Erro: método HTTP não suportado.</p></body></html>"""

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
