#include<bits/stdc++.h>
#include<memory>

class Node{
public:
    Node();
    Node(std::string& node);
    void print(){} // TODO: delete

    void set_callback(std::function<void(std::string& path, std::string& name, std::shared_ptr<Node> node)> callback){
        _callback = callback;
    }


private:
    static std::ifstream _s_xml_file;
    static std::stack<std::string> _s_xml_stack;

    std::string _name;
    std::string _text_value;
    std::unordered_map<std::string, std::string> _attributes;
    std::vector<std::shared_ptr<Node>> _child_nodes;

    bool _search_for_closing_tag = true;
    bool _tag_complete = false;
    std::function<void(std::string& path, std::string& name, std::shared_ptr<Node> node)> _callback;

private:
    void begin(std::string& node);

    int extract_properties(std::string& str);
    int extract_attributes(std::string& str, int i);

    int handle_tag_end_in_same_line(const std::string& node, std::size_t tag_end);
    bool parse_end_tag(const std::string& str, std::size_t pos = 0);
    bool is_valid_tag_char(const char& c);

    void replace_xml_escapes(std::string& str);
    void remove_initial_whitespaces(std::string& str);

    
};