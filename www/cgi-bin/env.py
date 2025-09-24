#!/usr/bin/env python3
import os

print("Content-Type: text/plain")
print()

print("CGI Environment Variables:")
print("=" * 40)

# Common CGI environment variables
cgi_vars = [
    'REQUEST_METHOD', 'QUERY_STRING', 'CONTENT_TYPE', 'CONTENT_LENGTH',
    'HTTP_USER_AGENT', 'HTTP_ACCEPT', 'HTTP_HOST', 'HTTP_REFERER',
    'SERVER_NAME', 'SERVER_PORT', 'SERVER_SOFTWARE', 'SERVER_PROTOCOL',
    'SCRIPT_NAME', 'PATH_INFO', 'REMOTE_ADDR', 'REMOTE_HOST'
]

for var in cgi_vars:
    value = os.environ.get(var, 'Not set')
    print(f"{var}={value}")

print("\nAll Environment Variables:")
print("=" * 40)
for key, value in sorted(os.environ.items()):
    if key.startswith(('HTTP_', 'SERVER_', 'REQUEST_', 'CONTENT_', 'SCRIPT_', 'PATH_', 'QUERY_', 'REMOTE_')):
        print(f"{key}={value}")
