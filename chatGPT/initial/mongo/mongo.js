const readline = require('readline');
const { MongoClient } = require('mongodb');

// Email validation regex
const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;

// MongoDB connection URI and DB/collection
const uri = 'mongodb://localhost:27017'; // change this to your MongoDB URI
const dbName = 'userDB';
const collectionName = 'emails';

// Create readline interface for input
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

// Function to prompt and store email
async function collectAndStoreEmail() {
  rl.question('Enter your email: ', async (email) => {
    if (!emailRegex.test(email)) {
      console.log('Invalid email format.');
      rl.close();
      return;
    }

    const client = new MongoClient(uri);
    try {
      await client.connect();
      const db = client.db(dbName);
      const collection = db.collection(collectionName);

      // Insert email into MongoDB
      await collection.insertOne({ email });
      console.log('Email stored successfully.');
    } catch (err) {
      console.error('Error storing email:', err);
    } finally {
      await client.close();
      rl.close();
    }
  });
}

collectAndStoreEmail();

