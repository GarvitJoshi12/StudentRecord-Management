# Student Record Management System

**Project-Based Learning (PBL) — Introduction to Data Structures**  
**Programme:** BCA (Artificial Intelligence & Data Science), Semester III

A menu-driven C application for managing student academic records. It demonstrates how a **singly linked list** can store records dynamically, while arrays and sorting algorithms support reports such as a merit list.

## Features

- Add, view, search, update and delete student records
- Stable, automatically assigned roll numbers
- Full names and department names with spaces
- Merit list sorted by CGPA
- Placement/scholarship eligibility filter by CGPA and attendance
- Academic statistics, including average CGPA and attendance risk count
- Name-sorted display
- Automatic data persistence in `student_records.txt`
- Input validation: name/department accept alphabets and spaces only; semester, CGPA and attendance have strict numeric ranges

## Data Structure Concepts Demonstrated

| Concept | Use in this project |
|---|---|
| Singly linked list | Dynamic storage of student nodes |
| Traversal | Display, search, statistics, saving records |
| Insertion | Adding a student at the end of the list |
| Deletion | Removing a node using previous/current pointers |
| Dynamic memory | `calloc`, `malloc`, and `free` |
| Array of pointers | Temporary reporting view of the linked list |
| Sorting | `qsort` for merit and alphabetical reports |
| File handling | Persistent storage using a delimiter-separated text file |

## How to Run

Compile with any standard C compiler (C11 or later recommended):

```powershell
gcc -std=c11 -Wall -Wextra -pedantic pbl.c -o student_records.exe
.\student_records.exe
```

The application creates `student_records.txt` automatically in the same folder. Do not edit that file while the application is running.

## Sample Demonstration Flow

1. Add three students with different CGPAs and attendance values.
2. Display all records and note the assigned roll numbers.
3. Search one record by its roll number.
4. Generate the merit list.
5. Use eligibility criteria such as CGPA `7.0` and attendance `75`.
6. Display statistics.
7. Exit and run again to demonstrate that records persist.

## Complexity Summary

| Operation | Time complexity |
|---|---:|
| Add student | O(n) |
| Search / update / delete by roll number | O(n) |
| Display / statistics | O(n) |
| Merit / name report | O(n log n) average after O(n) array creation |

## Suggested Viva Questions

1. Why is a linked list suitable when the number of students is unknown?
2. How does deletion differ for the first node and later nodes?
3. Why is a temporary array used for sorting reports instead of changing the linked list?
4. What is the difference between stack allocation and dynamic memory allocation?
5. Why must every allocated node be freed before program exit?
