#include "../socket/udp.h"
#include "proto_definition.h"
#include "timer.h"
#ifndef SR_H
#define SR_H

namespace proto::SR
{
    using namespace def;
    class SR
    {
    public:
        SR(const std::string &source_addr, const std::string &target_addr, const unsigned int &source_port, const unsigned int &target_port) : udp_socket(source_addr, target_addr, source_port, target_port),
                                                                                                                                               ack_expected(0), next_frame_to_send(0), frame_expected(0), too_far(NR_BUFS) {
                                                                                                                                                 
                                                                                                                                               }

        ~SR() = default;
        void start();

        static constexpr size_t MAX_BUF = 1024;              //接收数据包的缓冲大小
        static constexpr size_t MAX_SEQ = 7;                 //最大序列号
        static constexpr size_t NR_BUFS = (MAX_SEQ + 1) / 2; //窗口大小
        static constexpr unsigned int NORM_TIME_OUT = 10000;      //超时时间
        static constexpr unsigned int ACK_TIME_OUT = 3000;      //超时时间
    private:
        //检查是否在窗口范围内
        bool between(const seq_nr &a, const seq_nr &b, const seq_nr &c) const
        {
            return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
        }
        void inc(seq_nr &x)
        {
            (++x) %= (MAX_SEQ + 1);
        }
        void sendFrame(const frame_kind &fk, const seq_nr &frame_nr, const seq_nr &frame_expected);
        void receiveFrame(Frame &);

        void resend(size_t); //重传
        void ackBack();      //回复ack

        seq_nr ack_expected;       //lower edge of sender's window
        seq_nr next_frame_to_send; //upper edge of sender's window+1
        seq_nr frame_expected;     //lower edge of receiver's window
        seq_nr too_far;            //upper edge of receiver's window+1

        bool no_nak; //标记是否发过nak

        Packet out_buf[NR_BUFS]; //buffers for the outbound stream
        Packet in_buf[NR_BUFS];  //buffers for the inbound stream
        bool arrived[NR_BUFS];   //inbound bit map
        Timer timers[NR_BUFS];   //timer for sender

        Timer ack_timer; //ack timer

        Udp udp_socket; //利用frame

        //size_t nbuffered; //缓存的packet数量
    };
} // namespace proto::SR

#endif