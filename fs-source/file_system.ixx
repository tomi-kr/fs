module;


#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <Windows.h>


export module file_system;


namespace fs = std::filesystem;


void report(std::string log) {
    OutputDebugStringA(log.c_str());
    std::cout << log;
}


export struct File_System {

};


export struct File {
    std::fstream storage;

    ~File() {
        storage.close();
    }

    void close() {
        storage.close();
    }

    bool open_write(std::string_view file_name) {
        close();

        storage.open(file_name, std::ios::out | std::ios::binary);
        if (!storage.good()) {
            report("open_write(): storage is bad!\n");

            return false;
        }

        return true;
    }

    template <typename Pod_Type>
    void write_pod(const Pod_Type* ptr) {
        write_buffer(ptr, sizeof(Pod_Type));
    }

    void write_buffer(const void* buffer, std::size_t size) {
        storage.write((char*)buffer, size);
    }

    void write_buffer(const std::vector<char> &bytes) {
        write_buffer((char*)&bytes[0], bytes.size());
    }

    bool good() {
        return storage.good();
    }

    bool bad() {
        return storage.bad();
    }

    static std::vector<char> vector_file(std::string_view file_name)
    {
        std::vector<char> result;
        std::streampos file_size;
        std::ifstream file(file_name.data(), std::ios::binary);

        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        result.reserve(file_size);
        result.insert(result.begin(), std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        return result;
    }
};


struct File_Info {
    uint64_t start_offset;
    uint64_t size;
    std::string name;
};

export void build_package_file(std::string_view input_folder_name, std::string_view output_package_name) {
    std::vector<File_Info> input_files;
    File package;

    uint64_t start_offset = 0;
    uint64_t current_file_size = 0;    

    package.open_write(output_package_name);

    if (!package.good()) {
        return;
    }
    
    package.write_pod(&start_offset); // placeholder for the total payload size / header offset

    //
    // Skip the first 8 bytes as we use it to store the header offset, so the 1st file starts on the 9th byte in the package file.
    //
    start_offset += sizeof(start_offset);

    for (auto& p : fs::recursive_directory_iterator(input_folder_name.data())) {
        if (p.is_regular_file()) {
            std::string path = p.path().string();
            current_file_size = fs::file_size(p);
            
            if (current_file_size == 0) {
                report("  ** Ignored " + path + " because the file size is 0.\n");
                continue;
            }

            std::vector<char> data = File::vector_file(path.c_str());            
            package.write_buffer(data);

            // keep track of this for the header information
            input_files.push_back({.start_offset = start_offset, .size = current_file_size, .name = path});
            start_offset += current_file_size;

            report(path + " " + std::to_string(current_file_size) + " bytes\n");
        }
    }

    report("\nFound " + std::to_string(input_files.size()) + " files.\n");
}
