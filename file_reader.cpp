#include<bits/stdc++.h>

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