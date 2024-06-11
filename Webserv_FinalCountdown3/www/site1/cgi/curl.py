#!/usr/bin/env python3

import os, sys

method = os.environ["REQUEST_METHOD"]
upload_dir = os.environ["UPLOAD_PATH"]
payload = os.environ["PAYLOAD"]

if method == "POST":
    print("<html><body>")
    filename = "curlposted.txt"
    filepath = os.path.join(upload_dir, filename)

    try:
        with open(filepath, "w") as f:
            f.write(payload)
        print(f"<h3>Conteúdo gravado com sucesso no arquivo '<b>{filename}</b>' em <b>{filepath}</b>.</h3>")
    except Exception as e:
        print(f"<h3>Erro ao gravar no arquivo: {e}</h3>")
else:
    print("Content-Type: text/html\n")
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