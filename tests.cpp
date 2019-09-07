#include "node.cpp"
#include <gtest/gtest.h>

// class NodeTest : public testing::Test{
// public:
//     NodeTest(){}
// };

// TEST_F(NodeTest, NodeParsingWorks) { 
    
// }

void TestNode(){
    std::string s = "<name k1=\"v1=xyz\" k2=\"v2\" />";
    Node n(s);
    s = "<name k2=\"v2&quot;\" />";
    Node n2(s);
}

int main(){
    TestNode();
}