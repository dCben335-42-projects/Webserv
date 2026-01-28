print("""HTTP/1.1 200 OK\r
Content-type: text/html\r
\r
\r
<html>
<head>
<title>CGI Script</title>
</head>
<body>
<h1>Hello from CGI Script</h1>
<p>This is a simple CGI script written in Python.</p>
<p>Request Method: {}</p>
<p>Environment Variables:</p>
<ul>
""".format(__import__('os').environ.get('REQUEST_METHOD', 'GET')))

for key, value in sorted(__import__('os').environ.items()):
    print("<li><strong>{}</strong>: {}</li>".format(key, value))
print("""
</ul>
</body>
</html>
""")
