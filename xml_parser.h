#include<bits/stdc++.h>

class XMLParser{
    using callback_type = std::function<void(std::string path, std::string name, std::shared_ptr<Node> node)>;

public:
    XMLParser(std::string input_file, callback_type callback);
    void beginParsing();
    void set_callback(callback_type);

private:
    callback_type _callback;
    FileReader& _fileReaderReference;
    std::ifstream& _reader;
};
