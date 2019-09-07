#include "node.h"
#include <boost/algorithm/string/replace.hpp>

Node::Node(std::string& node){
    extract_properties(node);
}

void Node::extract_properties(std::string& str){
    if(str[0] != '<')
        return;

    std::size_t space_pos = str.find(' '); // TODO: Remove extra spaces
    name = str.substr(1,space_pos-1);  // Extract name

    std::size_t node_end_pos = str.rfind('>');
    if(str[node_end_pos-1] == '/')
        search_for_closing_tag = false;

    std::size_t eq_pos;
    std::string val;

    auto replace_escapes = [](std::string& val){
        boost::replace_all(val,"&quot;","\"");
        boost::replace_all(val,"&apos;","'");
        boost::replace_all(val,"&lt;","<");
        boost::replace_all(val,"&gt;",">");
        boost::replace_all(val,"&amp;","&");
    };

    auto extract_value = [&](std::size_t start_pos) -> std::string{
        // TODO: remove spaces at start
        std::size_t val_end = str.find(str[start_pos],start_pos+1);
        std::string val(str.substr(start_pos+1,val_end - start_pos -1));
        replace_escapes(val);
        space_pos = val_end+1;
        return val;
    };

    auto extract_key = [&]()-> std::string{
        // TODO: Remove spaces at start
        return str.substr(space_pos+1,eq_pos - space_pos - 1);
    };

    while(space_pos < node_end_pos){
        eq_pos = str.find('=',space_pos);      
        if(eq_pos == str.npos){
            break;
        }
        std::string key = extract_key();
        properties.emplace(key, extract_value(eq_pos+1));
    }
}