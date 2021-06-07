module;

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

export module file_system;

namespace fs = std::filesystem;

export struct File_System {

};

export struct File {

};


export void build_package_file(std::string_view input_folder_name, std::string_view output_package_name) {
    //uint64_t
    std::vector<std::string> input_files;
    for (auto& p : fs::recursive_directory_iterator(input_folder_name.data())) {
        if (p.is_regular_file()) {
            std::string path = p.path().string();
            input_files.push_back(path);
            std::cout << path << " " << fs::file_size(p) << " bytes\n";
        }
    }

    std::cout << "\nFound " << input_files.size() << " files.\n";
}
