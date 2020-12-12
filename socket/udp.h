#ifndef UDP_H
#define UDP_H


#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>


class Udp {
public:
    Udp(const std::string& _source_addr,  const std::string& _target_addr,const unsigned int& _source_port,
            const unsigned int& _target_port);
    ~Udp();

    /**
     * @brief    Send N bytes of BUF 
     *              Returns the number of bytes send or -1 for errors.
     * @param  buf            
     * @param  N               
     * @return ssize_t 
     */
    ssize_t send(const char* buf,const size_t& N);

    /**
     * @brief Read N bytes into BUF through socket FD.
                Returns the number of bytes read or -1 for errors.
     * @param  buf            
     * @param  N              
     * @return ssize_t 
     */
    ssize_t recv(char* buf,const size_t& N);

    /**
     * @brief close the socket
     */
    void close();

private:

    int fd;
    in_port_t source_port;
    in_addr_t source_addr;
    in_port_t target_port;
    in_addr_t target_addr;

};



#endif

