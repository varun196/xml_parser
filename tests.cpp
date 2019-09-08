#include "node.cpp"
#include <gtest/gtest.h>

std::stack<std::string> Node::_s_xml_stack;
std::ifstream Node::_s_xml_file("test.xml");


void TestNodeViaXML(){
    Node n;
    //n.begin_parsing();
    Node k;
    //k.begin_parsing();
    Node j;
    //j.begin_parsing();
    j.print();
}

void CallbackTest(){

}

int main(){
    //TestNode();
    TestNodeViaXML();
}