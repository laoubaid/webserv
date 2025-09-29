#!/bin/bash
# check_session.cgi - Check if user has valid session

echo "Content-Type: application/json"
echo ""

# Parse session_id from cookies
if [ -n "$HTTP_COOKIE" ]; then
    SESSION_ID=$(echo "$HTTP_COOKIE" | sed -n 's/.*session_id=\([^;]*\).*/\1/p')
    LOGIN_TIME=$(echo "$HTTP_COOKIE" | sed -n 's/.*login_time=\([^;]*\).*/\1/p')
    USERNAME=$(echo "$HTTP_COOKIE" | sed -n 's/.*username=\([^;]*\).*/\1/p')
else
    SESSION_ID=""
    LOGIN_TIME=""
    USERNAME=""
fi

# Check if session exists and is valid
if [ -n "$SESSION_ID" ] && [ "$SESSION_ID" != "" ] && [ -n "$LOGIN_TIME" ] && [ "$LOGIN_TIME" != "" ]; then
    # Session exists - you could add more validation here (expiry check, etc.)
    echo "{\"authenticated\": true, \"session_valid\": true, \"session_id\": \"$SESSION_ID\", \"username\": \"$USERNAME\", \"message\": \"Valid session\"}"
else
    # No session found
    echo "{\"authenticated\": false, \"session_valid\": false, \"cookies_received\": \"$HTTP_COOKIE\", \"message\": \"No valid session found\"}"
fi