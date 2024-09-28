#include "SchoolManagement.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
//  植田裕也です
//  腕野です
namespace fs = std::filesystem;

// Studentクラスのコンストラクタ
Student::Student(std::string name, int grade, int oneononeflag, std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots, std::map<std::string, std::vector<int>> availableSlots)
    : name(name), grade(grade), oneononeflag(oneononeflag), subjectSlots(subjectSlots), availableSlots(availableSlots) {}

// Teacherクラスのコンストラクタ
Teacher::Teacher(std::string name, std::vector<std::string> subjects, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, std::map<int, int>>> classCount)
    : name(name), subjects(subjects), availableSlots(availableSlots), classCount(classCount) {}

// Teacher並び替えの関数
void sortTeachers(std::vector<Teacher>& teachers, std::vector<std::string> strvec) {
    std::stable_partition(teachers.begin(), teachers.end(), 
        [&strvec](const Teacher& teacher) {
            return std::find(strvec.begin(), strvec.end(), teacher.name) != strvec.end();
        });
}

// 授業割り当て関数の実装
void assignClasses(std::vector<Student>& students, std::vector<Teacher>& teachers) {
    std::ofstream outFile("schedule.txt");
    if (!outFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return;
    }

    for (auto& student : students) {
        for (const auto& [studentSubject, subjectData] : student.subjectSlots) {
            auto& tsubslots = subjectData.second;
            auto& strvec = tsubslots.first;
            auto& numvec = tsubslots.second;
            sortTeachers(teachers, strvec);
            for (int slot = 0; slot < subjectData.first; slot++) {
                bool flag = false;
                int tempcount = 0;
                for (int i = 0; i < teachers.size(); ++i) {
                    auto& teacher = teachers[i];
                    if (numvec.size() > i && tempcount > numvec[i]){
                        tempcount = 0;
                        continue;
                    }
                    tempcount++;
                    for (const auto& teacherSubject : teacher.subjects) {
                        if (studentSubject != teacherSubject) continue;
                        for (auto& [studentDay, studentTimes] : student.availableSlots) {
                            for (auto& studentTime : studentTimes) {
                                if (studentTime == 0) continue;
                                for (auto& [teacherDay, teacherTimes] : teacher.availableSlots) {
                                    for (auto& teacherTime : teacherTimes) {
                                        if (teacherTime == 0) continue;
                                        if (teacherDay == studentDay && studentTime == teacherTime) {
                                            flag = true;
                                            outFile << "Student " << student.name << " (Grade " << student.grade << ") "
                                                    << "has been assigned to Teacher " << teacher.name << " for " << teacherSubject
                                                    << " on " << teacherDay << " at slot " << teacherTime << std::endl;

                                            if (teacher.classCount[teacherDay][teacherTime][student.oneononeflag] == 1 || (student.oneononeflag == 1)) {
                                                auto tit = std::find(teacherTimes.begin(), teacherTimes.end(), teacherTime);
                                                if (tit != teacherTimes.end()) {
                                                    teacherTimes.erase(tit);
                                                }
                                            }
                                            auto sit = std::find(studentTimes.begin(), studentTimes.end(), studentTime);
                                            if (sit != studentTimes.end()) {
                                                studentTimes.erase(sit);
                                            }

                                            teacher.classCount[teacherDay][teacherTime][student.oneononeflag]++;
                                        }
                                        if (flag) break;
                                    }
                                    if (flag) break;
                                }
                                if (flag) break;
                            }
                            if (flag) break;
                        }
                        if (flag) break;
                    }
                    if (flag) break;
                }
            }
        }
    }

    outFile.close();
}

// テキストファイルから生徒のデータを読み込む
std::vector<Student> loadStudentsFromFiles(const std::string& directory) {
    std::vector<Student> students;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::string name, line;
                int grade, oneononeflag;
                int english, math, science;
                int subslotnum;
                std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots;
                std::map<std::string, std::vector<int>> availableSlots;

                while (std::getline(file, line)) {
                    if (line.find("Name:") == 0) {
                        name = line.substr(6); // "Name: "をスキップ
                    } else if (line.find("Grade:") == 0) {
                        grade = std::stoi(line.substr(7)); // "Grade: "をスキップ
                    } else if (line.find("OneOnOneflag:") == 0) {
                        oneononeflag = std::stoi(line.substr(14));
                    } else if (line.find("Available Slots:") == 0) {
                        while (std::getline(file, line) && !line.empty()) {
                            std::istringstream availStream(line);
                            std::string date;
                            int time;
                            availStream >> date;
                            std::vector<int> times;
                            while (availStream >> time) {
                                times.push_back(time);
                            }
                            availableSlots[date] = times;
                        }
                    }
                    size_t colonPos = line.find("::");
                    if (colonPos != std::string::npos) {
                        std::string subject = line.substr(0, colonPos);  // 科目名を抽出
                        int count = std::stoi(line.substr(colonPos + 2));  // 数値を抽出
                        std::vector<std::string> strvec;
                        std::vector<int> numvec;
                        while (std::getline(file, line) && !line.empty()) {
                            std::istringstream iss(line);
                            std::string str;
                            int num;
                            iss >> str >> num;  // 空白で区切られた部分を分割
                            strvec.push_back(str);
                            numvec.push_back(num);
                        }
                        std::pair<std::vector<std::string>, std::vector<int>> tsubslots = {strvec, numvec};
                        subjectSlots[subject] = {count, tsubslots};  // 科目名に対応するデータを格納
                    }
                }
                students.emplace_back(name, grade, oneononeflag, subjectSlots, availableSlots);
                file.close();
            }
        }
    }

    return students;
}

// テキストファイルから講師のデータを読み込む
std::vector<Teacher> loadTeachersFromFiles(const std::string& directory) {
    std::vector<Teacher> teachers;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::string name, line;
                std::vector<std::string> subjects;
                std::map<std::string, std::vector<int>> availableSlots;
                std::map<std::string, std::map<int, std::map<int, int>>> classCount;

                while (std::getline(file, line)) {
                    if (line.find("Name: ") == 0) {
                        name = line.substr(6);
                    } else if (line.find("Subjects:") == 0) {
                        while (std::getline(file, line) && !line.empty()) {
                            subjects.push_back(line);
                        }
                    } else if (line.find("Available Slots:") == 0) {
                        while (std::getline(file, line) && !line.empty()) {
                            std::istringstream availStream(line);
                            std::string date;
                            int time;
                            availStream >> date;
                            std::vector<int> times;
                            while (availStream >> time) {
                                times.push_back(time);
                            }
                            availableSlots[date] = times;
                        }
                    }
                }

                teachers.emplace_back(name, subjects, availableSlots, classCount);
                file.close();
            }
        }
    }

    return teachers;
}

int main() {
    // ファイルからデータを読み込む
    std::vector<Student> students = loadStudentsFromFiles("student");
    std::vector<Teacher> teachers = loadTeachersFromFiles("teacher");

    // 授業の割り当て
    assignClasses(students, teachers);

    return 0;
}
