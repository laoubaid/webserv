#!/usr/bin/env python3
import os
import random
import string
from datetime import datetime

COOKIE_FILE = "sessions.txt"
COOKIE_LIFETIME = 30

def generate_cookie():
    """Generate a random 16-character cookie"""
    return ''.join(random.choices(string.ascii_letters + string.digits, k=16))

def read_cookies():
    """Read all cookies from file"""
    cookies = {}
    try:
        with open(COOKIE_FILE, 'r') as f:
            for line in f:
                line = line.strip()
                if line:
                    parts = line.split(',')
                    if len(parts) == 3:
                        cookie = parts[0].strip().strip('"')
                        creation_time = parts[1].strip().strip('"')
                        cookies[cookie] = creation_time
    except FileNotFoundError:
        pass
    return cookies

def save_cookie(cookie, creation_time):
    """Save a new cookie to file"""
    with open(COOKIE_FILE, 'a') as f:
        f.write(f'"{cookie}", "{creation_time}"\n')

def is_cookie_alive(creation_time_str):
    """Check if cookie is still alive (within 5 minutes)"""
    try:
        creation_time = datetime.strptime(creation_time_str, "%Y-%m-%d %H:%M:%S")
        now = datetime.now()
        diff = (now - creation_time).total_seconds()
        return diff < COOKIE_LIFETIME
    except:
        return False

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
    
    # Read existing cookies
    saved_cookies = read_cookies()
    
    # Check if user has a cookie
    if user_cookie and user_cookie in saved_cookies:
        creation_time = saved_cookies[user_cookie]
        if is_cookie_alive(creation_time):
            # Cookie exists and is alive
            print("Content-Type: text/html\r")
            print("\r")
            print("<html><body>")
            print("<h1>Hello again my friend!</h1>")
            print(f"<p>Your session is still active.</p>")
            print(f"<p>Cookie: {user_cookie}</p>")
            print(f"<p>Created: {creation_time}</p>")
            print("</body></html>")
        else:
            # Cookie exists but is dead
            new_cookie = generate_cookie()
            creation_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            save_cookie(new_cookie, creation_time)
            
            print(f"Set-Cookie: session_id={new_cookie}; Path=/\r")
            print("Content-Type: text/html\r")
            print("\r")
            print("<html><body>")
            print("<h1>Session ended</h1>")
            print(f"<p>Your previous session has expired.</p>")
            print(f"<p>New cookie created: {new_cookie}</p>")
            print(f"<p>Created: {creation_time}</p>")
            print("</body></html>")
    else:
        # No cookie or cookie not found - create new one
        new_cookie = generate_cookie()
        creation_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        save_cookie(new_cookie, creation_time)
        
        print(f"Set-Cookie: session_id={new_cookie}; Path=/\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body>")
        print("<h1>Welcome!</h1>")
        print(f"<p>First time here? Cookie created!</p>")
        print(f"<p>Your cookie: {new_cookie}</p>")
        print(f"<p>Created: {creation_time}</p>")
        print(f"<p>Expires in 5 minutes</p>")
        print("</body></html>")

if __name__ == "__main__":
    main()
