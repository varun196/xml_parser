#include "xml_parser.h"
#include "node.h"

XMLParser::XMLParser(std::string input_file): 
        xml_file(input_file)
{

}

void XMLParser::beginParsing(){
    std::string line;

    while(std::getline(xml_file, line)){

    }
}
