#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>

void createStudentFile(const std::string& filename, const std::string& name, int grade, 
                       const std::map<std::string, int>& subjectSlots, 
                       const std::map<std::string, std::vector<int>>& availableSlots) {
    std::ofstream file(filename);

    file << "Name: " << name << "\n";
    file << "Grade: " << grade << "\n";
    file << "Subjects:\n";
    for (const auto& [subject, slots] : subjectSlots) {
        file << subject << ": " << slots << "\n";
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

    file.close();
}

void createTeacherFile(const std::string& filename, const std::string& name, 
                       const std::vector<std::string>& subjects, 
                       const std::map<std::string, std::vector<int>>& availableSlots) {
    std::ofstream file(filename);

    file << "Name: " << name << "\n";
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

    file.close();
}

int dummy() {
    // 生徒データ
    createStudentFile("student/Alice.txt", "Alice", 3, 
                      {{"Math", 2}, {"English", 1}}, 
                      {{"4/1", {1, 2}}, {"4/2", {0}}, {"4/3", {2, 3}}, {"4/4", {0}}});
    createStudentFile("student/Bob.txt", "Bob", 2, 
                      {{"Science", 1}, {"Math", 2}}, 
                      {{"4/1", {0}}, {"4/2", {1, 3}}, {"4/3", {0}}, {"4/4", {1, 2}}});

    // 講師データ
    createTeacherFile("teacher/Mr_Smith.txt", "Mr. Smith", 
                      {"Math", "Science"}, 
                      {{"4/1", {1, 2}}, {"4/2", {1, 2, 3}}, {"4/3", {0}}, {"4/4", {0}}});
    createTeacherFile("teacher/Ms_Johnson.txt", "Ms. Johnson", 
                      {"English", "Math"}, 
                      {{"4/1", {0}}, {"4/2", {0}},{"4/3", {2, 3}}, {"4/4", {1, 2}}});

    return 0;
}
