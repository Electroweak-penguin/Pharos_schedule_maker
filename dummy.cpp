#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

void createStudentFile(const std::string& filename, const std::string& name, int grade, int isMale, int oneononeflag, int genderflag,
                       const std::map<std::string, std::pair<int, std::pair<std::vector<std::string>, std::vector<int>>>>& subjectSlots, 
                       const std::map<std::string, std::vector<int>>& availableSlots) {
    std::ofstream file(filename);

    file << "Name: " << name << "\n";
    file << "Grade: " << grade << "\n";
    file << "isMale: " << isMale << "\n";
    file << "OneOnOneflag: " << oneononeflag << "\n";
    file << "genderflag: " << genderflag << "\n";

    file << "Subjects:\n";
    for (const auto& [subject, subjectData] : subjectSlots) {
        file << subject << ":: " << subjectData.first << "\n";
        const auto& tsubjects = subjectData.second.first;  // tsubjectのベクター
        const auto& tslot = subjectData.second.second;  // tslotのベクター

        if (tsubjects.empty() || tslot.empty()) {
            file << "\n";
        } else {
            for (size_t i = 0; i < tsubjects.size() && i < tslot.size(); ++i) {
                file << tsubjects[i] << " " << tslot[i] << "\n";
            }
            file << "\n";
        }
    }
    file << "\n";
    file << "Available Slots:\n";
    for (const auto& [day, times] : availableSlots) {
        file << day << " ";
        for (int time : times) {
            file << time << " ";
        }
        file << "\n";
    }
    file << "\n";
    file << "class Count:\n";
    for (const auto& [day, times] : availableSlots) {
        file << day << " ";
        for (int time : times) {
            file << time << " ";
        }
        file << "\n";
    }
    file.close();
}

void createTeacherFile(const std::string& filename, const std::string& name, int isMale,
                       const std::vector<std::string>& subjects, 
                       const std::map<std::string, std::vector<int>>& availableSlots) {
    std::ofstream file(filename);

    file << "Name: " << name << "\n";
    file << "isMale: " << isMale << "\n";
    file << "Subjects:\n";
    for (const std::string& subject : subjects) {
        file << subject << "\n";
    }
    file << "\n";
    file << "Available Slots:\n";
    for (const auto& [day, times] : availableSlots) {
        file << day << " ";
        for (int time : times) {
            file << time << " ";
        }
        file << "\n";
    }
    file << "\n";
    file << "class Count:\n";
    for (const auto& [day, times] : availableSlots) {
        file << day << " ";
        for (int time : times) {
            file << time << " ";
        }
        file << "\n";
    }

    file.close();
}

int main() {
    // 生徒データ
    createStudentFile("student/Alice.txt", "Alice", 3, 0, 0, 0,
                      {{"Math", {2, {{"Mr_Smith", "Ms_Johnson"}, {1, 1}}}}, {"English", {1, {{}, {}}}}}, 
                      {{"4/1", {1, 2}}, {"4/2", {0}}, {"4/3", {2, 3}}, {"4/4", {0}}});
    createStudentFile("student/Bob.txt", "Bob", 2, 1, 1, 0,
                      {{"Science", {1, {{}, {}}}}, {"Math", {2, {{}, {}}}}}, 
                      {{"4/1", {0}}, {"4/2", {1, 3}}, {"4/3", {0}}, {"4/4", {1, 2}}});
    createStudentFile("student/Charlie.txt", "Charlie", 6, 1, 0, 0,
                      {{"Science", {1, {{}, {}}}}}, 
                      {{"4/1", {0}}, {"4/2", {1}}, {"4/3", {2, 3}}, {"4/4", {1, 2}}});

    // 講師データ
    createTeacherFile("teacher/Mr_Smith.txt", "Mr_Smith", 1,
                      {"Math", "Science"}, 
                      {{"4/1", {1, 2}}, {"4/2", {1, 2, 3}}, {"4/3", {0}}, {"4/4", {0}}});
    createTeacherFile("teacher/Ms_Johnson.txt", "Ms_Johnson", 0,
                      {"English", "Math", "Science"}, 
                      {{"4/1", {0}}, {"4/2", {0}},{"4/3", {2, 3}}, {"4/4", {1, 2}}});

    return 0;
}
