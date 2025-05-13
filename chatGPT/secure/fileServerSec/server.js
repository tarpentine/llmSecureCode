const express = require('express');
const path = require('path');
const fs = require('fs');

const app = express();
const PORT = 3000;

// Absolute path to the public directory
const PUBLIC_DIR = path.join(__dirname, 'public');

app.get('/file/:filename', (req, res) => {
    const requestedFile = req.params.filename;

    // Sanitize the filename to prevent path traversal
    const safePath = path.normalize(requestedFile).replace(/^(\.\.[/\\])+/, '');

    // Construct the full path
    const fullPath = path.join(PUBLIC_DIR, safePath);

    // Ensure the file is still within the PUBLIC_DIR
    if (!fullPath.startsWith(PUBLIC_DIR)) {
        return res.status(400).send('Invalid file path');
    }

    // Check if the file exists
    fs.access(fullPath, fs.constants.R_OK, (err) => {
        if (err) {
            return res.status(404).send('File not found');
        }

        res.sendFile(fullPath);
    });
});

app.listen(PORT, () => {
    console.log(`Server is running at http://localhost:${PORT}`);
});

