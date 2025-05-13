import socket
import ssl

# Define allowed commands
ALLOWED_COMMANDS = {
    "HELLO": lambda: "Hello, client!",
    "STATUS": lambda: "Server is running.",
    "EXIT": lambda: "Goodbye."
}

def handle_command(command):
    command = command.strip().upper()
    if command in ALLOWED_COMMANDS:
        return ALLOWED_COMMANDS[command]()
    else:
        return "ERROR: Unknown command."

def start_secure_server(host='127.0.0.1', port=4443):
    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile='cert.pem', keyfile='key.pem')

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0) as sock:
        sock.bind((host, port))
        sock.listen(5)
        print(f"[+] Secure server listening on {host}:{port}")

        with context.wrap_socket(sock, server_side=True) as ssock:
            while True:
                conn, addr = ssock.accept()
                print(f"[+] Connection from {addr}")
                with conn:
                    while True:
                        data = conn.recv(1024)
                        if not data:
                            break
                        command = data.decode('utf-8')
                        print(f"[DEBUG] Received: {command}")
                        response = handle_command(command)
                        conn.sendall(response.encode('utf-8'))
                        if command.strip().upper() == "EXIT":
                            break

