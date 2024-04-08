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
The server application is implemented based on a console program. Starting from version 1.1, the server is implemented in multithreaded mode and has the ability to scale. The server responds to user requests. Namely: login, registration, sending and receiving messages, receiving a list of recommended users, restoring the message history. The server also logs events.  

- **main_server** - works in the main thread. It waits for a request from users and passes them to new threads for processing by thread_sheduler.  
- **thread_sheduler** is a class that works in separate threads and is used to process a user request and send a response to a user socket. 
- **sql_engine** is an API for interacting with SQL through classes in separeted threads: messages_info_thread ,user_info_thread.
- **messages_info_thread** and **user_info_thread** - are classes that work in separate threads and have access to the corresponding databases.
- **logger** - class that works in a separate thread and provides an interface for sending an event logging request.
  
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
- V 1.1 - 08.04.2024 - Multithreaded server update