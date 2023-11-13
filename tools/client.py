import socket

# IRC server and channel information
server = "localhost"  # Replace with your IRC server
port = 6667  # Standard IRC port
channel = "#mychannel"  # Replace with the channel you want to join

# User information
nickname = "MyBot"  # Your bot's nickname
realname = "My Python IRC Bot"  # Your bot's real name

# Connect to the IRC server
irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
irc.connect((server, port))

# Send user and nickname information
irc.send(bytes("NICK " + nickname + "\r\n", "UTF-8"))
irc.send(bytes("USER " + nickname + " 0 * :" + realname + "\r\n", "UTF-8"))

# Join a channel
irc.send(bytes("JOIN " + channel + "\r\n", "UTF-8"))

# Main loop to send and receive messages
while True:
    data = irc.recv(4096).decode("UTF-8")
    print(data)

    if data.find("PING") != -1:
        irc.send(bytes("PONG " + data.split()[1] + "\r\n", "UTF-8"))

    if data.find("PRIVMSG " + channel) != -1:
        message = "Hello, IRC World!"
        irc.send(bytes("PRIVMSG " + channel + " :" + message + "\r\n", "UTF-8"))
