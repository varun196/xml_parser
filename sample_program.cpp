#include "xml_parser.cpp"

int main(){
    std::string file_name;
    std::cout<<"Enter relative / absolute file name:\n";
    std::cin>>file_name;
    //std::cout<<std::endl<<file_name;
    //return 0;
    bool print_id = false;
    auto callback = [&](std::string path, std::string name, std::shared_ptr<Node> node){
        if(name == "amount"){
            std::string& amount = node->get_value();
            int amt = stoi(amount);
            if(amt > 100){
                print_id = true;
            }
        }else{
            if(name == "order" && print_id){
                std::cout<<node->get_attribute("id")<<std::endl;
                print_id = false;
            }
        }
    };

    XMLParser parser(file_name, callback);
    parser.beginParsing();
    return 0;
}