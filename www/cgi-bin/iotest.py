#!/usr/bin/env python3
import sys, os

# Required header
print ("HTTP/1.1 200 OK")
print("Content-Type: text/plain\n")

# Read POST body if present
length = int(os.environ.get("CONTENT_LENGTH", 0) or 0)

if length > 0:
	body = sys.stdin.read(length)
	print("POST body:")
	# print(length)
	print(body)
else:
	print("No POST body.")
