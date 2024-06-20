#!/usr/bin/env python3

import os, sys

method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]
#payload = os.environ["PAYLOAD"]

if not os.path.exists(upload_dir):
    os.makedirs(upload_dir)

types = {
    'plain/text': '.txt',
    'image/jpeg': '.jpg',
    'image/png': '.png',
    'application/pdf': '.pdf',
    'application/json': '.json',
    'application/octet-stream': '.bin',
    'video/mp4': '.mp4',
    'audio/mpeg': '.mp3'
}


if method == "POST":
    if 'CONTENT_TYPE' in os.environ:
        content_type = os.environ["CONTENT_TYPE"]
        if content_type in types:
            print("<html><body>")
            data = sys.stdin.buffer.read()
            filename = 'output' + types[content_type]
            filepath = os.path.join(upload_dir, filename)
            try:
                with open(filepath, "wb") as f:
                    f.write(data)
                print(f"<h3>Conteúdo gravado com sucesso no arquivo '<b>{filename}</b>' em <b>{filepath}</b>.</h3>")
            except Exception as e:
                print(f"<h3>Erro ao gravar no arquivo: {e}</h3>")
        else:
            print(f"<h3>Erro: Tipo de arquivo não suportado pelo CGI</h3>")
    else:
        print(f"<h3>Erro: Cabeçalho CONTENT_TYPE ausente</h3>")
else:
    print("<html><body><p>Erro: método HTTP não suportado.</p>")

print("""
        </div>
        <footer>
            <p>Copyright © 2024 Clara Franco & Ívany Pinheiro.</p>
        </footer>
    </div>
</body>
</html>
""")

sys.exit(0)
