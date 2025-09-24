#!/usr/bin/env python3
import os
import urllib.parse

print("Content-Type: text/html")
print()

query_string = os.environ.get('QUERY_STRING', '')
print("<html><head><title>GET Parameters Test</title></head><body>")
print("<h1>GET Parameters Test</h1>")

if query_string:
    print(f"<p><strong>Query String:</strong> {query_string}</p>")
    
    # Parse query string
    params = urllib.parse.parse_qs(query_string)
    
    print("<h2>Parsed Parameters:</h2>")
    print("<ul>")
    for key, values in params.items():
        for value in values:
            print(f"<li><strong>{key}:</strong> {value}</li>")
    print("</ul>")
    
    # Echo back specific test parameters
    if 'name' in params:
        print(f"<p>Hello, {params['name'][0]}!</p>")
    if 'id' in params:
        print(f"<p>Your ID is: {params['id'][0]}</p>")
    if 'action' in params:
        print(f"<p>Action requested: {params['action'][0]}</p>")
else:
    print("<p>No query parameters received.</p>")
    print("<p>Try: <code>?name=test_user&id=123&action=get_test</code></p>")

print("</body></html>")
