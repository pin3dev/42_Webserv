#!/usr/bin/python3

print("HTTP/1.1 200 OK\nContent-type: text/html\n")
print("""
<html>
<head>
<title>Hello World</title>
<style>
    body {
        height: 100vh;
        margin: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        background: white;
        color: black;
        font-size: 4em;
        font-family: Arial, sans-serif;
    }
</style>
</head>
<body>
    <div>HELLO WORLD</div>
</body>
</html>
""")
