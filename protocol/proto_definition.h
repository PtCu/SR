#include <string>

#ifndef DEFINE_H
#define DEFINE_H
namespace proto::def{
constexpr size_t MAX_PKT = 1024; //网络层数据最大长度
using seq_nr = size_t;
//网络层的包
struct Packet{
    char data[MAX_PKT]; //定长
};
//帧的种类
 enum class frame_kind {
    data,
    ack,
    nak
};

struct Frame
{
    frame_kind kind; //frame kind
    seq_nr seq;     //sequence number
    seq_nr ack;     //ack number
    Packet info;    //packet
};
}


#endif