#!/bin/bash
# session_info.cgi - Display session information as JSON

echo "Content-Type: application/json"
echo "Access-Control-Allow-Origin: *"
echo ""

# Parse cookies from HTTP_COOKIE environment variable
if [ -n "$HTTP_COOKIE" ]; then
    SESSION_ID=$(echo "$HTTP_COOKIE" | sed -n 's/.*session_id=\([^;]*\).*/\1/p')
    LOGIN_TIME=$(echo "$HTTP_COOKIE" | sed -n 's/.*login_time=\([^;]*\).*/\1/p')
    USERNAME=$(echo "$HTTP_COOKIE" | sed -n 's/.*username=\([^;]*\).*/\1/p')
else
    SESSION_ID=""
    LOGIN_TIME=""
    USERNAME=""
fi

# Convert login_time to readable format if it exists
if [ -n "$LOGIN_TIME" ] && [ "$LOGIN_TIME" != "" ]; then
    LOGIN_TIME_READABLE=$(date -d "@$LOGIN_TIME" 2>/dev/null || echo "Invalid time")
else
    LOGIN_TIME_READABLE="Not available"
fi

# Get client info
CLIENT_IP=${REMOTE_ADDR:-"Unknown"}
USER_AGENT=${HTTP_USER_AGENT:-"Unknown"}

# Output JSON response
cat << EOF
{
    "session": {
        "session_id": "${SESSION_ID:-"No session"}",
        "username": "${USERNAME:-"Not logged in"}",
        "login_time": "$LOGIN_TIME_READABLE",
        "login_timestamp": "${LOGIN_TIME:-"0"}"
    },
    "client": {
        "ip_address": "$CLIENT_IP",
        "user_agent": "${USER_AGENT:0:100}",
        "request_time": "$(date)"
    },
    "cookies": {
        "raw": "${HTTP_COOKIE:-"No cookies"}"
    },
    "server": {
        "timestamp": "$(date +%s)",
        "server_name": "${SERVER_NAME:-"localhost"}",
        "server_port": "${SERVER_PORT:-"8080"}"
    }
}
EOF