#!/usr/bin/env python

# import time

# Required CGI header



# print ("Status: 200 ok")
# print ("Content-Type: text/plain\n")

# Sleep for 5 seconds
# time.sleep(5)

# Print response
# print ("Hello!")
#!/usr/bin/env python3
import sys

# HTTP header for CGI
print("Content-Type: text/plain")
print()

# Read stdin line-by-line, stop when a line contains "END"
for line in sys.stdin:
    if "END" in line:
        break
    sys.stdout.write(line)

