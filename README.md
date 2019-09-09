# XML Parser

A forward-only XML parser that parses xml.  
Executes callbacks indicating values at each node.   

Handles:

1. XML comments
2. CDATA sections
3. XML escapes:  
```
    &quot;    
    &apos;
    &lt;
    &gt;
    &amp;
```

## Running demo

Execute below line in terminal in the directory where the repo is cloned.  
```
    g++ sample_program.cpp -o sample_program && ./sample_program < "./data/sample_prog_input.txt"
```

The sample program takes in a sample XML and outputs ids of orders whose amounts exceed 100.  

## Files  

1. `file_reader.cpp` : Handles fstream for given file. Currently a singleton.  
2. `xml_parser.h` and `xml_parser.cpp` : The XML parser. Initiates and continues XML parsing.  
3. `node.h` and `node.cpp` : Contains the bulk of logic. Responsible for parsing XML and storing the results.
4.  `sample_program.cpp` : The demo program.  
5. `tests.cpp` : Used for testing.

## Data

1. `sample_prog_input.txt` : The terminal input for sample program.
2. `test.xml` : Testing data  
3. `sample_program_test.xml` : Data used for sample program.
4. `standard.xml` : Used by tests.cpp to test performance. Has 2 million lines of XML. 

## Details

### Callback syntax
The callback syntax is:  
```c++
    std::function<void(std::string path, std::string name, std::shared_ptr<Node> node)>;
```
where path is the path to current tag, name is the name of current tag and node contains data of the tag.  

### Node public functions

Currently, node supports only two public member functions:

```c++
    std::string& get_value();
    std::string& get_attribute(std::string key);
```

This can be expanded to allow iteration over node.

### Node structure  

Each node contains the tag's name, the path till that tag,tag attributes, tag value and pointer to child nodes.  

The pointer to child nodes can be used to get the entire XML subtree.  

Note that smart pointers are returned, so if callback does not save root level node, it will be deleted from memory.

```c++
    std::string _name; // Node's name
    std::string _path; // Current path
    std::string _text_value; // Current node's value
    std::unordered_map<std::string, std::string> _attributes; // Node Attributes
    std::vector<std::shared_ptr<Node>> _child_nodes; // Pointer to child nodes

    callback_type _callback = NULL;
```

### Running tests

To run test on standard.xml, execute:  
```
   g++ tests.cpp -o test && ./test > res.txt
```

Please note that some of the tests are mutually exclusive.

### Current limitations

- Spaces at start of XML are ignored, which may be an issue if they are part of value.  
- Does not handle beginning of non-nested tag on same line.  
- Expects special XML tags like `<?xml ...?>` on its own line

### Future improvements

- [ ] Improve space handling  
- [ ] Send callbacks on separate thread
- [ ] Have `xml_parser` manage `file_reader` instances and make the latter non-singleton
- [ ] Automate tests via google_test
