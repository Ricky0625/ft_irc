import socket

# Define the IP address and port where your IRC server listens
SERVER_IP = '127.0.0.1'
SERVER_PORT = 6667  # Change this to the actual port

# Create a socket to listen for IRC clients
client_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_listener.bind(('0.0.0.0', 6666))  # Port for clients to connect
client_listener.listen(1)

# Connect to your IRC server
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.connect((SERVER_IP, SERVER_PORT))

# Function to capture and log data between client and server
def capture_and_log(client, server, log_file):
    while True:
        # Capture data from the client and forward it to the server
        client_data = client.recv(4096)
        if not client_data:
            break
        server.send(client_data)
        log_file.write("Client -> Server: " + client_data.decode('utf-8'))
        
        # Capture data from the server and forward it to the client
        server_data = server.recv(4096)
        if not server_data:
            break
        client.send(server_data)
        log_file.write("Server -> Client: " + server_data.decode('utf-8'))

try:
    # Accept incoming client connections
    while True:
        client, _ = client_listener.accept()
        log_file = open("irc_proxy.log", "a")  # Open or create a log file
        log_file.write("New client connected\n")
        
        # Create a thread to capture and log data for each client
        # In a production environment, consider using a thread or process pool for better scalability.
        capture_and_log(client, server, log_file)

except KeyboardInterrupt:
    client_listener.close()
    server.close()