#include<bits/stdc++.h>
#include "file_reader.cpp"
#include<memory>

class Node : public std::enable_shared_from_this<Node>{
private:    
    using callback_type = std::function<void(std::string path, std::string name, std::shared_ptr<Node> node)>;

public:
    Node(callback_type callback = NULL, std::string path = "");
    
    void begin_parsing();
    void begin_parsing(std::string& node);

    //Getters
    std::string& get_value();
    std::string& get_attribute(std::string key);


private:
    std::ifstream& _reader;
    static std::stack<std::string> _s_xml_stack; // used to validate XML tag ordering

    std::string _name; // Node's name
    std::string _path; // Current path
    std::string _text_value; // Current node's value
    std::unordered_map<std::string, std::string> _attributes; // Node Attributes
    std::vector<std::shared_ptr<Node>> _child_nodes; // Pointer to child nodes

    bool _search_for_closing_tag = true;
    bool _tag_complete = false;

    std::size_t _end_tag_end_pos;
    callback_type _callback = NULL;
        
    const std::string _cdata_beg_str = "<![CDATA[";
    const std::string _cdata_end_str = "]]>";
private:

    int extract_properties(std::string& str); // Extracts name and attributes
    int extract_attributes(std::string& str, int i); // Extracts attributes


    bool parse_end_tag(const std::string& str, std::size_t pos = 0); // Verifies tag end and checks if xml was valid. Also calls callbacks.

    inline bool is_valid_tag_name_char(const char& c); // Check if char is valid char for tag name

    inline void replace_xml_escapes(std::string& str); // Replace escape escapes to original chars

    void parse_value(std::string& str, std::size_t start_from); // Parse value of current node: <p> Hello World </p>

    inline std::string get_next_line(); // Retrives next line from XML file and performs initial preprocessing.
    inline void remove_initial_whitespaces(std::string& str);
    void ignore_comments(std::string& str);
    int handle_cdata(std::string& str, long cdata_begin);
    
};