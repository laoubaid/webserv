#!/usr/bin/env python3
import os
import sys
import urllib.parse

print("Content-Type: text/html")
print()

print("<html><head><title>POST Data Test</title></head><body>")
print("<h1>POST Data Test</h1>")

request_method = os.environ.get('REQUEST_METHOD', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')

print(f"<p><strong>Request Method:</strong> {request_method}</p>")
print(f"<p><strong>Content Length:</strong> {content_length}</p>")

if request_method == 'POST' and content_length.isdigit() and int(content_length) > 0:
    # Read POST data
    post_data = sys.stdin.read(int(content_length))
    print(f"<p><strong>Raw POST Data:</strong> {post_data}</p>")
    
    # Parse form data
    if post_data:
        params = urllib.parse.parse_qs(post_data)
        
        print("<h2>Parsed POST Parameters:</h2>")
        print("<ul>")
        for key, values in params.items():
            for value in values:
                print(f"<li><strong>{key}:</strong> {value}</li>")
        print("</ul>")
        
        # Echo back specific test parameters
        if 'username' in params:
            print(f"<p>Username: {params['username'][0]}</p>")
        if 'password' in params:
            print(f"<p>Password received: {'*' * len(params['password'][0])}</p>")
        if 'submit' in params:
            print(f"<p>Submit button: {params['submit'][0]}</p>")
    else:
        print("<p>No POST data received.</p>")
else:
    print("<p>No POST data received or wrong method.</p>")
    print("<p>Send POST data like: <code>username=testuser&password=secret123&submit=login</code></p>")

print("</body></html>")
