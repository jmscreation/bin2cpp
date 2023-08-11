#include <iostream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <fstream>
#include <cmath>
#include <ctime>

#include "bin2cpp_crypt.h"

namespace fs = std::filesystem;

// Quick String Filter
bool filterString(std::string& str, const std::string& substr, const std::string& newstr){
    auto pos = str.find(substr);
    if(pos == std::string::npos) return false;

    str.replace(pos, substr.size(), newstr);
    return true;
}

// Convert To C++ Char Array

bool Bin2Cpp(const std::string& name, const std::string& key, const std::string& in, const std::string& out) {
    std::string out_cpp = out + ".cpp", out_h = out + ".h", macrodefine = name;

    std::ifstream input(in, std::ios::binary | std::ios::in);
    if(!input.is_open()){
        std::cout << "Failed to read binary file" << std::endl;
        return false;
    }

    input.seekg(0, input.end);
    size_t flen = input.tellg(); // get file length
    input.seekg(0, input.beg);
    
    fs::path cppfile(out_cpp);

    std::ofstream output(out_cpp, std::ios::binary | std::ios::out);
    if(!output.is_open()){
        std::cout << "Failed to open " << (cppfile.filename().string() + cppfile.extension().string()) << " file" << std::endl;
        input.close();
        return false;
    }

    std::cout << "Converting and encrypting binary to c++..." << std::endl;


    std::stringstream encbuf;
    {
        std::stringstream filebuf;
        while(!input.eof()){ // read file into memory
            char buf[1024];
            size_t len = input.readsome(buf, 1024);

            if(len == 0) break;

            filebuf.write(buf, len);
        }

        if(!bin2cpp::DataEncrypt(filebuf.view().data(), filebuf.view().size(), encbuf, key)){
            std::cout << "Failed to encrypt binary data!\n";
            return false;
        }
    }
    flen = encbuf.view().length(); // update file length to encrypted data size

    output << "#include \"" << out_h << "\"\nbin2cpp::Resource<" << flen << "> " << name << " {{"; // header information
    bool begin = true;
    for(const char c : encbuf.view()){
        output << (begin ? "" : ",") << static_cast<int16_t>(c);
        begin = false;
    }
    output << "}};";

    input.close();
    output.close();

    fs::path hfile(out_h);

    // Write header file
    std::ofstream header(out_h, std::ios::binary | std::ios::out);
    if(!header.is_open()){
        std::cout << "Failed to open " << (hfile.filename().string() + hfile.extension().string()) << " file" << std::endl;
        return false;
    }

    // Upper case macro definition
    std::transform(macrodefine.begin(), macrodefine.end(), macrodefine.begin(),
                   [](unsigned char c) -> unsigned char { return std::toupper(c); });

    header <<
        "#ifndef __" << macrodefine << "__\n" <<
        "#define __" << macrodefine << "__\n\n" <<
        "#include \"bin2cpp_crypt.h\"\n" <<
        "extern bin2cpp::Resource<" << flen << "> " << name << ";\n\n" <<
        "#endif // __" << macrodefine << "__";

    header.close();

    return true;
}


// Main Program

int main(int argc, char** argv) {
    srand(time(0));

    // Display Help
    if(argc < 3) {
        std::cout << "==== Bin2Cpp -> Convert a binary file to a raw cpp file for encrypted embedded data ====\n"
                  << "Usage:\n"
                  << "bin2cpp <input-file> <encryption-key> *<resource-name> (resource$n) *<output-file> (binary$n)\n"
                  << "$n = random number\n"
                  << "<argument value>\n"
                  << "*<optional argument>\n"
                  << "(default value)\n"
                  << "\n"
                  << "Examples:\n"
                  << "bin2cpp file.ext secret\n"
                  << "bin2cpp test.txt Pa$$w0rd TestTextFile\n"
                  << "bin2cpp run.exe \"Pwd Enc-payload\" RunExe bin_runexe\n";
        return 0;
    }


    std::string input = argv[1], // get resource file path
                secret = argv[2], // get resource encryption secret
                name = "resource$n",
                output = "binary$n";
    if(argc > 3){
        name = argv[3]; // update resource name
    }
    if(argc > 4){
        output = argv[4]; // update output filename
    }

    do {
        
        // random number substitution
        if(filterString(name, "$n", std::to_string(rand()))) continue;
        if(filterString(input, "$n", std::to_string(rand()))) continue;
        if(filterString(output, "$n", std::to_string(rand()))) continue;

        break;
    } while(1);

    return !Bin2Cpp(name, secret, input, output);
}