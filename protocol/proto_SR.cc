#include "proto_SR.h"


namespace proto::SR
{
    void SR::sendFrame(const frame_kind &fk, const seq_nr &frame_nr, const seq_nr &frame_expected)
    {
        //ack=frame_expected-1。即为最后收到的序号的ack
        Frame f{.kind = fk, .seq = frame_nr, .ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1)};
        if (fk == frame_kind::data)
        {
            f.info = out_buf[frame_nr % NR_BUFS];
        }

        if (fk == frame_kind::nak)
        { //one nak per frame
            //接收方请求重传一个帧。为了避免多次重传同一个丢失帧，接收方应当记下对于某一帧是否已经发送过nak
            no_nak = false;
        }

        udp_socket.send(reinterpret_cast<char *>(&f), sizeof(Frame));
        if (fk == frame_kind::data)
        {
            timers[frame_nr % NR_BUFS].start(NORM_TIME_OUT, [this, frame_nr] { resend(frame_nr); });
        }
        ack_timer.stop();
       // std::cout << "ack timer关闭" << std::endl;
    }

    void SR::receiveFrame(Frame &recFrame)
    {
        char tmp[MAX_BUF];
        if (udp_socket.recv(tmp, MAX_BUF) < 0)
        {
            throw "receive error";
        };
        memset(&recFrame, 0, sizeof(Frame));
        memcpy(&recFrame, tmp, sizeof(Frame));

    }

    void SR::resend(size_t frame_nr)
    {
        std::cout<<"作为发送方：回传"<<frame_nr<<"号帧"<<std::endl;
        this->sendFrame(frame_kind::data, frame_nr, frame_expected);
    }

    void SR::ackBack()
    {
        std::cout<<"ack定时器超时，作为接收方：发送ack确认帧"<<std::endl;
        this->sendFrame(frame_kind::ack, 0, frame_expected);
    }

    void SR::start()
    {
        std::thread([this] {
            for (;;)
            {
                Frame r{};
                receiveFrame(r);
                if (r.kind == frame_kind::data)
                {
                     std::cout << "作为接收方：收到"<<r.seq<<"号帧" << std::endl;
                    //如果不是期望接收的
                    if ((r.seq != this->frame_expected))
                    {
                        //回传nak
                        std::cout << "不是接收窗口左边的帧" << std::endl;
                        std::cout << "要求发送方回传中..." << std::endl;
                        this->sendFrame(frame_kind::nak, 0, frame_expected);
                         std::cout << "nak发送完成" << std::endl;
                    }
                    //否则启用ack计时器，防止单向流量导致饥饿
                    else
                    {
                        std::cout << "是窗口左边的帧，启动ack timer" << std::endl;
                        ack_timer.start(ACK_TIME_OUT, [this] { ackBack(); });
                    }
                    if (between(frame_expected, r.seq, too_far) && (arrived[r.seq % NR_BUFS] == false))
                    {
                        //可以接收任意顺序的序列
                        arrived[r.seq % NR_BUFS] = true;
                        in_buf[r.seq % NR_BUFS] = r.info;
                        std::cout << "作为接收方：暂存了" << r.seq << "号帧" << std::endl;
                        std::cout << "帧内容为：" << std::string(r.info.data) << std::endl;
                        //接收数据并移动窗口
                        while (arrived[frame_expected % NR_BUFS])
                        {
                            //to network layer
                            std::cout << "作为接收方：" << frame_expected << "号帧已送达网络层" << std::endl;
                            no_nak = true;
                            arrived[frame_expected % NR_BUFS] = false;
                            inc(frame_expected);
                            inc(too_far);
                            std::cout << "作为接收方：此时窗口左侧为" << frame_expected << std::endl;
                            ack_timer.start(ACK_TIME_OUT, [this] { ackBack(); });
                        }
                    }
                }

                //如果收到nak，说明要求回传
               if ((r.kind == frame_kind::nak) && (between(ack_expected, (r.ack + 1) % (MAX_SEQ + 1), next_frame_to_send)))
                {
                    std::cout << "作为发送方：收到了nak，马上回传..." << std::endl;
                    resend((r.ack + 1) % (MAX_SEQ + 1));
                    //std::cout << "作为发送方：回传了" << (r.ack + 1) % (MAX_SEQ + 1) << "号帧" << std::endl;
                }
                //接收ack确认帧并移动窗口
                while (between(ack_expected, r.ack, next_frame_to_send))
                {
                    std::cout << "作为发送方：收到" << ack_expected << "号帧的确认帧" << std::endl;
                    timers[ack_expected % NR_BUFS].stop();
                    inc(ack_expected);
                }
            }
        }).detach();

        for (;;)
        {
            std::cout << "输入要发送的消息" << std::endl;
            Frame s;
            std::cin.getline(s.info.data, MAX_BUF);
            out_buf[next_frame_to_send % NR_BUFS] = s.info;
#ifdef SIMULATION
            int x=rand()%2;
            if(x){
            size_t i = (next_frame_to_send + rand()) % MAX_SEQ;
            std::cout << "作为发送方：发送了" << next_frame_to_send+1 << "号帧" << std::endl;
            sendFrame(frame_kind::data,i , frame_expected);
            
            }
            else{
               
                std::cout << "作为发送方发送了" << next_frame_to_send << "号帧" << std::endl;
                 sendFrame(frame_kind::data, next_frame_to_send, frame_expected);
            }
           
#else
           
            std::cout << "作为发送方发送了" << next_frame_to_send << "号帧" << std::endl;
             sendFrame(frame_kind::data, next_frame_to_send, frame_expected);
#endif
            inc(next_frame_to_send);
        }
    }

} // namespace proto::SR
