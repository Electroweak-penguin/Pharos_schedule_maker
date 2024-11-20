#ifndef SCHOOLMANAGEMENT_H
#define SCHOOLMANAGEMENT_H

#include <string>
#include <vector>
#include <map>

// 生徒クラスの定義
class Student {
public:
    std::string name;
    int grade;
    int isMale;
    int oneononeflag;
    int genderflag;
    std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots; // 各科目のコマ数
    std::map<std::string, std::vector<int>> availableSlots; // 来塾可能な日付とコマ
    std::map<std::string, std::map<int, int>> classCount;

    Student(std::string name, int grade, int isMale, int oneononeflag,int genderflag, std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, int>> classCount);
    Student() {}
};

// 講師クラスの定義
class Teacher {
public:
    std::string name;
    int isMale;
    std::vector<std::string> subjects; // 担当科目
    std::map<std::string, std::vector<int>> availableSlots; // 来塾可能な日付とコマ
    std::map<std::string, std::map<int, int>> classCount;

    Teacher(std::string name, int isMale, std::vector<std::string> subjects, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, int>> classCount);
    Teacher() {}
};

// 授業割り当て関数
void assignClasses(std::vector<Student>& students, std::vector<Teacher>& teachers);

// テキストファイルからデータを読み込む関数
std::vector<Student> loadStudentsFromFiles(const std::string& directory);
std::vector<Teacher> loadTeachersFromFiles(const std::string& directory);

#endif // SCHOOLMANAGEMENT_H

// 担当Teacher優先並び替えの関数
void sortTeachers(std::vector<Teacher>& teachers, std::vector<std::string> strvec) {
    std::stable_partition(teachers.begin(), teachers.end(), 
        [&strvec](const Teacher& teacher) {
            return std::find(strvec.begin(), strvec.end(), teacher.name) != strvec.end();
        });
}

void ClassCountUpdate(bool isStudent, const std::string& name, const std::string& subject, const std::string& day, int slotValue, int countValue) {
    // ディレクトリ選択
    std::string directory = isStudent ? "student" : "teacher";
    std::string filePath = directory + "/" + name + ".txt";

    // ファイルを開く
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    // ファイル内容の読み取りと変更
    std::ostringstream updatedContent;
    std::string line;
    bool inAvailableSlots = false;
    bool inClassCount = false;
    int slotIndex = -1; // 更新対象スロットのインデックスを追跡

    while (std::getline(file, line)) {
        if (line == "Available Slots:") {
            inAvailableSlots = true;
            inClassCount = false;
            updatedContent << line << "\n";
            continue;
        } else if (line == "class Count:") {
            inAvailableSlots = false;
            inClassCount = true;
            updatedContent << line << "\n";
            continue;
        }

        if (inAvailableSlots) {
            std::istringstream stream(line);
            std::ostringstream updatedLine;
            std::string currentDay;
            stream >> currentDay;

            if (currentDay == day) {
                updatedLine << currentDay; // 日付を追加
                int currentSlot;
                int currentIndex = 0;

                while (stream >> currentSlot) {
                    if (currentSlot == slotValue) {
                        slotIndex = currentIndex; // 対応するスロット位置を記録
                    }
                    updatedLine << " " << currentSlot;
                    currentIndex++;
                }
                updatedContent << updatedLine.str() << "\n";
            } else {
                updatedContent << line << "\n";
            }
        } else if (inClassCount) {
            std::istringstream stream(line);
            std::ostringstream updatedLine;
            std::string currentDay;
            stream >> currentDay;

            if (currentDay == day && slotIndex != -1) {
                updatedLine << currentDay; // 日付を追加
                int currentCount;
                int currentIndex = 0;

                while (stream >> currentCount) {
                    if (currentIndex == slotIndex) {
                        currentCount = countValue; // 指定スロット値を更新
                    }
                    updatedLine << " " << currentCount;
                    currentIndex++;
                }
                updatedContent << updatedLine.str() << "\n";
            } else {
                updatedContent << line << "\n";
            }
        } else {
            updatedContent << line << "\n"; // その他の行はそのまま
        }
    }
    file.close();

    // ファイルを書き換える
    std::ofstream outFile(filePath, std::ios::trunc);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to write to file: " + filePath);
    }
    outFile << updatedContent.str();
    outFile.close();
}