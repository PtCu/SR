#include "SR.h"
int main(){

    proto::SR::SR s("127.0.0.1","127.0.0,1",9090,8080);
    s.start();
}