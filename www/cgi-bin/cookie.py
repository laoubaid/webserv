#!/usr/bin/env python3
import os
import random
import string

def generate_cookie():
    """Generate a random 16-character cookie"""
    return ''.join(random.choices(string.ascii_letters + string.digits, k=16))

def get_cookie_from_request():
    """Get user_token cookie from HTTP_COOKIE environment variable"""
    cookie_header = os.environ.get('HTTP_COOKIE', '')
    if not cookie_header:
        return None
    
    cookies = cookie_header.split(';')
    for cookie in cookies:
        cookie = cookie.strip()
        if cookie.startswith('user_token='):
            return cookie.split('=', 1)[1]
    return None

def main():
    # Check if user clicked login button
    query_string = os.environ.get('QUERY_STRING', '')
    
    # Get cookie from request
    user_token = get_cookie_from_request()
    
    # If login action and no token, set cookie and redirect
    if 'action=login' in query_string and not user_token:
        new_token = generate_cookie()
        print(f"Set-Cookie: user_token={new_token}; Path=/; Max-Age=30\r")
        print("Status: 302 Found\r")
        print(f"Location: {os.environ.get('SCRIPT_NAME', '/cgi-bin/cookie.py')}\r")
        print("Content-Type: text/html\r")
        print("\r")
        return
    
    print("Content-Type: text/html\r")
    print("\r")
    
    if user_token:
        # User is logged in
        script_name = os.environ.get('SCRIPT_NAME', '/cgi-bin/cookie.py')
        print(f"""
<html>
<head>
    <title>Logged In</title>
    <style>
        body {{ font-family: Arial; text-align: center; padding: 50px; background: #f0f0f0; }}
        .container {{ background: white; padding: 40px; border-radius: 10px; max-width: 500px; margin: 0 auto; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        .message {{ font-size: 24px; color: #4CAF50; margin: 20px; }}
        .info {{ color: #666; margin: 10px; }}
        .token {{ background: #f5f5f5; padding: 10px; border-radius: 5px; font-family: monospace; word-break: break-all; }}
        a {{ color: #2196F3; text-decoration: none; }}
        a:hover {{ text-decoration: underline; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="message"> You are logged in!</div>
        <p class="info">Your session will expire in 30 seconds.</p>
        <div class="token">Token: {user_token}</div>
        <p style="margin-top: 30px;"><a href="{script_name}">Refresh Page</a></p>
    </div>
</body>
</html>
        """)
    else:
        # User is not logged in - show login button
        script_name = os.environ.get('SCRIPT_NAME', '/cgi-bin/cookie.py')
        print(f"""
<html>
<head>
    <title>Login</title>
    <style>
        body {{ font-family: Arial; text-align: center; padding: 50px; background: #f0f0f0; }}
        .container {{ background: white; padding: 40px; border-radius: 10px; max-width: 500px; margin: 0 auto; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }}
        h1 {{ color: #333; }}
        .login-btn {{
            background: #4CAF50;
            color: white;
            border: none;
            padding: 15px 32px;
            font-size: 16px;
            cursor: pointer;
            border-radius: 4px;
            margin-top: 20px;
        }}
        .login-btn:hover {{ background: #45a049; }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome!</h1>
        <p>Click the button to login</p>
        <form method="GET" action="{script_name}">
            <input type="hidden" name="action" value="login">
            <button type="submit" class="login-btn">Login</button>
        </form>
    </div>
</body>
</html>
        """)

if __name__ == "__main__":
    main()
    