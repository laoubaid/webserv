#!/bin/bash
# login.cgi - Handle user login and set session cookie

# Read POST data
read -n $CONTENT_LENGTH POST_DATA

# Parse form data
USERNAME=$(echo "$POST_DATA" | sed -n 's/.*username=\([^&]*\).*/\1/p' | sed 's/+/ /g')
PASSWORD=$(echo "$POST_DATA" | sed -n 's/.*password=\([^&]*\).*/\1/p' | sed 's/+/ /g')

# URL decode (basic)
USERNAME=$(echo -e "${USERNAME//%/\\x}")
PASSWORD=$(echo -e "${PASSWORD//%/\\x}")

# Generate session ID
SESSION_ID=$(date +%s | sha256sum | head -c 32)
LOGIN_TIME=$(date +%s)

# Simple validation (any non-empty username/password works for testing)
if [ -n "$USERNAME" ] && [ -n "$PASSWORD" ]; then
    # Login successful - set session cookie and redirect to dashboard
    echo "Status: 302 Found"
    echo "Location: /dashboard.html"
    echo "Content-Type: text/html"
    echo "Set-Cookie: session_id=$SESSION_ID; Path=/; HttpOnly; SameSite=Strict; Max-Age=3600"
    echo "Set-Cookie: login_time=$LOGIN_TIME; Path=/; HttpOnly; Max-Age=3600"
    echo "Set-Cookie: username=$USERNAME; Path=/; Max-Age=3600"
    echo ""
    echo "<html><body>"
    echo "<p>Login successful. Session ID: $SESSION_ID</p>"
    echo "<p>Redirecting to dashboard...</p>"
    echo "<script>setTimeout(function() { window.location.href = '/dashboard.html'; }, 1000);</script>"
    echo "</body></html>"
else
    # Login failed - redirect back to login with error
    echo "Status: 302 Found"
    echo "Location: /login.html?error=1"
    echo "Content-Type: text/html"
    echo ""
    echo "<html><body><p>Login failed. Redirecting...</p></body></html>"
fi