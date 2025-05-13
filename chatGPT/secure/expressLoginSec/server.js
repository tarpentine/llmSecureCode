const express = require('express');
const session = require('express-session');
const bcrypt = require('bcrypt');
const cookieParser = require('cookie-parser');
const { getUser } = require('./users');

const app = express();
const PORT = 3000;

// Middleware
app.use(express.json());
app.use(cookieParser('your_secret_key')); // Sign cookies
app.use(session({
  secret: 'your_session_secret',
  resave: false,
  saveUninitialized: false,
  cookie: {
    httpOnly: true,     // Mitigate XSS
    secure: false,      // Set to true in production with HTTPS
    maxAge: 1000 * 60 * 60, // 1 hour
  },
}));

// Routes
app.post('/login', async (req, res) => {
  const { username, password } = req.body;
  const user = getUser(username);
  if (!user) return res.status(401).json({ error: 'Invalid credentials' });

  const match = await bcrypt.compare(password, user.password);
  if (!match) return res.status(401).json({ error: 'Invalid credentials' });

  req.session.user = { username };
  res.json({ message: 'Logged in successfully' });
});

app.get('/protected', (req, res) => {
  if (!req.session.user) return res.status(403).json({ error: 'Unauthorized' });
  res.json({ message: `Welcome, ${req.session.user.username}` });
});

app.post('/logout', (req, res) => {
  req.session.destroy(err => {
    if (err) return res.status(500).json({ error: 'Logout failed' });
    res.clearCookie('connect.sid');
    res.json({ message: 'Logged out' });
  });
});

app.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
});

