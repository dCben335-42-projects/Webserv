#!/usr/bin/env python3
import os
import sys

request_method = os.environ.get('REQUEST_METHOD', 'GET')

body = sys.stdin.read().strip()

print("HTTP/1.1 200 OK\r")
print("Content-type: text/html\r")
print("\r")
print("\r")
print("<html>")
print("<head>")
print("<title>CGI Script - Echo Body</title>")
print("</head>")
print("<body>")
print("<h1>Hello from CGI Script (NPH)</h1>")
print(f"<p>Request Method: {request_method}</p>")
print("<p>Body received from stdin:</p>")
print(f"<pre>{body}</pre>")
print("<p>Environment Variables:</p>")
print("<ul>")

for key, value in sorted(os.environ.items()):
    print(f"<li><strong>{key}</strong>: {value}</li>")

print("""
</ul>
</body>
</html>
""")
