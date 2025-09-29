#!/usr/bin/env php
<?php
header("Content-Type: text/html; charset=utf-8");
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HTML CGI Test</title>
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 40px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: white;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .success { 
            color: #28a745; 
            font-size: 2em;
            margin-bottom: 20px;
        }
        .info { 
            background: #e9ecef; 
            padding: 15px; 
            border-radius: 5px;
            margin: 20px 0;
        }
        .features {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-top: 20px;
        }
        .feature {
            background: #f8f9fa;
            padding: 15px;
            border-left: 4px solid #007bff;
            border-radius: 4px;
        }
        code {
            background: #f1f3f4;
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Courier New', monospace;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1 class="success">✓ HTML CGI Script Working!</h1>
        
        <div class="info">
            <p><strong>Content-Type:</strong> <code>text/html; charset=utf-8</code></p>
            <p><strong>Status:</strong> This is a properly formatted HTML5 response</p>
            <p><strong>Script Type:</strong> PHP CGI</p>
            <p><strong>Server Time:</strong> <?php echo date('Y-m-d H:i:s'); ?></p>
        </div>

        <h2>Test Features</h2>
        <div class="features">
            <div class="feature">
                <h3>HTML5 Standards</h3>
                <ul>
                    <li>DOCTYPE declaration</li>
                    <li>Semantic markup</li>
                    <li>Meta viewport</li>
                    <li>Language attribute</li>
                </ul>
            </div>
            
            <div class="feature">
                <h3>CSS Features</h3>
                <ul>
                    <li>Grid layout</li>
                    <li>Box shadows</li>
                    <li>Border radius</li>
                    <li>Responsive design</li>
                </ul>
            </div>
            
            <div class="feature">
                <h3>CGI Integration</h3>
                <ul>
                    <li>Proper headers</li>
                    <li>UTF-8 encoding</li>
                    <li>Content-Type set</li>
                    <li>Clean HTML output</li>
                </ul>
            </div>
        </div>

        <h2>Server Information</h2>
        <table style="width: 100%; border-collapse: collapse; margin-top: 15px;">
            <tr style="background: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Script Type</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">PHP CGI</td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>PHP Version</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><?php echo phpversion(); ?></td>
            </tr>
            <tr style="background: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Server Software</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><?php echo isset($_SERVER['SERVER_SOFTWARE']) ? $_SERVER['SERVER_SOFTWARE'] : 'Unknown'; ?></td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Request Method</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><?php echo isset($_SERVER['REQUEST_METHOD']) ? $_SERVER['REQUEST_METHOD'] : 'Unknown'; ?></td>
            </tr>
            <tr style="background: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Script Name</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><?php echo isset($_SERVER['SCRIPT_NAME']) ? $_SERVER['SCRIPT_NAME'] : 'Unknown'; ?></td>
            </tr>
            <tr>
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Content Type</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;">text/html; charset=utf-8</td>
            </tr>
            <tr style="background: #f8f9fa;">
                <td style="padding: 10px; border: 1px solid #dee2e6;"><strong>Status</strong></td>
                <td style="padding: 10px; border: 1px solid #dee2e6;" class="success">✓ Working</td>
            </tr>
        </table>

        <h2>Environment Variables</h2>
        <div style="background: #f8f9fa; padding: 15px; border-radius: 5px; margin-top: 15px;">
            <p><strong>HTTP_USER_AGENT:</strong> <?php echo isset($_SERVER['HTTP_USER_AGENT']) ? htmlspecialchars($_SERVER['HTTP_USER_AGENT']) : 'Not set'; ?></p>
            <p><strong>SERVER_NAME:</strong> <?php echo isset($_SERVER['SERVER_NAME']) ? htmlspecialchars($_SERVER['SERVER_NAME']) : 'Not set'; ?></p>
            <p><strong>SERVER_PORT:</strong> <?php echo isset($_SERVER['SERVER_PORT']) ? htmlspecialchars($_SERVER['SERVER_PORT']) : 'Not set'; ?></p>
            <p><strong>QUERY_STRING:</strong> <?php echo isset($_SERVER['QUERY_STRING']) && !empty($_SERVER['QUERY_STRING']) ? htmlspecialchars($_SERVER['QUERY_STRING']) : 'Empty'; ?></p>
        </div>

        <div style="margin-top: 30px; padding: 20px; background: #d4edda; border: 1px solid #c3e6cb; border-radius: 5px; color: #155724;">
            <h3 style="margin-top: 0;">✓ CGI HTML Test Successful</h3>
            <p>This page demonstrates that your web server can properly execute PHP CGI scripts and serve HTML content with the correct Content-Type headers.</p>
            <p><strong>Generated at:</strong> <?php echo date('Y-m-d H:i:s T'); ?></p>
        </div>
    </div>
</body>
</html>