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


export void report(std::string log) {
    OutputDebugStringA(log.c_str());
    std::cout << log;
}


export struct File_System {

};


export struct File {
    enum {
        Open_Mode_Write,
        Open_Mode_Read
    };

    FILE* storage = NULL;

    ~File() {
        close();
    }

    void close() {
        if (storage) {
            fflush(storage);
            fclose(storage);
            storage = NULL;
        }
    }

    bool open_write(std::string_view file_name) {
        close();
        
        errno_t error = fopen_s(&storage, file_name.data(), "wb");        
        if (error) {
            report("open_write(): error code is " + std::to_string(error) + "\n");

            return false;
        }

        return true;
    }

    template <typename Pod_Type>
    void write_pod(const Pod_Type &ptr) {
        write_buffer(&ptr, sizeof(Pod_Type));
    }

    void write_string(const std::string_view string) {
        write_buffer(string.data(), string.size());
    }

    void write_buffer(const void* buffer, std::size_t size) {
        fwrite(buffer, 1, size, storage);
    }

    void write_buffer(const std::vector<char> &bytes) {
        write_buffer(&bytes[0], bytes.size());
    }

    void rewind() {
        fflush(storage);
        fseek(storage, 0, SEEK_SET);
    }

    bool good() {
        return storage != NULL;
    }

    static std::vector<char> vector_file(std::string_view file_name) {
        std::vector<char> result;
        
        // std::ifstream is a lot slower for some reason
        FILE* file = NULL;
        uint64_t file_size = 0;

        fopen_s(&file, file_name.data(), "rb");
        if (!file) {
            return result;
        }

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        result.resize(file_size);
        
        fread(&result[0], sizeof(char), file_size, file);
        fclose(file);
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
    
    package.write_pod(uint64_t(0)); // placeholder for the total payload size / header offset

    //
    // Skip the first 8 bytes as we use it to store the header offset, so the 1st file starts on the 9th byte in the package file.
    //
    start_offset += sizeof(uint64_t);

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
            report("file: " + path + " start_offset: " + std::to_string(start_offset) + " current_file_size: " + std::to_string(current_file_size) + " \n");
            start_offset += current_file_size;            
        }
    }

    for (const File_Info& info : input_files) {
        package.write_pod(info.start_offset);
        package.write_pod(info.size);
        package.write_string(info.name);
        package.write_pod(char(0));
    }

    package.rewind();
    package.write_pod(start_offset);

    report("\nFound " + std::to_string(input_files.size()) + " files.\n");
}
