#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <openssl/sha.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

int main(int argc, char **argv)
{
    char   *arr;
    int size = 1048576;
    string str;
    fstream fin("test");

    arr = new char[size * 64 + 1];
    arr[size * 64 + 1] = '\0';
    for (int i = 0; i < size; ++i)
    {
        fin >> str;
        strncpy(arr + i * 64, str.c_str(), 64);
    }
    auto start = high_resolution_clock::now();

    int temp_size = size * 64;

    while (temp_size != 64)
    {
        char *temp = new char[temp_size / 2];
        for (int i = 0; i < temp_size / 128; ++i)
        {
            str = string(arr + i * 128, arr + (i + 1) * 128);
            strncpy(temp + i * 64, sha256(str).c_str(), 64);
        }
        delete[] arr;
        arr = temp;
        temp_size /= 2;
    }

    str = string(arr, arr + 64 * 8);
    cout << sha256(str) << endl;
    auto finish = high_resolution_clock::now();
    cout << "Total time: " << duration_cast<milliseconds>(finish - start).count() << " milliseconds" << endl;

    return 0;
}
