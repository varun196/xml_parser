#include<bits/stdc++.h>
using namespace std;

function<void(string)> callback;

void set_callback(function<void(string)> c){
    callback = c;
}
void execute_callback(string s){
    callback(s);
}

void foo(string s){
    cout<<s;
}

int main(){
    set_callback(foo);
    execute_callback("Hello");
    return 0;
}

