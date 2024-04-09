#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <list>
#include <dirent.h>

class FilenameMatcher {
private:
    std::unordered_map<std::string, int> filenameToNumberMap;
    std::vector<std::string> numberToFilenameVector;

public:
    FilenameMatcher(const std::string& directoryPath) {
        int number = 0;
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(directoryPath.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                std::string filename = ent->d_name;
                if (filename.find(".h") != std::string::npos) {
                    filenameToNumberMap[filename] = number;
                    numberToFilenameVector.push_back(filename);
                    number++;
                }
            }
            closedir(dir);
        } else {
            // Не удалось открыть директорию
            perror("");
            throw std::runtime_error("Unable to open directory");
        }
    }

    int filenameToNumber(const std::string& filename) {
        auto it = filenameToNumberMap.find(filename);
        if (it != filenameToNumberMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Filename not found");
    }

    std::string numberToFilename(int number) {
        if (number >= 0 && number < numberToFilenameVector.size()) {
            return numberToFilenameVector[number];
        }
        throw std::runtime_error("Number out of range");
    }
};

class DependencyProvider {
private:
    FilenameMatcher& matcher;
    std::string directoryPath;

    std::set<int> parseDependencies(const std::string& filePath) {
        std::set<int> dependencies;
        std::ifstream file(filePath);
        std::string line;
        while (std::getline(file, line)) {
            if (line.starts_with("#include")) {
                std::string includedFile = extractFilename(line);
                dependencies.insert(matcher.filenameToNumber(includedFile));
            }
        }
        return dependencies;
    }

    std::string extractFilename(const std::string& includeLine) {
        std::istringstream iss(includeLine);
        std::string token;
        while (iss >> token) {
            if (token == "#include") {
                char delim;
                iss >> delim; // Считываем открывающую кавычку
                std::string filename;
                std::getline(iss, filename, '\"'); // Считываем имя файла
                return filename;
            }
        }
        return "";
    }

public:
    DependencyProvider(const std::string& path, FilenameMatcher& matcher)
        : directoryPath(path), matcher(matcher) {}

    std::unordered_map<int, std::set<int>> provideDependencies() {
        std::unordered_map<int, std::set<int>> dependencies;
        for (const auto& entry : filenameToNumberMap) {
            std::string filePath = directoryPath + "/" + entry.first;
            dependencies[entry.second] = parseDependencies(filePath);
        }
        return dependencies;
    }
};

class OrderProvider {
private:
    std::unordered_map<int, std::set<int>> dependencies;

    void topologicalSortUtil(int v, std::unordered_set<int> &visited, std::list<int> &order) {
        visited.insert(v);
        for (int dep : dependencies[v]) {
            if (visited.find(dep) == visited.end()) {
                topologicalSortUtil(dep, visited, order);
            }
        }
        order.push_front(v);
    }

public:
    OrderProvider(const std::unordered_map<int, std::set<int>>& deps) : dependencies(deps) {}

    std::vector<int> provideOrder() {
        std::list<int> order;
        std::unordered_set<int> visited;
        for (const auto& pair : dependencies) {
            if (visited.find(pair.first) == visited.end()) {
                topologicalSortUtil(pair.first, visited, order);
            }
        }
        return std::vector<int>(order.begin(), order.end());
    }
};

class OrderProvidingEngine {
public:
    void execute(std::istream& input, std::ostream& output) {
        std::string directoryPath;
        std::getline(input, directoryPath);

        FilenameMatcher matcher(directoryPath);
        DependencyProvider dependencyProvider(directoryPath, matcher);
        auto dependencies = dependencyProvider.provideDependencies();

        OrderProvider orderProvider(dependencies);
        auto order = orderProvider.provideOrder();

        for (int fileNumber : order) {
            output << matcher.numberToFilename(fileNumber) << std::endl;
        }
    }
};

int main() {
    OrderProvidingEngine engine;
    engine.execute(std::cin, std::cout);
    return 0;
}
