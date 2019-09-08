#include<bits/stdc++.h>
#include "varun_threadpool.cpp"

using namespace std;

void foo(string s){
    cout<<s;
}

int main(){
    varun_threadpool::Threadpool& threadpool = varun_threadpool::Threadpool::get_instance(2);
    threadpool.get_io_service().post(boost::bind(foo, "X"));
    threadpool.stop();
    threadpool.wait_for_completion();
    return 0;
}