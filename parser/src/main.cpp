#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Helper function to split a string by delimiter
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_log_file.txt> <output_json_file.json>" << std::endl;
        return 1;
    }

    std::string inputFilePath = argv[1];
    std::string outputFilePath = argv[2];

    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open input file " << inputFilePath << std::endl;
        return 1;
    }

    json logEntries = json::array();
    std::string line;
    int lineNum = 0;

    while (std::getline(inputFile, line)) {
        lineNum++;
        std::vector<std::string> parts = split(line, ',');

        if (parts.size() < 4) { // Expect at least 4 parts
            std::cerr << "Warning: Skipping malformed line " << lineNum << ": " << line << std::endl;
            continue;
        }

        json entry;
        entry["timestamp_str"] = parts[0];
        entry["level"] = parts[1];
        entry["type"] = parts[2];

        // Combine remaining parts for description in case it contains commas
        std::string description = parts[3];
        for (size_t i = 4; i < parts.size(); ++i) {
            description += "," + parts[i];
        }
        entry["description"] = description;

        // Attempt to parse timestamp into Unix epoch for easier plotting
        std::tm tm = {};
        std::istringstream ss(parts[0]);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            std::cerr << "Warning: Could not parse timestamp on line " << lineNum << ": " << parts[0] << std::endl;
            entry["timestamp_epoch"] = nullptr; // Indicate parsing failed
        } else {
            entry["timestamp_epoch"] = std::mktime(&tm);
        }

        // Try to parse value if it's numeric (e.g., RPM, Speed, Voltage)
        try {
            if (entry["type"] == "Engine RPM" || entry["type"] == "Battery Voltage" || entry["type"] == "Vehicle Speed") {
                entry["value"] = std::stod(description);
            }
        } catch (const std::exception& e) {
            // Not a numeric value, or parsing failed, keep as string or leave out
            entry["value"] = nullptr; // Or simply don't add this field if not numeric
        }

        logEntries.push_back(entry);
    }

    inputFile.close();

    std::ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file " << outputFilePath << std::endl;
        return 1;
    }

    outputFile << std::setw(4) << logEntries << std::endl; // Pretty print JSON
    outputFile.close();

    std::cout << "Successfully parsed " << lineNum << " lines and saved to " << outputFilePath << std::endl;

    return 0;
}