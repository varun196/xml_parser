#include "xml_parser.h"

/*
TODO:
XML escape characters

There are only five:

"   &quot;
'   &apos;
<   &lt;
>   &gt;
&   &amp;

*/

XMLParser::XMLParser(std::string input_file): 
        xml_file(input_file)
{}

void XMLParser::beginParsing(){
    std::string line;

    while(std::getline(xml_file, line)){
        
    }
}
