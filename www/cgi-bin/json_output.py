#!/usr/bin/env python3
import json
import time
import os

print("Content-Type: application/json; charset=utf-8")
print()

response_data = {
    "status": "success",
    "message": "JSON output test successful",
    "timestamp": int(time.time()),
    "server_info": {
        "server_name": os.environ.get('SERVER_NAME', 'Unknown'),
        "server_port": os.environ.get('SERVER_PORT', 'Unknown'),
        "request_method": os.environ.get('REQUEST_METHOD', 'Unknown')
    },
    "test_data": {
        "numbers": [1, 2, 3, 4, 5],
        "boolean": True,
        "null_value": None,
        "nested_object": {
            "key1": "value1",
            "key2": "value2"
        }
    }
}

print(json.dumps(response_data, indent=2))
