#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <openssl/sha.h>
#include <mpi.h>

using namespace std;

int proc_num, proc_rank;

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
    double start, finish;

    arr = new char[size * 64 + 1];
    arr[size * 64 + 1] = '\0';
    for (int i = 0; i < size; ++i)
    {
        fin >> str;
        strncpy(arr + i * 64, str.c_str(), 64);
    }
    start = MPI_Wtime();
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);

    int temp_size = size * 64 / proc_num;
    char *small_arr = new char[temp_size];

    MPI_Scatter(arr, temp_size, MPI_CHAR, small_arr, temp_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    while (temp_size != 8 * proc_num)
    {
        char *temp = new char[temp_size / 2];
        for (int i = 0; i < temp_size / 128; ++i)
        {
            string str = string(small_arr + i * 128, small_arr + (i + 1) * 128);
            strncpy(temp + i * 64, sha256(str).c_str(), 64);
        }
        delete[] small_arr;
        small_arr = temp;
        temp_size /= 2;
    }
    
    char *res;

    res = new char[64 * proc_num];

    MPI_Allgather(small_arr, 64, MPI_CHAR, res, 64, MPI_CHAR, MPI_COMM_WORLD);
    
    if (proc_rank == 0)
    {
        string str = string(res, res + 64 * proc_num);
        cout << sha256(str) << endl;
        finish = MPI_Wtime();
        cout << "Total time: " << (finish - start) * 1000 << " milliseconds" << endl;
    }
    MPI_Finalize();
    return 0;
}
