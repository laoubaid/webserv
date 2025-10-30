<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Check if file was uploaded without errors
    if (isset($_FILES['uploaded_file']) && $_FILES['uploaded_file']['error'] === UPLOAD_ERR_OK) {
        $fileTmpPath = $_FILES['uploaded_file']['tmp_name'];
        $fileName = $_FILES['uploaded_file']['name'];
        $uploadDir = 'uploads/';

        // Make sure upload directory exists
        if (!is_dir($uploadDir)) {
            mkdir($uploadDir, 0755, true);
        }

        $destPath = $uploadDir . basename($fileName);

        // Move file to uploads folder
        if (move_uploaded_file($fileTmpPath, $destPath)) {
            echo "File uploaded successfully! Saved as: " . htmlspecialchars($fileName);
        } else {
            echo "Error moving uploaded file.";
        }
    } else {
        echo "No file uploaded or there was an upload error.";
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Simple File Upload</title>
</head>
<body>
    <h2>Upload a File</h2>
    <form method="post" enctype="multipart/form-data">
        <input type="file" name="uploaded_file" required>
        <button type="submit">Upload</button>
    </form>
</body>
</html>
