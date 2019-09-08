#include "node.cpp"
#include <gtest/gtest.h>

std::stack<std::string> Node::_s_xml_stack;

void TestNode(){
    std::string s;
    s = "<name k1=\"v1\" />";
    Node n1(s);
    s = "<name k1=\"v1=xyz\" k2=\"v2\" />";
    Node n2(s);
    s = "<name k1=\"v1&quot;\" />";
    Node n3(s);
    s = "<name k1=\"v1\"> Hello </name>";
    Node n4(s);
}

int main(){
    TestNode();
}