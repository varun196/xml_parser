#include "node.cpp"
#include "xml_parser.h"

XMLParser::XMLParser(std::string input_file, callback_type callback):
    _fileReaderReference(FileReader::get_instance(input_file)),
    _callback(callback),
    _reader(_fileReaderReference.get_reader())
{}

void XMLParser::beginParsing(){
    try{
        while(_reader.peek() != std::ifstream::traits_type::eof()){
            std::shared_ptr<Node> n= std::make_shared<Node>(_callback);
            n->begin_parsing();
            n = nullptr;
        }
    }catch(const std::exception& e){
        std::cout<<e.what();
    }catch(const std::string& e){
        std::cout<<e;
    }catch(const char* e){
        std::cout<<e;
    }
}
