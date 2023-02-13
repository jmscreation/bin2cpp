#pragma once
/*
    Encryption and Decryption for binary data embedded within the executable

    Encryption is for bin2cpp executable
    Decryption is for end-user application to decrypt binary data embedded
*/

#include <string>
#include <array>
#include <sstream>

namespace bin2cpp {


// Internal Encryption API Endpoint for Bin2Cpp
bool DataEncrypt(const char* data_in, size_t length_in, std::stringstream& data_out, const std::string& key); // abstraction for encryption
bool DataDecrypt(const char* data_in, size_t length_in, std::stringstream& data_out, const std::string& key); // abstraction for decryption

// wrapper class to keep the actual external resource data 
template<size_t LENGTH>
class Resource {
    const std::array<char, LENGTH> raw; // internal unprocessed static buffer
    std::stringstream processed_data; // internal processed buffer

    void PrepData() {

        processed_data.seekg(0, std::ios_base::end);
        data_length = processed_data.tellg(); // update data_length (size_t)
        processed_data.seekg(0, std::ios_base::beg);

        data = processed_data.view().data(); // update data (const char*)
    }

public:
    const char* embedded;   // raw data in file
    size_t embedded_length; // raw data length

    const char* data;       // processed data in memory - for end user this is the decrypted data
    size_t data_length;     // processed data length

    Resource(const std::array<char, LENGTH>& rs):
        raw(rs), embedded(raw.data()), embedded_length(raw.size()), data(nullptr), data_length(0) {}

    ~Resource() { FreeMemory(); }

    bool Decrypt(const std::string& key) {
        if(!DataDecrypt(embedded, embedded_length, processed_data, key)) return false;
        PrepData();
        if(processed_data.fail()){
            FreeMemory();
            return false;
        }
        return true;
    }

    // call to free memory resources when no longer needed
    void FreeMemory() {
        processed_data.str("");
        processed_data.clear(std::ios::goodbit);
        data = nullptr;
        data_length = 0;
    }
};

}