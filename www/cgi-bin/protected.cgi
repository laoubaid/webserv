#!/bin/bash
# protected.cgi - Handle access to protected pages (like dashboard.html)

# Parse session_id from cookies
if [ -n "$HTTP_COOKIE" ]; then
    SESSION_ID=$(echo "$HTTP_COOKIE" | sed -n 's/.*session_id=\([^;]*\).*/\1/p')
else
    SESSION_ID=""
fi

# Check if session exists and is valid
if [ -n "$SESSION_ID" ] && [ "$SESSION_ID" != "" ]; then
    # Valid session - serve the dashboard page
    echo "Content-Type: text/html"
    echo ""
    
    # Read and serve dashboard.html
    cat << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Session Dashboard</title>
    <script>
        // Page served by server-side session validation
        console.log('Dashboard loaded with server-side session validation');
    </script>
</head>
<body>
    <h1>Protected Dashboard</h1>
    <p>This page was served after server-side session validation.</p>
    <p>Session ID: $SESSION_ID</p>
    <a href="/cgi-bin/logout.cgi">Logout</a>
</body>
</html>
EOF
else
    # No valid session - redirect to login
    echo "Status: 302 Found"
    echo "Location: /login.html"
    echo "Content-Type: text/html"
    echo ""
    echo "<html><body><p>Access denied. Redirecting to login...</p></body></html>"
fi
