#include "node.h"
#include <boost/algorithm/string/replace.hpp>

// TODO: handle CDATA
// TODO: close file  myfile.close();
// TODO: handle exceptions
// TODO: Send callbacks on separate thread

Node::Node(callback_type callback, std::string path): 
_callback(callback), 
_path(path),
_reader(FileReader::get_instance().get_reader())
{}

std::string Node::get_next_line(){
    std::string line;
    if(_reader.peek() == std::ifstream::traits_type::eof()){
        throw "unexpected eof -- Invalid xml?";
    }
    std::getline(_reader, line);
    remove_initial_whitespaces(line);
    ignore_comments(line);
    return line;
}

void Node::begin_parsing(){
    if (_reader.is_open()){
        std::string line = get_next_line();
        begin_parsing(line);   
    }else{
        throw "File not open";
    }
}

void Node::parse_value(std::string& str, std::size_t start_from){
    std::size_t next_tag_start = start_from;
    if(str[start_from] == '<' && start_from+1 <str.length() && str[start_from+1] != '!'){
        if(str[start_from] == '<' && start_from+1 <str.length() && str[start_from+1] == '/'){
            parse_end_tag(str,start_from);
        }
        return;
    }
    
    // Find start of end tag or cdata or newtag
    while(next_tag_start < str.length() && str[next_tag_start] != '<'){
        if(str[next_tag_start] == '"' || str[next_tag_start] == '\'' ){ 
            // Skip "" or ''
            next_tag_start = str.find(str[next_tag_start], next_tag_start+1);
            if(next_tag_start == str.npos){ // if ending " or ' not in current line 
                _text_value.append(str.substr(start_from));
                str = get_next_line();
                next_tag_start = start_from = 0;
                continue;
            }
        }
        next_tag_start++;
    }

    // if end tag is not found, entire thing is value.
    if(next_tag_start == str.length()){
        _text_value.append(str.substr(start_from));
    }

    // if this is end tag
    if(next_tag_start+1 < str.length() && str[next_tag_start+1] == '/'){
        _text_value = str.substr(start_from, next_tag_start - start_from);
        parse_end_tag(str, next_tag_start);
    }else if(next_tag_start+1 < str.length() && str[next_tag_start+1] == '!'){
        handle_cdata(str, next_tag_start);
    }else{
        //throw "Next tag starting in same line is not handled ";
    }
    
    return ;  // TODO: return if two tags in same lines are handled

}

int Node::handle_cdata(std::string& str, long cdata_begin){
    // Verify this is cdata
    if(str.substr(cdata_begin, _cdata_beg_str.length()) != _cdata_beg_str) return -1;

    std::size_t end_cdata =str.find(_cdata_end_str);
    while(end_cdata == str.npos){
        _text_value.append(str.substr(cdata_begin +_cdata_beg_str.length()));
        str = get_next_line();
        cdata_begin = -1 * _cdata_beg_str.length();
        end_cdata =str.find(_cdata_end_str);
    }
    _text_value.append(str.substr(cdata_begin +_cdata_beg_str.length(), end_cdata - cdata_begin - _cdata_beg_str.length()));
    return end_cdata + _cdata_end_str.length();
}

void Node::begin_parsing(std::string& node){
    remove_initial_whitespaces(node);
    std::size_t tag_end = extract_properties(node);
    if(_search_for_closing_tag){
        _s_xml_stack.push(_name);
        parse_value(node, tag_end+1);
        std::string line;
        while(!_tag_complete){
            line = get_next_line();
            parse_end_tag(line);
            if(!_tag_complete){
                if(line[0] == '<' && line[1] != '!'){ // if this is a tag
                    std::shared_ptr<Node> ip_node= std::make_shared<Node>(_callback, _path);
                    ip_node->begin_parsing(line);
                    _child_nodes.emplace_back(ip_node);
                }else{
                    parse_value(line,0);
                }
            }
        }
    }
}

void Node::ignore_comments(std::string& str){
    std::size_t comm_beg = str.find("<!--");
    if(comm_beg == str.npos)    return;
    std::size_t comm_end = str.find("-->", comm_beg) + 3;
    str.erase(comm_beg, comm_end-comm_beg);
    ignore_comments(str);
}

int Node::extract_properties(std::string& str){
    // Extracts name and attributes
    if(str[0] != '<'){
        throw "Expected tag begin";
    }

    if(str[1] == '!'){
        return handle_cdata(str, 0);
    }

    if(str[1] == '?'){
        str = get_next_line();
    }

    int i = 0;
    while(is_valid_tag_name_char(str[++i]));
    _name = str.substr(1,i-1);
    if(_path != "") _path += "/";
    _path += _name;

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

bool Node::parse_end_tag(const std::string& str, std::size_t tag_begin /*=0*/){
    if(tag_begin+1 < str.length()){
        if(str[tag_begin] == '<' && str[tag_begin+1] == '/'){
            tag_begin += 2;
            size_t tag_end = tag_begin;
            while(is_valid_tag_name_char(str[tag_end])) tag_end++;
            if(str[tag_end] == '>'){
                std::string tag_name = str.substr(tag_begin,tag_end - tag_begin);
                if(tag_name == _s_xml_stack.top()){
                    if(_s_xml_stack.top() == _name){
                        _tag_complete = true;
                    }   
                    _s_xml_stack.pop();
                    if(_callback != NULL){
                        _callback(_path,_name,shared_from_this());
                    }
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

bool Node::is_valid_tag_name_char(const char& c){
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