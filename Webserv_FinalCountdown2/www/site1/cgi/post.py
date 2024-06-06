#!/usr/bin/env python3

import os, sys, cgi, cgitb

cgitb.enable()

form = cgi.FieldStorage()
method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]

#print("Content-Type: text/html\n")
#print(f"<p>Método: {method}</p>")
#print(f"<p>Campos do formulário: {form.list}</p>")

if method == "POST":
    print("""
    <!DOCTYPE html>
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
        <link rel="canonical" href="/index.html" />
        <link rel="shortcut icon" href="/favicon.ico">
        <title>Cliva Website - Carregar</title>
        <link rel="stylesheet" href="/styles.css">
        <link href="https://fonts.googleapis.com/css2?family=Exo:wght@400;700&family=Ubuntu:wght@400;700&display=swap" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Major+Mono+Display&display=swap" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=DM+Mono:ital,wght@0,300;0,400;0,500;1,300;1,400;1,500&family=Major+Mono+Display&display=swap" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Libre+Barcode+39+Text&display=swap" rel="stylesheet">
        <link href="https://fonts.googleapis.com/css2?family=Foldit:wght@100..900&family=Libre+Barcode+39+Text&display=swap" rel="stylesheet">
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
                    </ul>
                </nav>
            </header>
            <div class="main-content">
    """)

    # Verificar se o formulário contém um campo de arquivo
    if "fileUpload" in form:
        file_item = form["fileUpload"]

        # Verificar se o arquivo foi enviado corretamente
        if file_item.file:
            filename = os.path.basename(file_item.filename)
            filepath = os.path.join(upload_dir, filename)
            try:
                with open(filepath, "wb") as f:
                    f.write(file_item.file.read())
                print(f"<h3>File '<b>{filename}</b>' carregado com sucesso para <h3>{filepath}</b>.</p>")
            except Exception as e:
                print(f"<h3>Erro ao carregar o arquivo: {e}</h3>")
        else:
            print("<h3>Erro ao carregar o arquivo: nenhum arquivo foi fornecido.</h3>")
    else:
        print("<h3>Nenhum arquivo foi carregado.</h3>")

    print("""
            </div>
            <footer>
                <p>Copyright © 2024 Clara Franco & Ívany Pinheiro.</p>
            </footer>
        </div>
    </body>
    </html>
    """)
else:
    print("Content-Type: text/html\n")
    print("<html><body><p>Erro: método HTTP não suportado.</p></body></html>")

sys.exit(0)