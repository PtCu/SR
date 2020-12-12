#include "SR.h"
int main(){

    proto::SR::SR s("127.0.0.1","127.0.0,1",8080,9090);
    s.start();
}