#!/usr/bin/env python3
import os
import sys

def main():
    print("Content-Type: text/html")  # cabeçalho HTTP necessário
    print()  # Linha em branco termina os cabeçalhos

    # HTML com conteúdo dinâmico
    print(f"""<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
    <link rel="canonical" href="https://www.site1/index.html" />
    <link rel="shortcut icon" href="/favicon.ico">
    <title>Tutorial</title>
    <link rel="stylesheet" href="styles.css">
    <link href="https://fonts.googleapis.com/css2?family=Exo:wght@400;700&family=Ubuntu:wght@400;700&display=swap" rel="stylesheet">
</head>
<body>
    <div class="container-top">
        <header>
            <h2 class="site-logo">Cliva Webserv</h2>
            <h5 class="site-slogan">A única coisa especial aqui é sua avaliação</h5>
            <nav>
                <ul>
                    <li><a href="index.html">Início</a></li>
                    <li><a href="tutorial.html">Tutorial</a></li>
                    <li><a href="carregar.html">Carregar</a></li>
                    <li><a href="apagar.html">Apagar</a></li>
                </ul>
            </nav>
        </header>
        <div class="main-content">
            <h3>OI ENTREI NO PYTHON</h3>
            <p>BLABLABLABLABALBALBALBALBALBALBALBALBALABLABALBALBALABLBLABALBALBALABLAB</p>
        </div>
        <footer>
            <p>Copyright © 2024 Clara Franco & Ívany Pinheiro.</p>
        </footer>
    </div>
</body>
</html>""")

if __name__ == "__main__":
    main()
