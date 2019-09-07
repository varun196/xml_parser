#include<bits/stdc++.h>
#include<string_view>

class Node{
public:
    Node(std::string& node);

    static 

    // Members
    std::string name;
    std::string text_value;
    std::unordered_map<std::string, std::string> properties;
    std::vector<std::unique_ptr<Node>> child_nodes;
    bool search_for_closing_tag = true;

private:
    void extract_properties(std::string& str);
};