#include <iostream>

#include "bin2cpp_crypt.h"
#include "embed.h"


void TestDecrypt(const std::string& key = "12345") {

    if(!resource.Decrypt(key)){
        std::cout << "Decrypt general failure\n";
        return;
    }

    std::cout << "Decrypted Data " << resource.data_length << " bytes:\n";
    std::cout << std::string_view(resource.data, resource.data_length) << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
}

void TestEncrypt() {
    std::stringstream out;
    std::string plaintext = "this is a test";
    std::string key = "12345";

    if(!bin2cpp::DataEncrypt(plaintext.c_str(), plaintext.size(), out, key)){
        std::cout << "Encrypt general failure\n";
        return;
    }

    std::cout << "<" << out.view().length() << ">\n";
    for(const char c : out.view()){
        std::cout << int(c) << ",";
    }
}


int main(int argc, char* argv[]) {
    std::string key = "";
    if(argc > 1) key = argv[1];
    TestDecrypt(key);

    //TestEncrypt();
    return 0;
}