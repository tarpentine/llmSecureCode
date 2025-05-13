from cryptography.fernet import Fernet
import base64
import hashlib
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.backends import default_backend
import os
from cryptography.hazmat.primitives import hashes

# Derive a Fernet key from the password
def generate_key(password: str, salt: bytes) -> bytes:
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100_000,
        backend=default_backend()
    )
    return base64.urlsafe_b64encode(kdf.derive(password.encode()))

# Encrypt a message
def encrypt_message(message: str, password: str) -> tuple:
    salt = os.urandom(16)  # Secure random salt
    key = generate_key(password, salt)
    fernet = Fernet(key)
    encrypted = fernet.encrypt(message.encode())
    return encrypted, salt  # Return salt to use in decryption

# Decrypt a message
def decrypt_message(encrypted_message: bytes, password: str, salt: bytes) -> str:
    key = generate_key(password, salt)
    fernet = Fernet(key)
    decrypted = fernet.decrypt(encrypted_message)
    return decrypted.decode()

# Example usage
if __name__ == "__main__":
    msg = input("Enter your message: ")
    pwd = input("Enter a password: ")

    encrypted, salt = encrypt_message(msg, pwd)
    print("\nEncrypted message:", encrypted)
    print("Salt (save this!):", base64.b64encode(salt).decode())

    # For demonstration: decrypt immediately
    pwd2 = input("\nRe-enter the password to decrypt: ")
    try:
        decrypted = decrypt_message(encrypted, pwd2, salt)
        print("Decrypted message:", decrypted)
    except Exception as e:
        print("Failed to decrypt:", e)

