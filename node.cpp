#include "node.h"
#include <boost/algorithm/string/replace.hpp>

// TODO: handle path
// TODO: take care of comments and CDATA
// TODO: close file  myfile.close();

Node::Node(){
    if (_s_xml_file.is_open()){
        std::string line;
        std::getline(_s_xml_file, line);
        begin(line);   
    }else{
        throw "File not open";
    }
}

Node::Node(std::string& node){
    begin(node);
}

void Node::begin(std::string& node){
    std::size_t tag_end = extract_properties(node);
    if(_search_for_closing_tag){
        _s_xml_stack.push(_name);
        tag_end = handle_tag_end_in_same_line(node, tag_end);
        std::string line;
        while(!_tag_complete){
            std::getline(_s_xml_file, line);
            remove_initial_whitespaces(line);
            parse_end_tag(line);
            if(!_tag_complete){
                _child_nodes.emplace_back(line);
            }
        }
    }
}

int Node::extract_properties(std::string& str){
    if(str[0] != '<'){
        throw "Expected tag begin";
    }
    
    int i = 0;
    while(is_valid_tag_char(str[++i]));
    _name = str.substr(1,i-1);

    while(isspace(str[i]))  i++;

    return extract_attributes(str,i);   
}

int Node::extract_attributes(std::string& str, int key_start){
    
    if(str[key_start] == '/' &&  str[key_start + 1] == '>'){
        _search_for_closing_tag = false;
        return key_start+1;
    }else if(str[key_start] == '>'){
        return key_start;
    }

    std::size_t eq_pos = str.find('=', key_start);
    
    std::string key = str.substr(key_start, eq_pos - key_start); 
    // TODO: remove spaces after key end
    
    std::size_t val_start = eq_pos+1;
    while(isspace(str[val_start]))  val_start++;

    std::size_t val_end = str.find(str[val_start],val_start+1);
    std::string val = str.substr(val_start+1,val_end - val_start -1);
    replace_xml_escapes(val);

    _attributes.emplace(key, val);

    std::size_t next_key_start = val_end+1;
    while(isspace(str[next_key_start]))  next_key_start++;

    return extract_attributes(str, next_key_start);
}

int Node::handle_tag_end_in_same_line(const std::string& node, std::size_t tag_end){
    std::size_t next_tag_start = tag_end;
    while(next_tag_start < node.length() && isspace(node[next_tag_start]))  next_tag_start++;
    
    while(next_tag_start < node.length() && node[next_tag_start] != '<'){
        if(node[next_tag_start] == '"' || node[next_tag_start] == '\'' ){ 
            // Skip "" or ''
            next_tag_start = node.find(node[next_tag_start], next_tag_start+1);
            if(next_tag_start == node.npos){
                throw "Error";
            }
        }
        next_tag_start++;
    }

    if(next_tag_start < node.length() && node[next_tag_start] == '<'){
        if(next_tag_start+1 < node.length() && node[next_tag_start+1] == '/'){
            if(parse_end_tag(node, next_tag_start)){
                _text_value = node.substr(tag_end+1, next_tag_start - tag_end-1);
            }
        }else if(next_tag_start+1 < node.length() && node[next_tag_start+1] == '!'){
            // Handle comment and CDATA
        }else{
            throw "Next tag starting in same line is not handled ";
        }
    } 
}

bool Node::parse_end_tag(const std::string& str, std::size_t tag_begin /*=0*/){
    if(tag_begin+1 < str.length()){
        if(str[tag_begin] == '<' && str[tag_begin+1] == '/'){
            tag_begin += 2;
            size_t tag_end = tag_begin;
            while(is_valid_tag_char(str[tag_end])) tag_end++;
            if(str[tag_end] == '>'){
                std::string tag_name = str.substr(tag_begin,tag_end - tag_begin);
                if(tag_name == _s_xml_stack.top()){
                    if(_s_xml_stack.top() == _name){
                        _tag_complete = true;
                    }   
                    _s_xml_stack.pop();
                }else{
                    throw "Invalid XML";
                }
            }else{
                throw "Invalid XML";
            }
        }
    }
    return true;
}

bool Node::is_valid_tag_char(const char& c){
    return (isalnum(c) || c == '-' || c == '_' || c == '.');
}

void Node::replace_xml_escapes(std::string& val){
    boost::replace_all(val,"&quot;","\"");
    boost::replace_all(val,"&apos;","'");
    boost::replace_all(val,"&lt;","<");
    boost::replace_all(val,"&gt;",">");
    boost::replace_all(val,"&amp;","&");
};


void Node::remove_initial_whitespaces(std::string& str){
    int i=0;
    while(i < str.length() && isspace(str[i])) i++;
    str = str.substr(i);
}

// void Node::extract_properties(std::string& str){
//     if(str[0] != '<')
//         return;

//     std::size_t space_pos = str.find(' '); // TODO: Remove extra spaces
//     name = str.substr(1,space_pos-1);  // Extract name

//     // Extract Attributes
//     std::size_t node_end_pos = str.rfind('>');
//     if(str[node_end_pos-1] == '/')
//         search_for_closing_tag = false;

//     std::size_t eq_pos;
//     std::string val;

//     auto extract_value = [&](std::size_t start_pos) -> std::string{
//         // TODO: remove spaces at start
//         std::size_t val_end = str.find(str[start_pos],start_pos+1);
//         std::string val(str.substr(start_pos+1,val_end - start_pos -1));
//         replace_xml_escapes(val);
//         space_pos = val_end+1;
//         return val;
//     };

//     auto extract_key = [&]()-> std::string{
//         // TODO: Remove spaces at start
//         return str.substr(space_pos+1,eq_pos - space_pos - 1);
//     };

//     while(space_pos < node_end_pos){
//         eq_pos = str.find('=',space_pos);      
//         if(eq_pos == str.npos){
//             break;
//         }
//         std::string key = extract_key();
//         attributes.emplace(key, extract_value(eq_pos+1));
//     }
// }