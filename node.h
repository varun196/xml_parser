#include<bits/stdc++.h>
#include<string_view>

class Node{
public:
    Node();
    Node(std::string& node);

private:
    static std::ifstream _s_xml_file;
    static std::stack<std::string> _s_xml_stack;

    // Members
    std::string _name;
    std::string _text_value;
    std::unordered_map<std::string, std::string> _attributes;
    std::vector<Node> _child_nodes;
    bool _search_for_closing_tag = true;

private:
    void begin(std::string& node);

    int extract_properties(std::string& str);
    int extract_attributes(std::string& str, int i);

    bool parse_end_tag(std::string& str, std::size_t pos = 0);
    bool is_valid_tag_char(char& c);
    void replace_xml_escapes(std::string& str);
};