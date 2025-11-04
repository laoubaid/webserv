#!/usr/bin/env python3
import time
import os

print("Content-Type: text/plain; charset=utf-8")
print()

print("Plain Text CGI Test")
print("=" * 30)
print()
print(f"Current time: {time.strftime('%Y-%m-%d %H:%M:%S')}")
print(f"Port: {os.environ.get('SERVER_PORT', 'Unknown')}")
print(f"Method: {os.environ.get('REQUEST_METHOD', 'Unknown')}")
print()
print("This is plain text content.")
print("No HTML formatting here!")
print("Just simple, clean text output.")
print()
print("Features tested:")
print("- Plain text content type")
print("- UTF-8 encoding")
print("- Environment variable access")
print("- Timestamp generation")
