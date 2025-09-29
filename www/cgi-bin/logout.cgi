#!/bin/bash

# logout.cgi - Handle user logout and clear session cookies

# Clear session cookies by setting them to expire in the past
echo "Status: 302 Found"
echo "Location: /login.html?logout=1"
echo "Content-Type: text/html"
echo "Set-Cookie: session_id=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT"
echo "Set-Cookie: login_time=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT"
echo "Set-Cookie: username=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT"
echo "Set-Cookie: user_pref=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT"
echo ""
echo "<html><body><p>Logged out successfully. Redirecting...</p></body></html>"
