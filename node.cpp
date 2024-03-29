#include "node.h"
#include <boost/algorithm/string/replace.hpp>

// TODO: Send callbacks on separate thread

Node::Node(callback_type callback, std::string path): 
_callback(callback), 
_path(path),
_reader(FileReader::get_instance().get_reader())
{}


std::string& Node::get_value(){
    return _text_value;
}

std::string& Node::get_attribute(std::string key){
    auto itr = _attributes.find(key);
    if(itr != _attributes.end()){
        return itr->second;
    }else{
        throw "Error: No such key: key " + key +" node name: "+_name + " path:"+ _path;
    }
}

void Node::begin_parsing(){
    if (_reader.is_open()){
        std::string line = get_next_line();
        begin_parsing(line);   
    }else{
        throw "File not open";
    }
}

void Node::begin_parsing(std::string& node){
    remove_initial_whitespaces(node);
    std::size_t tag_end = extract_properties(node);
    if(_search_for_closing_tag){
        _s_xml_stack.push(_name);
        parse_value(node, tag_end+1); // Parse value present in current node
        std::string line;
        while(!_tag_complete){      // Till the current tag is not complete, all the tags within are child tags
            line = get_next_line();
            parse_end_tag(line);          // If the next line contains tag end, parse it.
            if(!_tag_complete){           // if end tag is still not found
                if(line[0] == '<' && line[1] != '!'){ // if this is a child tag, parse it recursively
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

void Node::parse_value(std::string& str, std::size_t start_from){
    std::size_t next_tag_start = start_from;
    if(str[start_from] == '<' && start_from+1 <str.length() && str[start_from+1] != '!'){
        if(str[start_from] == '<' && start_from+1 <str.length() && str[start_from+1] == '/'){
            parse_end_tag(str,start_from);
        }
        return;
    }
    
    // Find start of end tag or cdata or newtag
    while(next_tag_start < str.length() && str[next_tag_start] != '<') next_tag_start++;
    
    // if end tag is not found, entire thing is value.
    if(next_tag_start == str.length()){
        _text_value.append(str.substr(start_from));
    }

    // if this is end tag
    if(next_tag_start+1 < str.length() && str[next_tag_start+1] == '/'){
        _text_value.append(str.substr(start_from, next_tag_start - start_from));
        parse_end_tag(str, next_tag_start);
    }else if(next_tag_start+1 < str.length() && str[next_tag_start+1] == '!'){
        handle_cdata(str, next_tag_start);
    }else{
        // Handle nested tag in same line
        if(next_tag_start < str.length() && str[next_tag_start] == '<'){
            _text_value.append(str.substr(start_from, next_tag_start-start_from));
            std::shared_ptr<Node> ip_node= std::make_shared<Node>(_callback, _path);    // Create nested node
            std::string line = str.substr(next_tag_start);
            ip_node->begin_parsing(line);                           // Ask nested node to start parsing.
            _child_nodes.emplace_back(ip_node);                     // Save reference to nested node.
            if(ip_node->_end_tag_end_pos != line.length()-1){       // If child node's end tag is not the line's end: "<b><i>hi</i>there</b>"
                str = line.substr(ip_node->_end_tag_end_pos+1);     // Then current node has value ("there")
                parse_value(str,0 );                                // parse that value
            }
        }
    }
    
    return ;  

}

int Node::extract_properties(std::string& str){
    // Extracts name and attributes. Calls callback if tag is empty tag: <abc />
    // Returns pos of current tag end: '>'
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
    
    if(str[key_start] == '/' &&  str[key_start + 1] == '>'){ // if tag of form : <name ... />
        _search_for_closing_tag = false;
        if(_callback){
            _callback(_path,_name,shared_from_this());
        }
        return key_start+1;
    }else if(str[key_start] == '>'){ // If this is tag end
        return key_start;
    }

    std::size_t eq_pos = str.find('=', key_start);
    
    std::string key = str.substr(key_start, eq_pos - key_start); 
    // TODO: remove spaces after key end
    
    std::size_t val_start = eq_pos+1;
    while(isspace(str[val_start]))  val_start++;

    std::size_t val_end = str.find(str[val_start],val_start+1);         // Find ' or " depending on how value startes
    std::string val = str.substr(val_start+1,val_end - val_start -1);
    replace_xml_escapes(val);

    _attributes.emplace(key, val);

    std::size_t next_key_start = val_end+1;
    while(isspace(str[next_key_start]))  next_key_start++;

    return extract_attributes(str, next_key_start);
}

bool Node::parse_end_tag(const std::string& str, std::size_t tag_begin /*=0*/){

    if(tag_begin+1 < str.length()){
        if(str[tag_begin] == '<' && str[tag_begin+1] == '/'){ // If start of end tag
    
            tag_begin += 2;
            size_t tag_end = tag_begin;
            while(is_valid_tag_name_char(str[tag_end])) tag_end++;

            if(str[tag_end] == '>'){ // If end of end tag

                std::string tag_name = str.substr(tag_begin,tag_end - tag_begin);
                if(tag_name == _s_xml_stack.top()){ // Ensures validity of xml

                    if(_s_xml_stack.top() == _name){   // If current tag is closed 
                        _tag_complete = true;
                        _end_tag_end_pos = tag_end;
                    }   

                    _s_xml_stack.pop();
                    
                    if(_callback){
                        _callback(_path,_name,shared_from_this());
                    }

                }else{
                    throw "Invalid XML: tag_name" + tag_name +" stack_top:" + _s_xml_stack.top();
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

void Node::remove_initial_whitespaces(std::string& str){
    int i=0;
    while(i < str.length() && isspace(str[i])) i++;
    str = str.substr(i);
}

void Node::ignore_comments(std::string& str){
    std::size_t comm_beg = str.find("<!--");
    if(comm_beg == str.npos)    return;
    std::size_t comm_end = str.find("-->", comm_beg) + 3;
    str.erase(comm_beg, comm_end-comm_beg);
    ignore_comments(str);
}

int Node::handle_cdata(std::string& str, long cdata_begin){
    // Verify this is cdata
    if(str.substr(cdata_begin, _cdata_beg_str.length()) != _cdata_beg_str) return -1;
    // Parse CDATA
    std::size_t end_cdata =str.find(_cdata_end_str);
    while(end_cdata == str.npos){
        _text_value.append(str.substr(cdata_begin +_cdata_beg_str.length()));
        str = get_next_line();
        cdata_begin = -1 * _cdata_beg_str.length();
        end_cdata = str.find(_cdata_end_str);
    }
    _text_value.append(str.substr(cdata_begin +_cdata_beg_str.length(), end_cdata - cdata_begin - _cdata_beg_str.length()));
    return end_cdata + _cdata_end_str.length();
}
