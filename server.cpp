#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include "indexer.hpp"
#include "mysql/mysql.h"
#include "DbManager.hpp"

 
#define ERROR(msg) std::cerr << "Server Error: " << msg << ". " << strerror(errno) << std::endl;
#define LOG(msg) std::cout << "Server Log: " << msg << std::endl;

extern Indexer indexer;
 //DbManager* dbmanager = new DbManager() ;
int run_server()
{



	//Stream - telephone
	//Datagram - telegraph

	//TCP - Transmission Control Protocol
	//UDP - User Datagram Protocol

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //this function creates socket 
    //AF_INET::Means that socket is internet socket, 
    //SOCK_STREAM::controls is conection or is not
    //yazik na kotorom sistemi budut rabotat mejdu soboy, 0 oznachayet shto beryom protokol po umolchaniyu
	if (socket_fd == -1)
	{
		ERROR("Cannot create socket");
		return -1;
	}
	LOG("Socket created"); //ete hajox a verjacel

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
    //peredayom adres strukturi, zapolnyayem nulyami 
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("0.0.0.0"); //0i oznachayet lyuboy adres
	addr.sin_port = htons(1500);

	if (bind(socket_fd, (sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) 
	{
		ERROR("Cannot bind");
		return -1;
	}
	LOG("Bound successfully");

	if (listen(socket_fd, 0) == -1)
	{
		ERROR("Cannot listen.");
		return -1;
	}

	int client_sock_fd = -1;
	if ((client_sock_fd = accept(socket_fd, 0, 0)) == -1)
	{
		ERROR("Cannot accept connection.");
		return -1;
	}
	LOG("Connection accepted. ");
/*
	if (send(client_sock_fd, "Hello\n", 6, 0) == -1)
	{
		ERROR("Cannpt send");
		return 1;
	}
*/
    const int buf_size =100;
	char buf[buf_size];
	ssize_t bytes;
	if ((bytes = recv(client_sock_fd, buf, buf_size, 0)) == -1)
	{
		ERROR("Cannot receive");
		return 1;
	}
	buf[bytes - 2] = 0;

	LOG(buf)
     auto matches = indexer.GetRelevantURLs(buf); //by me 
	//auto matches = dbmanager->getLinkByWord(buf);
	std::cout << "==========================================="<<std::endl;
    std::copy(matches.begin(), matches.end(), std::ostream_iterator<std::string>(std::cout, "\r\n"));

    for(auto i = matches.begin(); i != matches.end(); ++i)
    {
        send(client_sock_fd, i->c_str(), i->size() -1, 0);
		 send(client_sock_fd, "\r\n", 2, 0);
    }


/*	
	DbManager *dbManager = new DbManager();
	vector<Link> links = dbManager->getLinks();
	for(link = links.begin(); link != links.end();)
    {
		std::string href = link.getLinks();
        send(client_sock_fd, href, href.size() -1, 0);
		send(client_sock_fd, "\r\n", 2, 0);
    }
	
	*/
	shutdown(client_sock_fd, SHUT_RDWR);
    close(client_sock_fd);
    close(socket_fd);
}