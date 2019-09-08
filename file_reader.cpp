#include<bits/stdc++.h>

// Note: Singleton won't work if we need to parse two files. 
// Solution: Create a file reader instance in parser and 
// send this instance to node.
// Left -- Implement if needed.

class FileReader{
    // Singleton
public:
    static FileReader& get_instance(std::string input_file = ""){
        static FileReader instance(input_file);
        return instance;
    }

    FileReader(FileReader const&)      = delete;
    void operator=(FileReader const&)  = delete;

    std::ifstream& get_reader(){
        return _s_xml_stream;
    }

private:
    std::ifstream _s_xml_stream;
    FileReader(std::string input_file): _s_xml_stream(input_file)  {}
};