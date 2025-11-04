#!/usr/bin/env python3
import os
import cgi
import cgitb
from pathlib import Path

cgitb.enable()

# Get the directory where this script is located
script_dir = Path(__file__).parent

# Get all CGI script files
cgi_extensions = ['.py', '.php', '.cgi', '.pl', '.sh']
cgi_files = []

for file in script_dir.iterdir():
    if file.is_file() and file.suffix in cgi_extensions:
        # Get file info
        stat = file.stat()
        cgi_files.append({
            'name': file.name,
            'path': file.name,
            'extension': file.suffix,
        })

# Sort by name
cgi_files.sort(key=lambda x: x['name'])

# Get the script URL base
script_name = os.environ.get('SCRIPT_NAME', '/cgi-bin')
base_url = script_name.rsplit('/', 1)[0]

# Generate HTML
print("Content-Type: text/html\n")
print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Scripts Directory</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #0d1117 0%, #161b22 100%);
            min-height: 100vh;
            color: #e6edf3;
            padding: 2rem 1rem;
        }
        .container {
            max-width: 1400px;
            margin: 0 auto;
        }
        .header {
            text-align: center;
            margin-bottom: 3rem;
        }
        h1 {
            font-size: clamp(2rem, 4vw, 3rem);
            font-weight: 600;
            background: linear-gradient(135deg, #58a6ff, #79c0ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
            margin-bottom: 0.5rem;
        }
        .subtitle {
            font-size: 1.1rem;
            color: rgba(230, 237, 243, 0.7);
            margin-top: 0.5rem;
        }
        .stats {
            display: flex;
            justify-content: center;
            gap: 2rem;
            margin-top: 1.5rem;
            flex-wrap: wrap;
        }
        .stat-item {
            padding: 0.75rem 1.5rem;
            background: rgba(13, 17, 23, 0.8);
            border: 2px solid rgba(88, 166, 255, 0.3);
            border-radius: 12px;
            backdrop-filter: blur(10px);
        }
        .stat-number {
            font-size: 1.8rem;
            font-weight: 700;
            color: #58a6ff;
        }
        .stat-label {
            font-size: 0.9rem;
            color: rgba(230, 237, 243, 0.6);
            margin-top: 0.25rem;
        }
        .cards-grid {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(320px, 1fr));
            gap: 1.5rem;
            margin-top: 2rem;
        }
        .card {
            background: rgba(13, 17, 23, 0.8);
            border: 2px solid rgba(88, 166, 255, 0.3);
            border-radius: 16px;
            padding: 1.5rem;
            backdrop-filter: blur(10px);
            transition: all 0.3s ease;
            cursor: pointer;
            position: relative;
            overflow: hidden;
        }
        .card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 4px;
            background: linear-gradient(90deg, #58a6ff, #79c0ff);
            transform: scaleX(0);
            transform-origin: left;
            transition: transform 0.3s ease;
        }
        .card:hover {
            transform: translateY(-5px);
            border-color: #58a6ff;
            box-shadow: 0 12px 30px -10px rgba(88, 166, 255, 0.3);
        }
        .card:hover::before {
            transform: scaleX(1);
        }
        .card-header {
            display: flex;
            align-items: center;
            gap: 1rem;
            margin-bottom: 1rem;
        }
        .file-icon {
            width: 48px;
            height: 48px;
            border-radius: 12px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: 700;
            font-size: 0.9rem;
            flex-shrink: 0;
        }
        .icon-py {
            background: linear-gradient(135deg, rgba(88, 166, 255, 0.2), rgba(88, 166, 255, 0.1));
            border: 2px solid #58a6ff;
            color: #79c0ff;
        }
        .icon-php {
            background: linear-gradient(135deg, rgba(163, 113, 247, 0.2), rgba(163, 113, 247, 0.1));
            border: 2px solid #a371f7;
            color: #bc8cff;
        }
        .icon-cgi {
            background: linear-gradient(135deg, rgba(63, 185, 80, 0.2), rgba(63, 185, 80, 0.1));
            border: 2px solid #3fb950;
            color: #56d364;
        }
        .icon-pl {
            background: linear-gradient(135deg, rgba(255, 191, 0, 0.2), rgba(255, 191, 0, 0.1));
            border: 2px solid #f0c800;
            color: #f0c800;
        }
        .icon-sh {
            background: linear-gradient(135deg, rgba(248, 81, 73, 0.2), rgba(248, 81, 73, 0.1));
            border: 2px solid #f85149;
            color: #ff7b72;
        }
        .card-title {
            flex: 1;
        }
        .file-name {
            font-size: 1.1rem;
            font-weight: 600;
            color: #e6edf3;
            word-break: break-all;
        }
        .file-ext {
            font-size: 0.85rem;
            color: rgba(230, 237, 243, 0.6);
            margin-top: 0.25rem;
        }
        .card-body {
            margin-top: 1rem;
        }
        .file-info {
            display: flex;
            flex-wrap: wrap;
            gap: 0.75rem;
        }
        .info-badge {
            padding: 0.4rem 0.8rem;
            background: rgba(22, 27, 34, 0.8);
            border: 1px solid rgba(88, 166, 255, 0.2);
            border-radius: 6px;
            font-size: 0.85rem;
            color: rgba(230, 237, 243, 0.8);
        }
        .badge-executable {
            background: rgba(63, 185, 80, 0.1);
            border-color: #3fb950;
            color: #56d364;
        }
        .badge-not-executable {
            background: rgba(248, 81, 73, 0.1);
            border-color: #f85149;
            color: #ff7b72;
        }
        .run-btn {
            width: 100%;
            padding: 0.75rem;
            margin-top: 1rem;
            background: linear-gradient(135deg, rgba(88, 166, 255, 0.2), rgba(88, 166, 255, 0.1));
            border: 2px solid #58a6ff;
            border-radius: 10px;
            color: #e6edf3;
            font-size: 0.95rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            text-decoration: none;
            display: block;
            text-align: center;
        }
        .run-btn:hover {
            background: linear-gradient(135deg, rgba(88, 166, 255, 0.3), rgba(88, 166, 255, 0.2));
            transform: translateY(-2px);
            box-shadow: 0 6px 20px -6px rgba(88, 166, 255, 0.4);
        }
        .empty-state {
            text-align: center;
            padding: 4rem 2rem;
            color: rgba(230, 237, 243, 0.5);
        }
        .empty-icon {
            font-size: 4rem;
            margin-bottom: 1rem;
            opacity: 0.3;
        }
        @media (max-width: 768px) {
            .cards-grid {
                grid-template-columns: 1fr;
            }
            .stats {
                gap: 1rem;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>CGI Scripts Directory</h1>
            <p class="subtitle">Available executable scripts in this directory</p>
            <div class="stats">
                <div class="stat-item">
                    <div class="stat-number">""" + str(len(cgi_files)) + """</div>
                    <div class="stat-label">Total Scripts</div>
                </div>
            </div>
        </div>
""")

if cgi_files:
    print('        <div class="cards-grid">')
    for file in cgi_files:
        # Determine icon class
        ext = file['extension'].lower()
        if ext == '.py':
            icon_class = 'icon-py'
            icon_text = 'PY'
        elif ext == '.php':
            icon_class = 'icon-php'
            icon_text = 'PHP'
        elif ext == '.cgi':
            icon_class = 'icon-cgi'
            icon_text = 'CGI'
        elif ext == '.sh':
            icon_class = 'icon-sh'
            icon_text = 'SH'
        else:
            icon_class = 'icon-cgi'
            icon_text = ext[1:].upper()
        
        # Generate card
        print(f"""
            <div class="card">
                <div class="card-header">
                    <div class="file-icon {icon_class}">{icon_text}</div>
                    <div class="card-title">
                        <div class="file-name">{file['name']}</div>
                    </div>
                </div>
                <div class="card-body">
                    <a href="{base_url}/{file['path']}" class="run-btn">▶ Run Script</a>
                </div>
            </div>
        """)
    print('        </div>')
else:
    print("""
        <div class="empty-state">
            <div class="empty-icon">📁</div>
            <h2>No CGI Scripts Found</h2>
            <p>No executable scripts were found in this directory</p>
        </div>
    """)

print("""
    </div>
</body>
</html>""")
