require('dotenv').config();
const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');
const validator = require('validator');

const app = express();
const port = process.env.PORT || 3000;

// Middleware
app.use(bodyParser.json());

// Connect to MongoDB
mongoose.connect(process.env.MONGO_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
})
.then(() => console.log("Connected to MongoDB"))
.catch(err => {
    console.error("MongoDB connection error:", err);
    process.exit(1);
});

// Email Schema
const userSchema = new mongoose.Schema({
    email: {
        type: String,
        required: true,
        unique: true,
        validate: [validator.isEmail, 'Invalid email'],
    },
});

const User = mongoose.model('User', userSchema);

// POST endpoint to save email
app.post('/subscribe', async (req, res) => {
    try {
        const { email } = req.body;

        // Basic email validation
        if (!email || !validator.isEmail(email)) {
            return res.status(400).json({ error: 'Invalid email format.' });
        }

        // Normalize and sanitize
        const sanitizedEmail = validator.normalizeEmail(email);

        const newUser = new User({ email: sanitizedEmail });
        await newUser.save();

        res.status(201).json({ message: 'Email saved successfully.' });
    } catch (err) {
        if (err.code === 11000) {
            res.status(409).json({ error: 'Email already exists.' });
        } else {
            console.error("Error saving email:", err);
            res.status(500).json({ error: 'Internal server error.' });
        }
    }
});

app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});

