const express = require('express');
const session = require('express-session');
const cookieParser = require('cookie-parser');
const bcrypt = require('bcrypt');

const app = express();
const PORT = 3000;

// In-memory "database"
const users = {};

app.use(express.urlencoded({ extended: true }));
app.use(express.json());
app.use(cookieParser());

// Session configuration
app.use(session({
  secret: 'super-secret-key',
  resave: false,
  saveUninitialized: false,
  cookie: { maxAge: 60000 } // 1 minute for demo
}));

// Home route
app.get('/', (req, res) => {
  if (req.session.username) {
    res.send(`Welcome back, ${req.session.username}! <a href="/logout">Logout</a>`);
  } else {
    res.send('Hello, guest! <a href="/login">Login</a> or <a href="/register">Register</a>');
  }
});

// Registration route
app.get('/register', (req, res) => {
  res.send(`
    <form method="POST" action="/register">
      <input name="username" placeholder="Username" required />
      <input name="password" type="password" placeholder="Password" required />
      <button type="submit">Register</button>
    </form>
  `);
});

app.post('/register', async (req, res) => {
  const { username, password } = req.body;
  if (users[username]) {
    return res.send('User already exists. <a href="/register">Try again</a>');
  }
  const hashedPassword = await bcrypt.hash(password, 10);
  users[username] = { username, password: hashedPassword };
  res.send('Registration successful! <a href="/login">Login</a>');
});

// Login route
app.get('/login', (req, res) => {
  res.send(`
    <form method="POST" action="/login">
      <input name="username" placeholder="Username" required />
      <input name="password" type="password" placeholder="Password" required />
      <button type="submit">Login</button>
    </form>
  `);
});

app.post('/login', async (req, res) => {
  const { username, password } = req.body;
  const user = users[username];
  if (!user || !(await bcrypt.compare(password, user.password))) {
    return res.send('Invalid credentials. <a href="/login">Try again</a>');
  }
  req.session.username = username;
  res.redirect('/');
});

// Logout route
app.get('/logout', (req, res) => {
  req.session.destroy(err => {
    if (err) return res.send('Error logging out.');
    res.clearCookie('connect.sid');
    res.redirect('/');
  });
});

// Start server
app.listen(PORT, () => {
  console.log(`Server running at http://localhost:${PORT}`);
});

