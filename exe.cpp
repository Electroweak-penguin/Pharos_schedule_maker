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
Student::Student(std::string name, int grade, int isMale, int oneononeflag, int genderflag, std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, int>> classCount)
    : name(name), grade(grade), isMale(isMale), oneononeflag(oneononeflag), genderflag(genderflag), subjectSlots(subjectSlots), availableSlots(availableSlots), classCount(classCount) {}

// Teacherクラスのコンストラクタ
Teacher::Teacher(std::string name, int isMale, std::vector<std::string> subjects, std::map<std::string, std::vector<int>> availableSlots, std::map<std::string, std::map<int, int>> classCount)
    : name(name), isMale(isMale), subjects(subjects), availableSlots(availableSlots), classCount(classCount) {}

// 授業割り当て関数の実装
void assignClasses(std::vector<Student>& students, std::vector<Teacher>& teachers) {
    std::ofstream outFile("schedule.txt", std::ios::app); // ファイルを追記モードで開く
    if (!outFile.is_open()) {
        std::cerr << "Unable to open output file!" << std::endl;
        return;
    }

    for (auto& student : students) {//生徒でループ
        for (const auto& [studentSubject, subjectData] : student.subjectSlots) {//生徒の科目でループ
            auto& tsubslots = subjectData.second;//その科目の通塾可能日時
            auto& strvec = tsubslots.first;//その科目の担当教師
            auto& numvec = tsubslots.second;//その科目の通塾回数
            sortTeachers(teachers, strvec);//担当教師を先にするように並び替え
            for (int slot = 0; slot < subjectData.first; slot++) {//生徒のその科目の受講回数でループ
                bool flag = false;
                int tempcount = 0;
                for (int i = 0; i < teachers.size(); ++i) {//教師でループ
                    if (student.genderflag == 1 && student.isMale != teachers[i].isMale) continue;
                    auto& teacher = teachers[i];
                    if (numvec.size() > i && tempcount > numvec[i]){//担当講師の担当回数を超えたら次の講師に
                        tempcount = 0;
                        continue;
                    }
                    tempcount++;
                    for (const auto& teacherSubject : teacher.subjects) {
                        if (studentSubject != teacherSubject) continue;
                        std::vector<int>  dates = {2, 3, 4, 5, 6, 1, 0};
                        std::vector<int>  times = {2, 3, 4, 5, 1};
                        for (int date : dates) {//曜日ループ
                        for (int time : times) {//時間帯ループ
                        for (int studate = 0; studate < student.availableSlots.size(); studate++){//生徒の通塾可能日時でループ
                            if (date != studate % 7) continue;
                            auto it = student.availableSlots.begin();
                            std::advance(it, studate);
                            auto& studentDay = it->first;
                            auto& studentTimes = it->second;
                            for (auto& studentTime : studentTimes) {//生徒がいける日時
                                if (time !=  studentTime% 5 +1) continue;
                                if (studentTime == 0) continue;
                                if(student.classCount[studentDay][studentTime] > 1) continue;
                                for (auto& [teacherDay, teacherTimes] : teacher.availableSlots) { //教師の通塾可能日時でループ
                                    for (auto& teacherTime : teacherTimes) {//教師がいける日時
                                        if (teacherTime == 0) continue;
                                        if(teacher.classCount[teacherDay][teacherTime] > 2) continue;
                                        if (teacherDay == studentDay && studentTime == teacherTime) {
                                            flag = true;
                                            teacher.classCount[teacherDay][teacherTime]++;
                                            ClassCountUpdate(0, teacher.name, teacherSubject, teacherDay, teacherTime, teacher.classCount[teacherDay][teacherTime]);
                                            student.classCount[studentDay][studentTime]++;
                                            ClassCountUpdate(1, student.name, studentSubject, studentDay, studentTime, student.classCount[studentDay][studentTime]);
                                            if (student.oneononeflag == 1) teacher.classCount[teacherDay][teacherTime] = 4;//その日時にすでに2人の授業(==3)があるか個別指導(==3)の場合
                                            outFile << "Student " << student.name << " (Grade " << student.grade << ") "
                                                    << "has been assigned to Teacher " << teacher.name << " for " << teacherSubject
                                                    << " on " << teacherDay << " at slot " << teacherTime << std::endl;
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
std::vector<Student> loadStudentsFromFiles(const std::string& directory, const std::vector<std::string>& targetStudents, bool loadAll) {
    std::vector<Student> students;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().stem().string(); // ファイル名（拡張子なし）

            // フラグがfalseの場合はtargetStudentsに含まれるかをチェック
            if (!loadAll && std::find(targetStudents.begin(), targetStudents.end(), filename) == targetStudents.end()) {
                continue;
            }

            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::string name, line;
                int grade, isMale, oneononeflag, genderflag;
                int english, math, science;
                int subslotnum;
                std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>> subjectSlots;
                std::map<std::string, std::vector<int>> availableSlots;
                std::map<std::string, std::map<int, int>> classCount;

                while (std::getline(file, line)) {
                    if (line.find("Name:") == 0) {
                        name = line.substr(6); // "Name: "をスキップ
                    } else if (line.find("Grade:") == 0) {
                        grade = std::stoi(line.substr(7)); // "Grade: "をスキップ
                    } else if (line.find("isMale: ") == 0) {
                        isMale = std::stoi(line.substr(8));
                    } else if (line.find("OneOnOneflag:") == 0) {
                        oneononeflag = std::stoi(line.substr(14));
                    } else if (line.find("genderflag:") == 0) {
                        genderflag = std::stoi(line.substr(12));
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
                    }  else if (line.find("class Count:") == 0) {
                        while (std::getline(file, line) && !line.empty()) {
                            std::istringstream countStream(line);
                            std::string date;
                            countStream >> date; // 日付を取得

                            // `Available Slots:`のスロットと対応するために取得
                            const auto& availableSlotsForDay = availableSlots[date];
                            int slotIndex = 0;

                            for (const auto& slot : availableSlotsForDay) {
                                int count;
                                countStream >> count; // `class Count:`の値を取得
                                classCount[date][slot] = count; // スロット値をキーにして格納
                            }
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
                students.emplace_back(name, grade, isMale, oneononeflag, genderflag, subjectSlots, availableSlots, classCount);
                file.close();
            }
        }
    }

    return students;
}

// テキストファイルから講師のデータを読み込む
std::vector<Teacher> loadTeachersFromFiles(const std::string& directory, const std::vector<std::string>& targetTeachers, bool loadAll) {
    std::vector<Teacher> teachers;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().stem().string(); // ファイル名（拡張子なし）

            // フラグがfalseの場合はtargetTeachersに含まれるかをチェック
            if (!loadAll && std::find(targetTeachers.begin(), targetTeachers.end(), filename) == targetTeachers.end()) {
                continue;
            }

            std::ifstream file(entry.path());
            if (file.is_open()) {
                std::string name, line;
                int isMale;
                std::vector<std::string> subjects;
                std::map<std::string, std::vector<int>> availableSlots;
                std::map<std::string, std::map<int, int>> classCount;

                while (std::getline(file, line)) {
                    if (line.find("Name: ") == 0) {
                        name = line.substr(6);
                    } else if (line.find("isMale: ") == 0) {
                        isMale = std::stoi(line.substr(8));
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
                    }  else if (line.find("class Count:") == 0) {
                        while (std::getline(file, line) && !line.empty()) {
                            std::istringstream countStream(line);
                            std::string date;
                            countStream >> date; // 日付を取得

                            // `Available Slots:`のスロットと対応するために取得
                            const auto& availableSlotsForDay = availableSlots[date];
                            int slotIndex = 0;

                            for (const auto& slot : availableSlotsForDay) {
                                int count;
                                countStream >> count; // `class Count:`の値を取得
                                classCount[date][slot] = count; // スロット値をキーにして格納
                            }
                        }
                    }

                }
                teachers.emplace_back(name, isMale, subjects, availableSlots, classCount);
                file.close();
            }
        }
    }

    return teachers;
}

// 実行関数
int main() {
    // 全てのデータを読み込むフラグ
    bool loadAll = true; // 全てのデータを読み込む場合はtrueに設定

    // config.txtに基づいてデータを読み込む
    std::vector<std::string> studentNames;
    std::vector<std::string> teacherNames;

    if (!loadAll) {
        loadConfig("config.txt", studentNames, teacherNames);
    }

    std::vector<Student> students = loadStudentsFromFiles("student", studentNames, loadAll);
    std::vector<Teacher> teachers = loadTeachersFromFiles("teacher", teacherNames, loadAll);

    // 授業の割り当て
    assignClasses(students, teachers);

    return 0;
}
