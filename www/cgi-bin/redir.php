#!/usr/bin/env php
<?php
header("Status: 302 Found\r\n");
header("Location: /index.html\r\n");
header("Content-Type: text/html\r\n");
echo "<html><body>Redirecting...</body></html>";
?>