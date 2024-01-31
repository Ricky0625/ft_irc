# ft_irc

Internet Relay Chat or IRC is a text-based communication protocol on the Internet. It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.

# Table of Contents

- [ft\_irc](#ft_irc)
- [Table of Contents](#table-of-contents)
  - [Project overview](#project-overview)
  - [About IRC](#about-irc)
  - [Prerequisites](#prerequisites)
  - [Features](#features)
    - [Connection related](#connection-related)
    - [Channel related](#channel-related)
    - [Extra](#extra)
  - [Authors](#authors)

## Project overview

In this project, our focus is on creating an IRC server in C++98. The server should be able to efficiently handle multiple users, ensuring smooth communication. We need to make sure that it can work with any of the existing IRC clients. The key features of our IRC server include user authentication, joining chat channels, and private messaging. Also, we need to differentiate between operators (mainly channel operators) and regular users.

## About IRC

IRC, short for Internet Relay Chat, is a communication protocol that enables real-time text messaging over the internet. It was developed in the late 1980s and continues to be used today in some communities or even organizations. IRC operates on a client-server model, where users connect to an IRC server using client software, allowing them to join channels and engage in conversations. If you are familiar with Discord, you can think of IRC as the old-school, text-only version of it. Both platforms facilitate real-time communication, but IRC takes a more straightforward and minimalistic approach.

## Prerequisites

Before you start this project, it's important to understand the following concepts:

1. Socket programming
2. I/O Multiplexing

There's a bunch of good resources online, so DYOR! Here are some of the resources that I found useful:

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/multi/index.html)

Also, to fully understand how IRC actually works, you are encouraged to read the following RFCs:

- [RFC 2812](https://tools.ietf.org/html/rfc2812)
- [RFC 2813](https://tools.ietf.org/html/rfc2813)

There's also one particular documentation you can read to understand how IRC works in practice:

- [IRC Protocol Documentation](https://modern.ircdocs.horse/)

## Features

For our project, we implemented these commands:

### Connection related

- `PASS`
- `NICK`
- `USER`
- `PING`
- `PONG`
- `OPER`
- `QUIT`

### Channel related

- `JOIN`
- `PART`
- `NAMES`
- `LIST`
- `TOPIC`
- `INVITE`
- `KICK`
- `PRIVMSG`
- `NOTICE`
- `MODE`

### Extra

- `MOTD`

## Authors

- [**Ricky**](https://github.com/Ricky0625)
- [**Matthew**](https://github.com/MTLKS)

Here is the presentation deck for this project: [ft_irc Peer evaluation](https://www.canva.com/design/DAF6_BI4sHk/lvaSYZlne3vnJ-KDt6kqKg/edit?utm_content=DAF6_BI4sHk&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)

We also created a flow chart of our IRC server: [IRC Server Flow Chart](https://www.figma.com/file/vOD24dHARKZtkM5YXSGrFZ/ft_irc-Program-flow?type=whiteboard&node-id=0%3A1&t=Z42ssn8VQYiNplMz-1)

