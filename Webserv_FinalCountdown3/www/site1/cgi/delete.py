import os, sys, cgi, datetime

method = os.environ['REQUEST_METHOD']
root_folder = os.environ['ROOT_FOLDER']

if method == 'GET':

    print("""
    <!DOCTYPE html>
    <html lang="pt-BR">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
        <link rel="canonical" href="/index.html" />
        <link rel="shortcut icon" href="/favicon.ico">
        <title>Cliva Website - Apagar</title>
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
                        <li><a href="/get.py">Tutorial</a></li>
                        <li><a href="/post.html">Carregar</a></li>
                        <li><a href="/delete.py">Apagar</a></li>
                    </ul>
                </nav>
            </header>
            <div class="main-content">
                <h3>Apague ficheiros carregados</h3>
                <p>No espaço abaixo, escreva o nome do ficheiro, e carregue no botão, se o arquivo existir, será apagado da memória do nosso servidor físico</p>
                <form id="delete-form">
                    <input type="text" name="filename" id="filename" class="input-text" placeholder="Digite o nome do arquivo aqui" required>
                    <button type="submit" class="button-link">DELETAR</button>
                </form>
                <p id="message"></p>
            </div>
        </div>
        <script>
            document.getElementById('delete-form').addEventListener('submit', function(event) {
                event.preventDefault();
                const filename = document.getElementById('filename').value;

                if (filename) {
                    fetch(`/upload/${filename}`, { method: 'DELETE' })
                        .then(response => {
                            if (response.status === 200) {
                                document.getElementById('message').textContent = `Arquivo ${filename} foi deletado.`;
                            } else {
                                document.getElementById('message').textContent = `Falha ao deletar o arquivo ${filename}.`;
                            }
                        })
                        .catch(error => {
                            document.getElementById('message').textContent = `Erro ao deletar o arquivo: ${error}`;
                        });
                } else {
                    alert('Por favor, insira o nome do arquivo.');
                }
            });
        </script>
    </body>
    </html>
    """)

if 'UPLOAD_PATH' in os.environ:
    folder = os.environ['UPLOAD_PATH']
else:
    folder = os.path.join(root_folder, 'upload')

if not os.path.exists(folder):
    print("<p>Upload folder is not set or not match.</p>")
    sys.exit(0)


sys.exit(0)