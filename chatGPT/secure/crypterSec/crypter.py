import base64
import os
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives import hashes
from cryptography.fernet import Fernet
from getpass import getpass

def derive_key(password: str, salt: bytes) -> bytes:
    """Derives a secure key from the password and salt."""
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100_000,
    )
    return base64.urlsafe_b64encode(kdf.derive(password.encode()))

def encrypt_message(message: str, password: str) -> bytes:
    salt = os.urandom(16)
    key = derive_key(password, salt)
    f = Fernet(key)
    encrypted = f.encrypt(message.encode())
    return salt + encrypted  # Prepend salt for decryption

def decrypt_message(encrypted_data: bytes, password: str) -> str:
    salt = encrypted_data[:16]
    encrypted = encrypted_data[16:]
    key = derive_key(password, salt)
    f = Fernet(key)
    return f.decrypt(encrypted).decode()

def main():
    choice = input("Encrypt or Decrypt? (e/d): ").lower().strip()
    if choice not in ('e', 'd'):
        print("Invalid choice.")
        return

    password = getpass("Enter password: ")

    if choice == 'e':
        message = input("Enter message to encrypt: ")
        encrypted = encrypt_message(message, password)
        print(f"\nEncrypted message (base64):\n{base64.b64encode(encrypted).decode()}")
    else:
        b64_input = input("Enter encrypted message (base64): ")
        try:
            encrypted_data = base64.b64decode(b64_input)
            decrypted = decrypt_message(encrypted_data, password)
            print(f"\nDecrypted message:\n{decrypted}")
        except Exception as e:
            print(f"Decryption failed: {e}")

if __name__ == "__main__":
    main()

