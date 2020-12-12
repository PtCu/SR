#include <iostream>
#include "udp.h"

Udp::Udp(const std::string &_source_addr, const std::string &_target_addr, const unsigned int &_source_port,
         const unsigned int &_target_port)
{

    //目的端口和源端口
    this->source_port = htons(_source_port);
    this->target_port = htons(_target_port);

    //源地址和目的地址
    in_addr tmp;

    inet_aton(_source_addr.c_str(), &tmp);
    this->source_addr = tmp.s_addr;

    inet_aton(_target_addr.c_str(), &tmp);
    this->target_addr = tmp.s_addr;

    try
    {
        //创建套接字
        if ((this->fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            throw "socket create error!";
        }

        //bind
        sockaddr_in s_addr;
        s_addr.sin_family = AF_INET;
        s_addr.sin_addr.s_addr = this->source_addr;
        s_addr.sin_port = this->source_port;
        //绑定端口和ip。不区分服务器和客户端时必须绑定端口
        if (::bind(this->fd, reinterpret_cast<sockaddr *>(&s_addr), sizeof(s_addr)) < 0)
        {
            throw "socket bind error!";
        }
    }
    catch (const char *e)
    {
        std::cout << *e << std::endl;
    }
}

Udp::~Udp()
{
    this->close();
}

ssize_t Udp::recv(char *buf, const size_t &N)
{
    if (0 == N)
        return 0;

    sockaddr_in tmp;
    tmp.sin_family = AF_INET;
    tmp.sin_addr.s_addr = this->target_addr;
    tmp.sin_port = this->target_port;
    socklen_t tmp_len = sizeof(tmp);
    return ::recvfrom(fd, buf, N, 0, reinterpret_cast<sockaddr *>(&tmp), &tmp_len);
}

ssize_t Udp::send(const char *buf, const size_t &N)
{
    sockaddr_in tmp;
    tmp.sin_family = AF_INET;
    tmp.sin_addr.s_addr = target_addr;
    tmp.sin_port = target_port;
    return ::sendto(fd, buf, N, 0, reinterpret_cast<sockaddr *>(&tmp), sizeof(tmp));
}

void Udp::close()
{
    ::close(this->fd);
}
