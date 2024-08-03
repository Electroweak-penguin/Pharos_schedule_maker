#include "SchoolManagement.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

// Studentクラスのコンストラクタ
Student::Student(std::string name, int grade, std::map<std::string, int> subjectSlots, std::map<std::string, std::vector<int>> availableSlots)
    : name(name), grade(grade), subjectSlots(subjectSlots), availableSlots(availableSlots) {}

// Teacherクラスのコンストラクタ
Teacher::Teacher(std::string name, std::vector<std::string> subjects, std::map<std::string, std::vector<int>> availableSlots)
    : name(name), subjects(subjects), availableSlots(availableSlots) {}

// 授業割り当て関数の実装
void assignClasses(std::vector<Student>& students, std::vector<Teacher>& teachers) {
    std::ofstream outFile("schedule.txt");
    if (!outFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return;
    }

    for (auto& student : students) {
        for (const auto& [studentSubject, slots] : student.subjectSlots) {
            for (int slot = 0; slot < slots; slot++){
                bool flag =false;
                for (auto& teacher : teachers) {
                    for (const auto& teacherSubject : teacher.subjects) {
                        if (studentSubject != teacherSubject)continue;
                        for (auto& [studentDay, studentTimes] : student.availableSlots) {
                            for (auto& studentTime : studentTimes) {
                                if (studentSubject=="Science"){
                                    std::cout<< studentTime <<std::endl;
                                }
                                if (studentTime == 0)continue;
                                for (auto& [teacherDay, teacherTimes] : teacher.availableSlots) {
                                    for (auto& teacherTime : teacherTimes) {
                                        if (teacherTime == 0)continue;
                                        if (teacherDay == studentDay && studentTime == teacherTime){
                                            flag = true;
                                            outFile << "Student " << student.name << " (Grade " << student.grade << ") "
                                                    << "has been assigned to Teacher " << teacher.name << " for " << teacherSubject
                                                    << " on " << teacherDay << " at slot " << teacherTime << std::endl;
                                            auto tit = std::find(teacherTimes.begin(), teacherTimes.end(), teacherTime);
                                            if (tit != teacherTimes.end()) {
                                                teacherTimes.erase(tit);
                                            }
                                            auto sit = std::find(studentTimes.begin(), studentTimes.end(), studentTime);
                                            if (sit != studentTimes.end()) {
                                                studentTimes.erase(sit);
                                            }
                                            
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
                int grade;
                int english, math, science;
                std::map<std::string, int> subjectSlots;
                std::map<std::string, std::vector<int>> availableSlots;

                while (std::getline(file, line)) {
                    if (line.find("Name:") == 0) {
                        name = line.substr(6); // "Name: "をスキップ
                    } else if (line.find("Grade:") == 0) {
                        grade = std::stoi(line.substr(7)); // "Grade: "をスキップ
                    } else if (line.find("English:") == 0) {
                        english = std::stoi(line.substr(9));
                        subjectSlots["English"] = english;
                    } else if (line.find("Math:") == 0) {
                        math = std::stoi(line.substr(6));
                        subjectSlots["Math"] = math;
                    } else if (line.find("Science:") == 0) {
                        science = std::stoi(line.substr(9));
                        subjectSlots["Science"] = science;
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
                students.emplace_back(name, grade, subjectSlots, availableSlots);
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

                teachers.emplace_back(name, subjects, availableSlots);
                file.close();
            }
        }
    }

    return teachers;
}

int exe() {
    // ファイルからデータを読み込む
    std::vector<Student> students = loadStudentsFromFiles("student");
    std::vector<Teacher> teachers = loadTeachersFromFiles("teacher");

    // 授業の割り当て
    assignClasses(students, teachers);

    return 0;
}
