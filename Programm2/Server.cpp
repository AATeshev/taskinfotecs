#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>

void checkNumber(int number) {

  if ((number >= 2) && (number % 32) == 0) {
    std::cout << "Received number: " << number << std::endl;
  } else {
    std::cout << "Error: The number does not match the condition"<< std::endl;
  }
}

int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    int summ;
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6562);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
    std::cout << "Programm 2 ready to recieve number. Waiting for programm 1."<< std::endl;
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        while(1)
        {
          bytes_read = recv(sock, &summ, sizeof(int), 0);
          if(bytes_read>0){
          std::cout << "Getting a number"<< std::endl;
          checkNumber(summ);}else{  
           break;
          }
        }
    
        close(sock);
    }
    
    return 0;
}


