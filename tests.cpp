#include "node.cpp"
#include <gtest/gtest.h>

std::stack<std::string> Node::_s_xml_stack;
std::ifstream Node::_s_xml_file("test.xml");


void TestNodeViaXML(){
    Node n;
    n.begin_parsing();
    Node k;
    k.begin_parsing();
    Node j;
    j.begin_parsing();
    j.print();
}

void foo(std::string path, std::string name, std::shared_ptr<Node> node){
    std::cout<<name<<std::endl;
}

void CallbackTest(){
    std::shared_ptr<Node> n= std::make_shared<Node>(foo);
    n->begin_parsing();
    std::shared_ptr<Node> j= std::make_shared<Node>(foo);
    j->begin_parsing();
    std::shared_ptr<Node> k= std::make_shared<Node>(foo);
    k->begin_parsing();
}

int main(){
    //TestNode();
    //TestNodeViaXML();
    CallbackTest();
}