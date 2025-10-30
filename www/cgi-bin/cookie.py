#!/usr/bin/env python3
import os
import random
import string
from datetime import datetime, timedelta

def generate_cookie():
    """Generate a random 16-character cookie"""
    return ''.join(random.choices(string.ascii_letters + string.digits, k=16))

def get_cookie_from_request():
    """Get session_id cookie from HTTP_COOKIE environment variable"""
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    if not cookie_header:
        return None
    
    cookies = cookie_header.split(';')
    for cookie in cookies:
        cookie = cookie.strip()
        if cookie.startswith('session_id='):
            return cookie.split('=', 1)[1]
    return None

def main():
    # Get cookie from request
    user_cookie = get_cookie_from_request()
    
    if user_cookie:
        # User has a cookie - they're returning
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body>")
        print("<h1>Hello again my friend!</h1>")
        print(f"<p>Your session is still active.</p>")
        print(f"<p>Cookie: {user_cookie}</p>")
        print("<p>This cookie will expire in 30 seconds from first visit.</p>")
        print("<p><a href='/cgi-bin/cookie.py'>Refresh</a></p>")
        print("</body></html>")
    else:
        # No cookie - first time or expired
        new_cookie = generate_cookie()
        
        # Set cookie with 30 second max-age
        print(f"Set-Cookie: session_id={new_cookie}; Path=/; Max-Age=30\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body>")
        print("<h1>Welcome!</h1>")
        print(f"<p>First time here or your session expired!</p>")
        print(f"<p>New cookie created: {new_cookie}</p>")
        print(f"<p>This cookie will expire in 30 seconds.</p>")
        print("<p><a href='/cgi-bin/cookie.py'>Refresh to test</a></p>")
        print("</body></html>")

if __name__ == "__main__":
    main()