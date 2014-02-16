#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <string.h>
#include <stdlib.h> 
#include "GPIOClass.h"
int main()
{


	GPIOClass* gpio4=new GPIOClass("4");
	GPIOClass* gpio17=new GPIOClass("17");
	gpio4->export_gpio();
	gpio17->export_gpio();
	gpio17->setdir_gpio("out");
	gpio4->setdir_gpio("out");

    int status;
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in c++, it will be given a block of memory. This memory is not nessesary
    // empty. Therefor we use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);

    std::cout << "Setting up the structs..."  << std::endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

    // Now fill up the linked list of host_info structs with google's address information.
    status = getaddrinfo(NULL, "7777", &host_info, &host_info_list);
    // getaddrinfo returns 0 on succes, or some other value when an error occured.
    // (translated into human readable text by the gai_gai_strerror function).
    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;


    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1)  std::cout << "socket error " ;

    std::cout << "Binding socket..."  << std::endl;
    // we use to make the setsockopt() function to make sure the port is not in use
    // by a previous execution of our code. (see man page for more information)
    int yes = 1;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  std::cout << "bind error" << std::endl ;

    std::cout << "Listen()ing for connections..."  << std::endl;
    status =  listen(socketfd, 5);
    if (status == -1)  std::cout << "listen error" << std::endl ;


    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        std::cout << "listen error" << std::endl ;
    }
    else
    {
        std::cout << "Connection accepted. Using new socketfd : "  <<  new_sd << std::endl;
    }


    std::cout << "Waiting to recieve data..."  << std::endl;
ReadData:
 ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0){ std::cout << "host shut down." << std::endl ; return(0);  }
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    //std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0';
    std::cout << incomming_data_buffer << std::endl;
    std::string str(incomming_data_buffer);
    int value=atoi(str.c_str());
    std::cout<<"value is"<<value<<std::endl;
    if(value==1)
	{
	 std::string inputstate;
	gpio4->getval_gpio(inputstate);
	if(inputstate=="0")
	 {
	  gpio4->setval_gpio("1");
	 std::cout<<"seting gpio 4to  1"<<std::endl;
	 }
	else
	 {
	 gpio4->setval_gpio("0");
	 std::cout<<"seting gpio 4to  0"<<std::endl;
	 }
	}
    else if(value==2)
	{
	std::string inputstate;
        gpio17->getval_gpio(inputstate);
        if(inputstate=="0")
         {
          gpio17->setval_gpio("1");
         std::cout<<"seting gpio 17to  1"<<std::endl;
         }
        else
         {
         gpio17->setval_gpio("0");
         std::cout<<"seting gpio 17 to  0"<<std::endl;
         }
	}

   // std::cout << "send()ing back a message..."  << std::endl;
   // char *msg = "thank you.";
   // int len;
   // ssize_t bytes_sent;
   // len = strlen(msg);
   // bytes_sent = send(new_sd, msg, len, 0);
goto ReadData;
    //std::cout << "Stopping server..." << std::endl;
    //freeaddrinfo(host_info_list);
    //close(new_sd);
    //close(socketfd);

return 0 ;


}


