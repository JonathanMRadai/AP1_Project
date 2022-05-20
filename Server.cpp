#include "Server.h"
#include "algorithm"

Server::Server(int port) throw (const char*) : t(nullptr){

    //open a socket for the server
    serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD < 0)
        throw "socket problem";

    server = gethostbyname("localhost");
    if(server==NULL)
        throw "no such host";

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

    serv_addr.sin_port = htons(port);

    //bind the socket
    if (bind(serverFD,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        throw "Failed to bind to port";
}

void Server::start(ClientHandler& ch) throw(const char*) {

    t = new thread([this, &ch]() throw(const char*)->void {

        int users = 0;
        vector<thread*> ts;

        //start listing
        if (listen(serverFD, USERS) < 0)
            throw "Failed to listen on socket";

        auto addrlen = sizeof(serv_addr);
        while(users < USERS) {
            int connection = accept(serverFD, (struct sockaddr*)&serv_addr, (socklen_t*)&addrlen);
            if (connection < 0)
                throw "Failed to grab connection";
            ts.push_back(new thread([connection, &ch](){ch.handle(connection);}));
            ++users;
        }
                       for_each(ts.begin(), ts.end(),[](thread* thread)
                                {
                                  if(thread->joinable()) {
                                      thread->join();
                                  }
                                  delete(thread);
                                  thread = nullptr;

                                }
                       );
                       ts.clear();
    }
    );
}

void Server::stop(){
	t->join(); // do not delete this!
    if (t != nullptr) {
        delete(t);
        t = nullptr;
    }
}

Server::~Server() {
close(serverFD);
}
