#include "xml_parser.cpp"
#include <gtest/gtest.h>

std::stack<std::string> Node::_s_xml_stack;

void TestNodeViaXML(){
    FileReader::get_instance("test.xml");
    Node n;
    n.begin_parsing();
    Node k;
    k.begin_parsing();
    Node j;
    j.begin_parsing();
    //j.print();
}

void foo(std::string path, std::string name, std::shared_ptr<Node> node){
    std::cout<<name<<std::endl;
}

void CallbackTest(){
    FileReader::get_instance("test.xml");
    // EXCLUSIVE to TestNodeViaXML test.
    std::shared_ptr<Node> n= std::make_shared<Node>(foo);
    n->begin_parsing();
    std::shared_ptr<Node> j= std::make_shared<Node>(foo);
    j->begin_parsing();
    std::shared_ptr<Node> k= std::make_shared<Node>(foo);
    k->begin_parsing();
}

void XMLParserTest(){
    XMLParser parser("test.xml", foo);
    parser.beginParsing();
}


int main(){
    //TestNodeViaXML();
    //CallbackTest();
    XMLParserTest();
}