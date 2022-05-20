#ifndef SERVER_H_
#define SERVER_H_
#include "commands.h"
#include "CLI.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <thread>
#define USERS 2

using namespace std;


class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID) {
        ServerIO sio(clientID);
        CLI(&sio).start();
        close(clientID);
    }
};


// implement on Server.cpp
class Server {

	thread* t; // the thread to run the start() method in
    int serverFD;
    struct sockaddr_in serv_addr;
    struct hostent *server;
public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */
