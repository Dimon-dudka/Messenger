# Messenger
This project offers the implementation of a client-server messenger. It implements two applications: a server and a messenger client.

## Technology stack:
- C++ 17 
- QT 6.6
- SQLITE
- GIT
- CMake
- TCP/IP
- JSON

## App functions:
- Registration and authorization of users.
- Choice of the interlocutor.
- Send text messages to another user.
- Restoring the message history.
- Local storage of user chat IDs with ranking by chat popularity in order to recommend a particular chat to the user.
- User recommendations for the entered parts of the login.
- A single error window with different scenarios and error types.
- Logging of client and server errors.

## Server 
The server application is implemented based on a console program. The main logic of the server is implemented in the "main_server" class. It establishes a connection with the client and listens to all incoming messages. Messages are received and sent to clients in JSON format. Next, the "sql_engine" class is accessed, in which possible database queries are encapsulated. In addition, the logger class has been added to log errors and the places where they occurred.
### Server structure
The general structure of the application classes:
![Server structure](https://github.com/Dimon-dudka/Messenger/blob/main/Server/resources/server_structure.png)

## Client 
The client is implemented based on QT Widgets. The main logic of Backend and Frontend is implemented in the "widget_manager" class. 
Frontend is a graphical description of windows and some logic within them. Backend - provides the ability to manage windows, communicate with the server, logging errors and access the local database.

### Messenger structure
The general structure of the application classes:
![Server structure](https://github.com/Dimon-dudka/Messenger/blob/main/Messenger/resources/messenger_structure.png)

## App versions:
- V 1.0 - 12.02.2024 - Release version