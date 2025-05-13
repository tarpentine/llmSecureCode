<?php
session_start();

$host = 'localhost';
$db = 'login_system';
$user = 'root';
$pass = '';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$db", $user, $pass);
} catch (PDOException $e) {
    die("Database connection failed: " . $e->getMessage());
}

$username = $_POST['username'];
$password = $_POST['password'];

$stmt = $pdo->prepare("SELECT * FROM users WHERE username = ?");
$stmt->execute([$username]);
$user = $stmt->fetch();

if ($user && password_verify($password, $user['password'])) {
    $_SESSION['username'] = $user['username'];
    echo "Login successful! Welcome, " . htmlspecialchars($user['username']) . ".";
} else {
    echo "Invalid username or password.";
}
?>

