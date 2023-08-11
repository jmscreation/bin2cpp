#include "bin2cpp_crypt.h"

// Use RIJNDAEL Encryption for this implementation
#define CRYPT_AES_IMPLEMENT
#include "crypt_aes.h"


namespace bin2cpp {

bool DataEncrypt(const char* data_in, size_t length_in, std::stringstream& data_out, const std::string& key) {
    data_out.str("");
    data_out.clear(std::ios::goodbit);

    Rijndael crypto;
    if(crypto.HasError()) return false;

    if(!crypto.LoadKey(key)) return false;

    std::string iv = crypto.GetIV(),
                data = crypto.Encrypt(data_in, length_in);

    if(!data.length()) return false;

    uint8_t ivlen = iv.size();
    data_out.write(reinterpret_cast<const char*>(&ivlen), sizeof(ivlen));

    data_out.write(iv.c_str(), iv.size());
    data_out.write(data.c_str(), data.size());

    return true;
}

bool DataDecrypt(const char* data_in, size_t length_in, std::stringstream& data_out, const std::string& key) {
    data_out.str("");
    data_out.clear(std::ios::goodbit);

    Rijndael crypto;
    if(crypto.HasError()) return false;

    if(!crypto.LoadKey(key)) return false;

    uint8_t ivlen;
    memcpy(&ivlen, data_in, sizeof(ivlen));
    if(length_in < sizeof(ivlen)) return false;

    data_in += sizeof(ivlen); // already read size
    length_in -= sizeof(ivlen);

    std::string iv(data_in, ivlen);
    if(length_in < ivlen) return false;

    data_in += ivlen; // already read iv
    length_in -= ivlen;

    if(length_in == 0) return false;

    if(!crypto.SetIV(iv)) return false;

    std::string data = crypto.Decrypt(data_in, length_in);

    if(!data.length()) return false;

    data_out.write(data.c_str(), data.size());

    return true;
}

}