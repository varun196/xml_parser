#include<bits/stdc++.h>

// TODO: hold filereader

class XMLParser{
public:
    XMLParser(std::string input_file);
    void beginParsing();

private:
    std::stack<std::string> xml_stack;
    std::ifstream xml_file;

};
