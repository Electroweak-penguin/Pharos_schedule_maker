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
    int oneononeflag;
    std::map<std::string, int> subjectSlots; // 各科目のコマ数
    std::map<std::string, std::vector<int>> availableSlots; // 来塾可能な日付とコマ

    Student(std::string name, int grade, int oneononeflag, std::map<std::string, int> subjectSlots, std::map<std::string, std::vector<int>> availableSlots);
    Student() {}
};

// 講師クラスの定義
class Teacher {
public:
    std::string name;
    std::vector<std::string> subjects; // 担当科目
    std::map<std::string, std::vector<int>> availableSlots; // 来塾可能な日付とコマ
    std::map<std::string, std::map<int, std::map<int, int>>> classCount;

    Teacher(std::string name, std::vector<std::string> subjects, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, std::map<int, int>>> classCount);
    Teacher() {}
};

// 授業割り当て関数
void assignClasses(std::vector<Student>& students, std::vector<Teacher>& teachers);

// テキストファイルからデータを読み込む関数
std::vector<Student> loadStudentsFromFiles(const std::string& directory);
std::vector<Teacher> loadTeachersFromFiles(const std::string& directory);

#endif // SCHOOLMANAGEMENT_H
