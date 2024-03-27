#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <filesystem>

namespace fs = std::filesystem;

class FilenameMatcher {
private:
    std::map<std::string, int> filename_to_number;
    std::map<int, std::string> number_to_filename;

public:
    FilenameMatcher(const std::string& directory_path) {
        int index = 0;
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            std::string filename = entry.path().filename().string();
            filename_to_number[filename] = index;
            number_to_filename[index] = filename;
            index++;
        }
    }

    int filenameToNumber(const std::string& filename) {
        if (filename_to_number.find(filename) != filename_to_number.end()) {
            return filename_to_number[filename];
        } else {
            return -1; // Not found
        }
    }

    std::string numberToFilename(int number) {
       

      if (number_to_filename.find(number) != number_to_filename.end()) {
            return number_to_filename[number];
    } else {
            return ""; // Not found
        }
    }
};

class DependencyProvider {
private:
    std::vector<std::string> filenames;
    std::vector<std::unordered_set<int>> dependencies;

public:
    DependencyProvider(const std::string& directory_path, const FilenameMatcher& matcher) {
       int index = 0;
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            std::string filename = entry.path().filename().string();
            filename_to_number[filename] = index;
            dependencies.push_back({});
            index++;
        }

        for (const auto& entry : fs::directory_iterator(directory_path)) {
            std::string filename = entry.path().filename().string();
            std::ifstream file(entry.path());
            std::string line;
            while (std::getline(file, line)) {
                if (line.find("#include") != std::string::npos) {
                    std::istringstream iss(line);
                    std::string include_directive;
                    std::string include_file;
                    iss >> include_directive >> include_file;
                    include_file = include_file.substr(1, include_file.size() - 2); // Remove quotes from include file
                    int included_file_index = filename_to_number[include_file];
                    dependencies[filename_to_number[filename]].insert(included_file_index);
                }
            }
        } 
    }

    std::unordered_set<int> provideDependencies(int file_index) {
        if (file_index >= 0 && file_index < dependencies.size()) {
            return dependencies[file_index];
        } else {
            return {}; // Empty set if file_index is out of bounds
        }
    }
};

class OrderValidator {
private:
    const DependencyProvider& dependency_provider;

public:
    OrderValidator(const DependencyProvider& provider) : dependency_provider(provider) {}

    bool isOrderCorrect(const std::vector<int>& order) {
        std::unordered_set<int> processed_files;
        for (int file_index : order) {
            // Check if all dependencies of the current file have been processed
            for (int dependency : dependencies[file_index]) {
                if (processed_files.find(dependency) == processed_files.end()) {
                    return false; // Dependency not met
                }
            }
            processed_files.insert(file_index);
        }
        return true; // All dependencies met
    }
};

class OrderValidatingEngine {
public:
    void execute(const std::string& directory_path, const std::vector<std::unordered_set<int>>& dependencies, std::istream& input, std::ostream& output) {
        try {
            // Validate directory path
            if (!fs::is_directory(directory_path)) {
                throw std::invalid_argument("Invalid directory path.");
            }

            // Create FilenameMatcher and DependencyProvider instances
            FilenameMatcher filename_matcher(directory_path);
            DependencyProvider dependency_provider(dependencies);

            // Read order of files from input and validate
            std::vector<int> order;
            int file_number;
            while (input >> file_number) {
                if (file_number < 0 || file_number >= dependencies.size()) {
                    throw std::invalid_argument("Invalid file number.");
                }
                order.push_back(file_number);
            }

            // Validate the order
            OrderValidator validator(dependency_provider);
            bool is_order_correct = validator.isOrderCorrect(order);

            // Output the result
            if (is_order_correct) {
                output << "File processing order is correct." << std::endl;
            } else {
                output << "Incorrect file processing order. Files with unmet dependencies: ";
                for (int file_index : order) {
                    if (!validator.isOrderCorrect({file_index})) {
                        output << filename_matcher.numberToFilename(file_index) << " ";
                    }
                }
                output << std::endl;
            }
        } catch (const std::exception& e) {
            // Handle exceptions
            output << "Error: " << e.what() << std::endl;
        }
    }
};

int main() {
    OrderValidatingEngine order_validating_engine;
    order_validating_engine.execute(std::cin, std::cout);
    return 0;
}
 
