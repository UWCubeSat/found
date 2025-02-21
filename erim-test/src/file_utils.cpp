#include "file_utils.hpp"

bool hasValidExtension(const std::string& filename, const std::string& extension) {
    return filename.size() >= extension.size() &&
           filename.substr(filename.size() - extension.size()) == extension;
}
