#pragma once

#define LTM_RIJNDAEL
#define LTM_DESC
#include "tomcrypt.h"

#include <iostream>
#include <string>
#include <algorithm>

class Rijndael {
    uint8_t key[MAXBLOCKSIZE];
    uint8_t iv[MAXBLOCKSIZE];

    int aes_idx, prng_idx, hash_idx,
        keysize, ivsize;

    bool error;

    bool InitSystem();

public:
    Rijndael();
    virtual ~Rijndael();

    inline bool HasError() const { return error; }

    std::string sha256_hash(const std::string& data);
    std::string prng_generate();


    bool GenerateKey();
    bool LoadKey(const std::string& plaintext);

    bool ImportKey(const std::string& datakey);
    std::string ExportKey();

    void ClearKey();

    bool SetIV(const std::string& dataiv);
    bool NewIV();
    std::string GetIV();

    std::string Encrypt(const char* data, size_t length);
    std::string Decrypt(const char* data, size_t length);
};



#ifdef CRYPT_AES_IMPLEMENT
#undef CRYPT_AES_IMPLEMENT

/*
    Implementation Of Rijndael TomCrypt Wrapper
*/

Rijndael::Rijndael(): error(false) {
    if(!InitSystem()){
        error = true;
    }
}

Rijndael::~Rijndael() {
    unregister_hash(&sha256_desc);
    unregister_prng(&sprng_desc);
}

bool Rijndael::InitSystem() {
    if(register_cipher(&aes_desc) == -1){
        std::cout << "aes_desc failure\n";
        return false;
    }

    if(register_prng(&sprng_desc) == -1){
        std::cout << "prng failure\n";
        return false;
    }

    if(register_prng(&yarrow_desc) == -1){
        std::cout << "yarrow failure\n";
        return false;
    }

    if(register_hash(&sha256_desc) == -1){
        std::cout << "sha256 failure\n";
        return false;
    }

    ltc_mp = ltm_desc;

    aes_idx = find_cipher("aes");
    prng_idx = find_prng("sprng");
    hash_idx = find_hash("sha256");

    if (aes_idx == -1) {
        std::cout << "missing aes cipher\n";
        return false;
    }
    if (prng_idx == -1) {
        std::cout << "missing sprng\n";
        return false;
    }
    if (hash_idx == -1) {
        std::cout << "missing sha256\n";
        return false;
    }

    ivsize = cipher_descriptor[aes_idx].block_length;
    keysize = hash_descriptor[hash_idx].hashsize;

    if (cipher_descriptor[aes_idx].keysize(&keysize) != CRYPT_OK) {
        std::cout << "Invalid keysize\n";
        return false;
    }

    if(!NewIV()) return false;

    return true;
}

std::string Rijndael::sha256_hash(const std::string& data) {
    char hashbuf[32]; // SHA256 32 bytes
    unsigned long hashlen = sizeof(hashbuf);
    int code = hash_memory(hash_idx, reinterpret_cast<const uint8_t*>(data.c_str()), data.size(), (uint8_t*)hashbuf, &hashlen);
    if(code != CRYPT_OK){
        std::cout << "hash failure: " << error_to_string(code) << "\n";
        return "";
    }
    std::string hash(hashbuf, hashlen);
    return hash;
}

std::string Rijndael::prng_generate() {
    uint8_t out[64];
    
    unsigned long read = sprng_read(out, sizeof(out), NULL);
    if(!read){
        std::cout << "sprng failed\n";
        return "";
    }
    std::string data(reinterpret_cast<const char*>(out), read);

    return data;
}

bool Rijndael::LoadKey(const std::string& plaintext) {
    unsigned long outlen;
    if(int code = hash_memory(hash_idx, reinterpret_cast<const uint8_t*>(plaintext.c_str()), plaintext.size(), key, &outlen) != CRYPT_OK){
        std::cout << "hash failure: " << error_to_string(code) << "\n";
        return false;
    }

    return true;
}

std::string Rijndael::GetIV() {
    return std::string(reinterpret_cast<const char*>(iv), ivsize);
}

bool Rijndael::SetIV(const std::string& dataiv) {
    if(ivsize != dataiv.size()){
        std::cout << "import failure: invalid iv length\n";
        return false;
    }
    memcpy(iv, dataiv.c_str(), ivsize);
    return true;
}

bool Rijndael::NewIV() {
    unsigned long read = sprng_read(iv, ivsize, nullptr);
    if(!read){
        std::cout << "sprng failed\n";
        return false;
    }
    return true;
}

bool Rijndael::GenerateKey() {
    uint8_t nkey[512];
    unsigned long outlen;

    unsigned long read = sprng_read(nkey, sizeof(nkey), nullptr);
    if(!read){
        std::cout << "sprng failed\n";
        return false;
    }

    if(int code = hash_memory(hash_idx, nkey, read, key, &outlen) != CRYPT_OK){
        std::cout << "hash failure: " << error_to_string(code) << "\n";
        return false;
    }

    if(outlen < keysize){
        std::cout << "hash failure: failed to meet key length\n";
        return false;
    }

    if(outlen > keysize){
        std::cout << "warning: a truncated hash key was generated\n";
    }

    return true;
}

bool Rijndael::ImportKey(const std::string& keydata) {
    if(keydata.size() != keysize){
        std::cout << "import failure: invalid key length\n";
        return false;
    }
    memcpy(key, keydata.c_str(), keydata.size());
    return true;
}


std::string Rijndael::ExportKey() {
    std::string key;

    key.assign(key, keysize);

    return key;
}

void Rijndael::ClearKey() {
    memset(key, 0, sizeof(key)); // zero-memory key
}

std::string Rijndael::Encrypt(const char* data, size_t length) {
    std::string output;

    symmetric_OFB state;
    ofb_start(aes_idx, iv, key, keysize, 0, &state);
    const uint8_t * pos = reinterpret_cast<const uint8_t*>(data),
                  * end = pos + length;
    do {
        uint8_t cache[MAXBLOCKSIZE]; // cache buffer
        unsigned long len = std::min(uint32_t(state.blocklen), uint32_t(end - pos));
        
        if(int code = ofb_encrypt(pos, cache, len, &state) != CRYPT_OK){
            std::cout << "encrypt failure: " << error_to_string(code) << "\n";
            return "";
        }
        output.append(reinterpret_cast<const char*>(cache), len);

        pos += len;
    } while(pos < end);

    ofb_done(&state);
    return output;
}

std::string Rijndael::Decrypt(const char* data, size_t length) {
    std::string output;

    symmetric_OFB state;
    ofb_start(aes_idx, iv, key, keysize, 0, &state);
    const uint8_t * pos = reinterpret_cast<const uint8_t*>(data),
                  * end = pos + length;
    do {
        uint8_t cache[MAXBLOCKSIZE]; // cache buffer
        unsigned long len = std::min(uint32_t(state.blocklen), uint32_t(end - pos));

        if(int code = ofb_decrypt(pos, cache, len, &state) != CRYPT_OK){
            std::cout << "encrypt failure: " << error_to_string(code) << "\n";
            return "";
        }
        output.append(reinterpret_cast<const char*>(cache), len);

        pos += len;
    } while(pos < end);

    ofb_done(&state);
    return output;
}


#endif