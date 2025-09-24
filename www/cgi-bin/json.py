#!/usr/bin/env python3
import os
import sys
import json

print("Content-Type: application/json")
print()

request_method = os.environ.get('REQUEST_METHOD', '')
content_type = os.environ.get('CONTENT_TYPE', '')
content_length = os.environ.get('CONTENT_LENGTH', '0')

response = {
    'status': 'success',
    'request_method': request_method,
    'content_type': content_type,
    'content_length': content_length,
    'received_data': None,
    'message': ''
}

if request_method == 'POST' and content_length.isdigit() and int(content_length) > 0:
    # Read POST data
    post_data = sys.stdin.read(int(content_length))
    
    if 'application/json' in content_type:
        try:
            # Parse JSON data
            json_data = json.loads(post_data)
            response['received_data'] = json_data
            response['message'] = f"JSON data received successfully. Message: {json_data.get('message', 'No message')}"
            
            # Echo back the timestamp if present
            if 'timestamp' in json_data:
                response['timestamp_received'] = json_data['timestamp']
                
        except json.JSONDecodeError as e:
            response['status'] = 'error'
            response['message'] = f"Invalid JSON: {str(e)}"
            response['raw_data'] = post_data
    else:
        response['received_data'] = post_data
        response['message'] = "Non-JSON POST data received"
else:
    response['status'] = 'error'
    response['message'] = 'No POST data received or wrong method'

print(json.dumps(response, indent=2))
