#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

// ファイルの`Available Slots`を基に`class Count`を初期化する
void initializeClassCount(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::ostringstream fileContent;
    std::string line;
    bool inAvailableSlots = false;
    bool inClassCount = false;
    std::vector<std::string> availableSlotsData;
    std::vector<std::string> classCountData;

    while (std::getline(inputFile, line)) {
        if (line == "Available Slots:") {
            inAvailableSlots = true;
            fileContent << line << "\n";
            continue;
        } else if (line == "class Count:") {
            inAvailableSlots = false;
            inClassCount = true;
            fileContent << line << "\n";
            continue;
        }

        if (inAvailableSlots) {
            availableSlotsData.push_back(line);
            fileContent << line << "\n";
        } else if (inClassCount) {
            if (!line.empty()) {
                classCountData.push_back(line);
            }
        } else {
            fileContent << line << "\n";
        }
    }
    inputFile.close();

    // `class Count`を更新
    std::ostringstream updatedClassCount;
    for (const auto& slotLine : availableSlotsData) {
        if (slotLine.empty()) {
            continue;
        }
        std::istringstream slotStream(slotLine);
        std::ostringstream countLine;
        std::string date;
        int slot;

        slotStream >> date;
        countLine << date;

        while (slotStream >> slot) {
            countLine << " " << (slot == 0 ? 0 : 1);
        }
        updatedClassCount << countLine.str() << "\n";
    }

    // ファイル内容を更新
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Failed to write to file: " + filePath);
    }

    outputFile << fileContent.str();
    outputFile << updatedClassCount.str(); // 新しいclass Countを書き込む
    outputFile.close();
}

// schedule.txtを初期化する関数
void initializeSchedule(const std::string& schedulePath, const std::vector<std::string>& studentNames, const std::vector<std::string>& teacherNames, bool initializeAll) {
    if (initializeAll) {
        // ファイルの内容を全て削除
        std::ofstream scheduleFile(schedulePath, std::ios::trunc);
        if (!scheduleFile.is_open()) {
            throw std::runtime_error("Failed to open schedule.txt for writing");
        }
        scheduleFile.close();
        std::cout << "schedule.txt has been cleared completely.\n";
    } else {
        // 特定の行を削除する
        std::ifstream scheduleFile(schedulePath);
        if (!scheduleFile.is_open()) {
            throw std::runtime_error("Failed to open schedule.txt for reading");
        }

        std::vector<std::string> updatedLines;
        std::string line;

        while (std::getline(scheduleFile, line)) {
            bool toDelete = false;

            // 学生名が含まれているかチェック
            for (const auto& student : studentNames) {
                if (line.find("Student " + student) != std::string::npos) {
                    toDelete = true;
                    break;
                }
            }

            // 教師名が含まれているかチェック
            for (const auto& teacher : teacherNames) {
                if (line.find("Teacher " + teacher) != std::string::npos) {
                    toDelete = true;
                    break;
                }
            }

            if (!toDelete) {
                updatedLines.push_back(line);
            }
        }
        scheduleFile.close();

        // schedule.txtを書き換え
        std::ofstream outFile(schedulePath, std::ios::trunc);
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open schedule.txt for writing");
        }

        for (const auto& updatedLine : updatedLines) {
            outFile << updatedLine << "\n";
        }
        outFile.close();
        std::cout << "schedule.txt has been updated based on config.txt.\n";
    }
}

// 初期化の処理にschedule.txt対応を追加
void initialize(const std::string& configPath, const std::string& schedulePath, bool initializeAll) {
    std::vector<std::string> studentNames;
    std::vector<std::string> teacherNames;

    // config.txtを読み込み
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open config.txt");
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (line == "Student:") {
            while (std::getline(configFile, line) && !line.empty()) {
                studentNames.push_back(line);
            }
        } else if (line == "Teacher:") {
            while (std::getline(configFile, line) && !line.empty()) {
                teacherNames.push_back(line);
            }
        }
    }
    configFile.close();

    // studentとteacherのファイルを初期化
    if (initializeAll) {
        for (const auto& file : fs::directory_iterator("student")) {
            if (file.is_regular_file()) {
                initializeClassCount(file.path().string());
            }
        }
        for (const auto& file : fs::directory_iterator("teacher")) {
            if (file.is_regular_file()) {
                initializeClassCount(file.path().string());
            }
        }
    } else {
        for (const auto& studentFile : studentNames) {
            initializeClassCount("student/" + studentFile + ".txt");
        }
        for (const auto& teacherFile : teacherNames) {
            initializeClassCount("teacher/" + teacherFile + ".txt");
        }
    }

    // schedule.txtの初期化
    initializeSchedule(schedulePath, studentNames, teacherNames, initializeAll);
}

// メイン関数
int main() {

    bool initializeAll = true; // 全てのファイルを初期化する場合はtrueにする
    const std::string configPath = "config.txt";
    const std::string schedulePath = "schedule.txt";

    initialize(configPath, schedulePath, initializeAll);

    std::cout << "Initialization completed successfully.\n";

    return 0;
}
