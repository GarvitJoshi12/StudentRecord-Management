#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "student_records.txt"
#define NAME_LEN 60
#define DEPT_LEN 40
#define LINE_LEN 256

/* A singly linked list is used to store student records dynamically. */
typedef struct Student {
    int rollNo;
    char name[NAME_LEN];
    char department[DEPT_LEN];
    int semester;
    float cgpa;
    float attendance;
    struct Student *next;
} Student;

static Student *head = NULL;
static int nextRollNo = 1;

static void trimNewline(char *text) {
    text[strcspn(text, "\r\n")] = '\0';
}

/* Names and departments may contain letters and spaces only. */
static int hasOnlyAlphabetsAndSpaces(const char *text) {
    int hasAlphabet = 0;
    while (*text != '\0') {
        if (isalpha((unsigned char)*text)) {
            hasAlphabet = 1;
        } else if (!isspace((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return hasAlphabet;
}

static int readAlphabeticText(const char *prompt, char *value, size_t size) {
    printf("%s", prompt);
    if (fgets(value, (int)size, stdin) == NULL) {
        return 0;
    }
    trimNewline(value);
    if (!hasOnlyAlphabetsAndSpaces(value)) {
        printf("Use alphabets and spaces only. Numbers and symbols are not allowed.\n");
        return -1;
    }
    return 1;
}

static int readIntInRange(const char *prompt, int minimum, int maximum, int *value) {
    char line[LINE_LEN];
    char *end;
    long number;

    printf("%s", prompt);
    if (fgets(line, sizeof(line), stdin) == NULL) return 0;
    errno = 0;
    number = strtol(line, &end, 10);
    while (isspace((unsigned char)*end)) end++;
    if (errno || end == line || *end != '\0' || number < minimum || number > maximum) {
        printf("Enter a whole number from %d to %d.\n", minimum, maximum);
        return -1;
    }
    *value = (int)number;
    return 1;
}

static int readFloatInRange(const char *prompt, float minimum, float maximum, float *value) {
    char line[LINE_LEN];
    char *end;
    float number;

    printf("%s", prompt);
    if (fgets(line, sizeof(line), stdin) == NULL) return 0;
    errno = 0;
    number = strtof(line, &end);
    while (isspace((unsigned char)*end)) end++;
    if (errno || end == line || *end != '\0' || number < minimum || number > maximum) {
        printf("Enter a value from %.1f to %.1f.\n", minimum, maximum);
        return -1;
    }
    *value = number;
    return 1;
}

static Student *createStudent(void) {
    Student *student = (Student *)calloc(1, sizeof(*student));
    if (student == NULL) printf("Unable to allocate memory.\n");
    return student;
}

static void appendStudent(Student *student) {
    Student *current;
    if (head == NULL) {
        head = student;
        return;
    }
    for (current = head; current->next != NULL; current = current->next) { }
    current->next = student;
}

static Student *findByRoll(int rollNo) {
    Student *current;
    for (current = head; current != NULL; current = current->next) {
        if (current->rollNo == rollNo) return current;
    }
    return NULL;
}

static int countStudents(void) {
    int count = 0;
    Student *current;
    for (current = head; current != NULL; current = current->next) count++;
    return count;
}

static void saveRecords(void) {
    FILE *file = fopen(DATA_FILE, "w");
    Student *current;
    if (file == NULL) {
        perror("Could not save records");
        return;
    }
    fputs("rollNo|name|department|semester|cgpa|attendance\n", file);
    for (current = head; current != NULL; current = current->next) {
        fprintf(file, "%d|%s|%s|%d|%.2f|%.2f\n", current->rollNo,
                current->name, current->department, current->semester,
                current->cgpa, current->attendance);
    }
    fclose(file);
}

static void loadRecords(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[LINE_LEN];
    if (file == NULL) return; /* First run: no data file yet. */
    fgets(line, sizeof(line), file); /* Header */
    while (fgets(line, sizeof(line), file) != NULL) {
        Student *student = createStudent();
        char *token;
        if (student == NULL) break;
        trimNewline(line);
        token = strtok(line, "|");
        if (token == NULL) { free(student); continue; }
        student->rollNo = atoi(token);
        token = strtok(NULL, "|"); if (token == NULL) { free(student); continue; }
        strncpy(student->name, token, NAME_LEN - 1);
        token = strtok(NULL, "|"); if (token == NULL) { free(student); continue; }
        strncpy(student->department, token, DEPT_LEN - 1);
        token = strtok(NULL, "|"); if (token == NULL) { free(student); continue; }
        student->semester = atoi(token);
        token = strtok(NULL, "|"); if (token == NULL) { free(student); continue; }
        student->cgpa = (float)atof(token);
        token = strtok(NULL, "|"); if (token == NULL) { free(student); continue; }
        student->attendance = (float)atof(token);
        if (student->rollNo <= 0 || student->semester < 1 || student->semester > 8 ||
            student->cgpa < 0 || student->cgpa > 10 || student->attendance < 0 || student->attendance > 100) {
            free(student);
            continue;
        }
        if (student->rollNo >= nextRollNo) nextRollNo = student->rollNo + 1;
        appendStudent(student);
    }
    fclose(file);
}

static void printTableHeader(void) {
    printf("\n%-7s %-25s %-18s %-5s %-6s %-9s\n", "ROLL", "NAME", "DEPARTMENT", "SEM", "CGPA", "ATTENDANCE");
    puts("--------------------------------------------------------------------------------");
}

static void printStudent(const Student *student) {
    printf("%-7d %-25.25s %-18.18s %-5d %-6.2f %-8.2f%%\n", student->rollNo,
           student->name, student->department, student->semester,
           student->cgpa, student->attendance);
}

static void addStudent(void) {
    Student *student = createStudent();
    int result;
    if (student == NULL) return;
    puts("\n--- Add Student ---");
    while ((result = readAlphabeticText("Name (alphabets only): ", student->name, sizeof(student->name))) < 0) { }
    if (!result) { free(student); return; }
    while ((result = readAlphabeticText("Department (alphabets only): ", student->department, sizeof(student->department))) < 0) { }
    if (!result) { free(student); return; }
    while ((result = readIntInRange("Semester (1-8): ", 1, 8, &student->semester)) < 0) { }
    if (!result) { free(student); return; }
    while ((result = readFloatInRange("CGPA (0-10): ", 0, 10, &student->cgpa)) < 0) { }
    if (!result) { free(student); return; }
    while ((result = readFloatInRange("Attendance % (0-100): ", 0, 100, &student->attendance)) < 0) { }
    if (!result) { free(student); return; }
    student->rollNo = nextRollNo++;
    appendStudent(student);
    saveRecords();
    printf("Student added successfully. Assigned roll number: %d\n", student->rollNo);
}

static void displayStudents(void) {
    Student *current;
    if (head == NULL) { puts("\nNo student records are available."); return; }
    printTableHeader();
    for (current = head; current != NULL; current = current->next) printStudent(current);
    printf("\nTotal records: %d\n", countStudents());
}

static void searchStudent(void) {
    int rollNo, result;
    Student *student;
    while ((result = readIntInRange("\nEnter roll number: ", 1, INT_MAX, &rollNo)) < 0) { }
    if (!result) return;
    student = findByRoll(rollNo);
    if (student == NULL) { puts("Record not found."); return; }
    puts("\n--- Student Record ---");
    printf("Roll No.    : %d\nName        : %s\nDepartment  : %s\nSemester    : %d\nCGPA        : %.2f\nAttendance  : %.2f%%\n",
           student->rollNo, student->name, student->department, student->semester, student->cgpa, student->attendance);
}

static void updateStudent(void) {
    int rollNo, result;
    Student *student;
    while ((result = readIntInRange("\nEnter roll number to update: ", 1, INT_MAX, &rollNo)) < 0) { }
    if (!result) return;
    student = findByRoll(rollNo);
    if (student == NULL) { puts("Record not found."); return; }
    puts("Enter the complete replacement details.");
    while ((result = readAlphabeticText("Name (alphabets only): ", student->name, sizeof(student->name))) < 0) { }
    if (!result) return;
    while ((result = readAlphabeticText("Department (alphabets only): ", student->department, sizeof(student->department))) < 0) { }
    if (!result) return;
    while ((result = readIntInRange("Semester (1-8): ", 1, 8, &student->semester)) < 0) { }
    if (!result) return;
    while ((result = readFloatInRange("CGPA (0-10): ", 0, 10, &student->cgpa)) < 0) { }
    if (!result) return;
    while ((result = readFloatInRange("Attendance % (0-100): ", 0, 100, &student->attendance)) < 0) { }
    if (!result) return;
    saveRecords();
    puts("Record updated successfully.");
}

static void deleteStudent(void) {
    int rollNo, result;
    Student *current = head, *previous = NULL;
    while ((result = readIntInRange("\nEnter roll number to delete: ", 1, INT_MAX, &rollNo)) < 0) { }
    if (!result) return;
    while (current != NULL && current->rollNo != rollNo) { previous = current; current = current->next; }
    if (current == NULL) { puts("Record not found."); return; }
    if (previous == NULL) head = current->next; else previous->next = current->next;
    free(current);
    saveRecords();
    puts("Record deleted successfully.");
}

static int compareByName(const void *left, const void *right) {
    const Student *a = *(const Student **)left;
    const Student *b = *(const Student **)right;
    return strcmp(a->name, b->name);
}

static int compareByMerit(const void *left, const void *right) {
    const Student *a = *(const Student **)left;
    const Student *b = *(const Student **)right;
    if (a->cgpa < b->cgpa) return 1;
    if (a->cgpa > b->cgpa) return -1;
    return strcmp(a->name, b->name);
}

static Student **makeArray(int *size) {
    int index = 0;
    Student *current;
    Student **students;
    *size = countStudents();
    if (*size == 0) return NULL;
    students = malloc((size_t)*size * sizeof(*students));
    if (students == NULL) return NULL;
    for (current = head; current != NULL; current = current->next) students[index++] = current;
    return students;
}

static void meritList(void) {
    int size, i;
    Student **students = makeArray(&size);
    if (size == 0) { puts("\nNo student records are available."); return; }
    if (students == NULL) { puts("Memory allocation failed."); return; }
    qsort(students, (size_t)size, sizeof(*students), compareByMerit);
    puts("\n--- Merit List (CGPA: high to low) ---");
    printf("%-6s %-7s %-25s %-6s\n", "RANK", "ROLL", "NAME", "CGPA");
    puts("--------------------------------------------------");
    for (i = 0; i < size; i++) printf("%-6d %-7d %-25.25s %-6.2f\n", i + 1, students[i]->rollNo, students[i]->name, students[i]->cgpa);
    free(students);
}

static void eligibilityList(void) {
    float minimumCgpa, minimumAttendance;
    int result, found = 0;
    Student *current;
    while ((result = readFloatInRange("\nMinimum CGPA (0-10): ", 0, 10, &minimumCgpa)) < 0) { }
    if (!result) return;
    while ((result = readFloatInRange("Minimum attendance % (0-100): ", 0, 100, &minimumAttendance)) < 0) { }
    if (!result) return;
    printTableHeader();
    for (current = head; current != NULL; current = current->next) {
        if (current->cgpa >= minimumCgpa && current->attendance >= minimumAttendance) { printStudent(current); found = 1; }
    }
    if (!found) puts("No students meet both criteria.");
}

static void statistics(void) {
    Student *current;
    float totalCgpa = 0, totalAttendance = 0, highest = 0, lowest = 10;
    int count = 0, atRisk = 0;
    if (head == NULL) { puts("\nNo data available for statistics."); return; }
    for (current = head; current != NULL; current = current->next) {
        totalCgpa += current->cgpa; totalAttendance += current->attendance;
        if (current->cgpa > highest) highest = current->cgpa;
        if (current->cgpa < lowest) lowest = current->cgpa;
        if (current->attendance < 75) atRisk++;
        count++;
    }
    printf("\n--- Academic Statistics ---\nTotal Students             : %d\nAverage CGPA               : %.2f\nHighest / Lowest CGPA      : %.2f / %.2f\nAverage Attendance         : %.2f%%\nBelow 75%% Attendance       : %d\n", count, totalCgpa / count, highest, lowest, totalAttendance / count, atRisk);
}

static void sortAndDisplayByName(void) {
    int size, i;
    Student **students = makeArray(&size);
    if (size == 0) { puts("\nNo student records are available."); return; }
    if (students == NULL) { puts("Memory allocation failed."); return; }
    qsort(students, (size_t)size, sizeof(*students), compareByName);
    puts("\n--- Students Sorted by Name ---");
    printTableHeader();
    for (i = 0; i < size; i++) printStudent(students[i]);
    free(students);
}

static void freeMemory(void) {
    Student *current = head;
    while (current != NULL) { Student *next = current->next; free(current); current = next; }
    head = NULL;
}

int main(void) {
    int choice, result;
    loadRecords();
    puts("========================================================");
    puts("       STUDENT RECORD MANAGEMENT SYSTEM");
    puts("     BCA (AI & Data Science) | Data Structures PBL");
    puts("========================================================");
    do {
        puts("\n1. Add student\n2. Display all students\n3. Search by roll number\n4. Update student\n5. Delete student\n6. Merit list\n7. Eligibility list\n8. Academic statistics\n9. Sort and display by name\n0. Save and exit");
        do { result = readIntInRange("Choose an option: ", 0, 9, &choice); } while (result < 0);
        if (!result) break;
        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: meritList(); break;
            case 7: eligibilityList(); break;
            case 8: statistics(); break;
            case 9: sortAndDisplayByName(); break;
            case 0: break;
        }
    } while (choice != 0);
    saveRecords();
    freeMemory();
    puts("Records saved. Thank you for using the system.");
    return 0;
}
