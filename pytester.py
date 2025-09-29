#!/usr/bin/env python3
"""
Web Server CGI Tester
Tests CGI functionality on a web server including script execution, 
environment variables, POST/GET handling, and error conditions.
"""

import requests
import json
import time
import os
import sys
from urllib.parse import urlencode
from typing import Dict, List, Tuple, Optional

class CGITester:
    def __init__(self, base_url: str, timeout: int = 10):
        """
        Initialize the CGI tester
        
        Args:
            base_url: Base URL of the web server (e.g., 'http://localhost:8080')
            timeout: Request timeout in seconds
        """
        self.base_url = base_url.rstrip('/')
        self.timeout = timeout
        self.session = requests.Session()
        self.results = []
    
    def log_result(self, test_name: str, status: str, message: str, details: Dict = None):
        """Log test result"""
        result = {
            'test': test_name,
            'status': status,
            'message': message,
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'details': details or {}
        }
        self.results.append(result)
        
        # Print result
        status_symbol = "✓" if status == "PASS" else "✗" if status == "FAIL" else "!"
        print(f"{status_symbol} {test_name}: {message}")
        if details:
            for key, value in details.items():
                print(f"    {key}: {value}")
    
    def test_basic_cgi_execution(self, cgi_path: str = '/cgi-bin/test.py'):
        """Test basic CGI script execution"""
        test_name = "Basic CGI Execution"
        try:
            url = f"{self.base_url}{cgi_path}"
            response = self.session.get(url, timeout=self.timeout)
            
            if response.status_code == 200:
                self.log_result(test_name, "PASS", "CGI script executed successfully", {
                    'status_code': response.status_code,
                    'content_type': response.headers.get('content-type', 'Not specified'),
                    'response_length': len(response.text)
                })
                return True
            else:
                self.log_result(test_name, "FAIL", f"HTTP {response.status_code}", {
                    'status_code': response.status_code,
                    'response': response.text[:200] + '...' if len(response.text) > 200 else response.text
                })
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_environment_variables(self, cgi_path: str = '/cgi-bin/env.py'):
        """Test CGI environment variable passing"""
        test_name = "CGI Environment Variables"
        try:
            url = f"{self.base_url}{cgi_path}"
            headers = {
                'User-Agent': 'CGI-Tester/1.0',
                'Accept': 'text/html,application/json',
                'Custom-Header': 'test-value'
            }
            
            response = self.session.get(url, headers=headers, timeout=self.timeout)
            
            if response.status_code == 200:
                # Look for common CGI environment variables in response
                content = response.text.lower()
                env_vars_found = []
                expected_vars = ['request_method', 'query_string', 'http_user_agent', 
                               'server_name', 'server_port', 'content_type']
                
                for var in expected_vars:
                    if var in content:
                        env_vars_found.append(var)
                
                if len(env_vars_found) >= 3:  # At least 3 env vars should be present
                    self.log_result(test_name, "PASS", "Environment variables accessible", {
                        'found_vars': ', '.join(env_vars_found),
                        'total_found': len(env_vars_found)
                    })
                    return True
                else:
                    self.log_result(test_name, "WARN", "Few environment variables found", {
                        'found_vars': ', '.join(env_vars_found) if env_vars_found else 'None',
                        'response_sample': response.text[:300]
                    })
                    return False
            else:
                self.log_result(test_name, "FAIL", f"HTTP {response.status_code}")
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_get_parameters(self, cgi_path: str = '/cgi-bin/params.py'):
        """Test GET parameter handling in CGI"""
        test_name = "CGI GET Parameters"
        try:
            params = {'name': 'test_user', 'id': '123', 'action': 'get_test'}
            url = f"{self.base_url}{cgi_path}"
            
            response = self.session.get(url, params=params, timeout=self.timeout)
            
            if response.status_code == 200:
                # Check if parameters are reflected in response
                content = response.text.lower()
                params_found = sum(1 for value in params.values() if str(value).lower() in content)
                
                if params_found >= 2:  # At least 2 parameters should be processed
                    self.log_result(test_name, "PASS", "GET parameters processed", {
                        'parameters_sent': len(params),
                        'parameters_found': params_found,
                        'query_string': urlencode(params)
                    })
                    return True
                else:
                    self.log_result(test_name, "WARN", "Parameters not found in response", {
                        'sent_params': params,
                        'response_sample': response.text[:200]
                    })
                    return False
            else:
                self.log_result(test_name, "FAIL", f"HTTP {response.status_code}")
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_post_data(self, cgi_path: str = '/cgi-bin/post.py'):
        """Test POST data handling in CGI"""
        test_name = "CGI POST Data"
        try:
            url = f"{self.base_url}{cgi_path}"
            post_data = {'username': 'testuser', 'password': 'secret123', 'submit': 'login'}
            
            response = self.session.post(url, data=post_data, timeout=self.timeout)
            
            if response.status_code == 200:
                # Check if POST data is reflected in response
                content = response.text.lower()
                data_found = sum(1 for value in post_data.values() if str(value).lower() in content)
                
                if data_found >= 1:  # At least 1 POST field should be processed
                    self.log_result(test_name, "PASS", "POST data processed", {
                        'fields_sent': len(post_data),
                        'fields_found': data_found,
                        'content_type': response.headers.get('content-type', 'Not specified')
                    })
                    return True
                else:
                    self.log_result(test_name, "WARN", "POST data not found in response", {
                        'sent_data': list(post_data.keys()),
                        'response_sample': response.text[:200]
                    })
                    return False
            else:
                self.log_result(test_name, "FAIL", f"HTTP {response.status_code}")
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_json_post(self, cgi_path: str = '/cgi-bin/json_test.py'):
        """Test JSON POST data handling in CGI"""
        test_name = "CGI JSON POST"
        try:
            url = f"{self.base_url}{cgi_path}"
            json_data = {'message': 'Hello CGI', 'timestamp': int(time.time()), 'test': True}
            json_string = json.dumps(json_data)
            
            response = self.session.post(
                url, 
                data=json_string,
                headers={
                    'Content-Type': 'application/json',
                    'Content-Length': str(len(json_string.encode('utf-8')))
                },
                timeout=self.timeout
            )
            
            if response.status_code == 200:
                content = response.text.lower()
                # Look for JSON-related content
                if 'json' in content or 'hello cgi' in content or str(json_data['timestamp']) in content:
                    self.log_result(test_name, "PASS", "JSON POST data processed", {
                        'content_type_sent': 'application/json',
                        'data_size': len(json_string),
                        'response_type': response.headers.get('content-type', 'Not specified')
                    })
                    return True
                else:
                    self.log_result(test_name, "WARN", "JSON data not processed", {
                        'response_sample': response.text[:200]
                    })
                    return False
            else:
                self.log_result(test_name, "FAIL", f"HTTP {response.status_code}", {
                    'status_code': response.status_code,
                    'response': response.text[:200] + '...' if len(response.text) > 200 else response.text
                })
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_error_handling(self, cgi_path: str = '/cgi-bin/nonexistent.py'):
        """Test CGI error handling for non-existent scripts"""
        test_name = "CGI Error Handling"
        try:
            url = f"{self.base_url}{cgi_path}"
            response = self.session.get(url, timeout=self.timeout)
            
            if response.status_code == 404:
                self.log_result(test_name, "PASS", "Proper 404 for non-existent CGI", {
                    'status_code': response.status_code
                })
                return True
            elif response.status_code == 500:
                self.log_result(test_name, "WARN", "500 error (check server logs)", {
                    'status_code': response.status_code,
                    'response_sample': response.text[:200]
                })
                return False
            else:
                self.log_result(test_name, "WARN", f"Unexpected status code: {response.status_code}")
                return False
                
        except requests.exceptions.RequestException as e:
            self.log_result(test_name, "FAIL", f"Request failed: {str(e)}")
            return False
    
    def test_cgi_content_types(self, cgi_paths: Dict[str, str] = None):
        """Test different content types from CGI scripts"""
        if cgi_paths is None:
            cgi_paths = {
                'HTML': '/cgi-bin/html.php',
                'JSON': '/cgi-bin/json_output.py',
                'Plain Text': '/cgi-bin/text.py'
            }
        
        for content_type, path in cgi_paths.items():
            test_name = f"CGI Content Type - {content_type}"
            try:
                url = f"{self.base_url}{path}"
                response = self.session.get(url, timeout=self.timeout)
                
                if response.status_code == 200:
                    content_type_header = response.headers.get('content-type', '').lower()
                    self.log_result(test_name, "PASS", "Content served successfully", {
                        'expected': content_type,
                        'content_type_header': content_type_header,
                        'response_length': len(response.text)
                    })
                else:
                    self.log_result(test_name, "WARN", f"HTTP {response.status_code} - script may not exist")
                    
            except requests.exceptions.RequestException as e:
                self.log_result(test_name, "WARN", f"Request failed: {str(e)}")
    
    def run_all_tests(self, custom_cgi_paths: Dict[str, str] = None, stop_on_fail: bool = True):
        """Run all CGI tests"""
        print(f"Starting CGI tests for: {self.base_url}")
        print("=" * 50)
        
        # Use custom paths if provided, otherwise use defaults
        paths = custom_cgi_paths or {}
        
        # Run basic tests one at a time, stopping on failure if requested
        tests = [
            ('basic', self.test_basic_cgi_execution, paths.get('basic', '/cgi-bin/test.py')),
            ('env', self.test_cgi_environment_variables, paths.get('env', '/cgi-bin/env.py')),
            ('get', self.test_cgi_get_parameters, paths.get('get', '/cgi-bin/params.py')),
            ('post', self.test_cgi_post_data, paths.get('post', '/cgi-bin/post.py')),
            ('json', self.test_cgi_json_post, paths.get('json', '/cgi-bin/json_test.py')),
            ('error', self.test_cgi_error_handling, paths.get('error', '/cgi-bin/nonexistent.py')),
        ]
        
        for test_name, test_func, test_path in tests:
            result = test_func(test_path)
            
            if stop_on_fail and not result:
                print(f"\n❌ Test failed: {test_name}")
                print("Stopping tests due to failure.")
                print("\n" + "=" * 50)
                self.print_summary()
                return False
            
            # Small delay between tests
            time.sleep(0.1)
        
        # Test content types if we got this far
        if not stop_on_fail or all(result['status'] != 'FAIL' for result in self.results):
            content_type_paths = paths.get('content_types', {
                'HTML': '/cgi-bin/html.php',
                'JSON': '/cgi-bin/json_output.py',
                'Plain Text': '/cgi-bin/text.py'
            })
            self.test_cgi_content_types(content_type_paths)
        
        # Summary
        print("\n" + "=" * 50)
        self.print_summary()
        return True
    
    def print_summary(self):
        """Print test summary"""
        total = len(self.results)
        passed = sum(1 for r in self.results if r['status'] == 'PASS')
        failed = sum(1 for r in self.results if r['status'] == 'FAIL')
        warnings = sum(1 for r in self.results if r['status'] == 'WARN')
        
        print(f"Test Summary:")
        print(f"  Total tests: {total}")
        print(f"  Passed: {passed}")
        print(f"  Failed: {failed}")
        print(f"  Warnings: {warnings}")
        
        if failed > 0:
            print(f"\nFailed tests:")
            for result in self.results:
                if result['status'] == 'FAIL':
                    print(f"  - {result['test']}: {result['message']}")
    
    def save_results(self, filename: str = 'cgi_test_results.json'):
        """Save test results to JSON file"""
        with open(filename, 'w') as f:
            json.dump({
                'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
                'server': self.base_url,
                'results': self.results
            }, f, indent=2)
        print(f"\nResults saved to: {filename}")


def main():
    """Main function for command-line usage"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Test CGI functionality on a web server')
    parser.add_argument('url', help='Base URL of the web server (e.g., http://localhost:8080)')
    parser.add_argument('--timeout', type=int, default=10, help='Request timeout in seconds')
    parser.add_argument('--save', help='Save results to JSON file')
    parser.add_argument('--cgi-path', help='Custom CGI path for basic test (default: /cgi-bin/test.py)')
    parser.add_argument('--continue-on-fail', action='store_true', help='Continue testing even if a test fails')
    
    args = parser.parse_args()
    
    # Create and run tester
    tester = CGITester(args.url, args.timeout)
    
    # Set custom paths if provided
    custom_paths = {}
    if args.cgi_path:
        custom_paths['basic'] = args.cgi_path
    
    # Run tests - stop on failure by default, continue if flag is set
    stop_on_fail = not args.continue_on_fail
    success = tester.run_all_tests(custom_paths if custom_paths else None, stop_on_fail)
    
    # Save results if requested
    if args.save:
        tester.save_results(args.save)
    
    # Exit with appropriate code
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()