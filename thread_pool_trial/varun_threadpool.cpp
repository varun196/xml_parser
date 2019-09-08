#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include<bits/stdc++.h>

using namespace std;

namespace varun_threadpool{
    class Threadpool {
        /*
            Assign tasks as: 
            ioService.post(boost::bind(myTask, "Hello World!"));

            Boost bind reference: https://www.boost.org/doc/libs/1_54_0/libs/bind/bind.html#with_functions
        */
    public:
        // Singleton
        static  Threadpool& get_instance(int num_threads){
            static Threadpool instance(num_threads);
            return instance;
        }   

        Threadpool(Threadpool const&)   = delete;
        void operator=(Threadpool const&)  = delete;

        boost::asio::io_service& get_io_service(){
            return _ioService;
        }

        void stop(){    
            // any tasks added after stop() won't execute
            _ioService.stop();  
        }

        void wait_for_completion(){
            _threadpool.join_all();
        }

        ~Threadpool(){
            stop();
            wait_for_completion();
        }

    private:
        // Reference: https://stackoverflow.com/questions/19500404/how-to-create-a-thread-pool-using-boost-in-c
        boost::asio::io_service _ioService;
        boost::thread_group _threadpool;
        boost::asio::io_service::work _work;

        Threadpool(int num_threads): _work(_ioService){
            for(int i = 0; i < num_threads; i++){
                _threadpool.create_thread(
                    boost::bind(&boost::asio::io_service::run, &_ioService)
                );
            }
        }
    };
}

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
