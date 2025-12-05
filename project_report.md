# Project Report: Student Record Management System (SRMS)

## 1. Introduction
The Student Record Management System (SRMS) is a role-based console application written in C. 
It allows different users (ADMIN, STAFF, USER, GUEST) to access different menus and functionalities.

## 2. Objectives
- Implement role-based access control.
- Demonstrate file handling in C.
- Perform basic CRUD operations on student records.
- Maintain credentials in a structured format.

## 3. Files Used
- **srms_fancy.c** : Main C program source code.
- **students.txt** : Stores student records in the format `roll|name|marks`.
- **credentials.txt** : Stores login information `username password ROLE roll`.
- **demo/** : Contains screenshots and session transcript.

## 4. System Design (Flow Overview)
1. Program starts with login.
2. User credentials are read from `credentials.txt`.
3. Based on ROLE:
   - ADMIN → Full access (CRUD + credentials management)
   - STAFF → View + update marks
   - USER → View own record only
   - GUEST → Brief view only
4. Student data is stored using file handling (read/write).

## 5. Implementation
The program uses:
- File operations (`fopen`, `fprintf`, `fscanf`, `fgets`).
- Temporary files for safe updates.
- Role-based menus.
- String parsing to read records formatted as `roll|name|marks`.

## 6. Sample Credentials
admin admin123 ADMIN 0
staff1 sstaff STAFF 0
student1 stu123 USER 101
guest guest GUEST 0

shell
Copy code

## 7. Sample Student Record Format
101|Sunny Kumar|78.5

csharp
Copy code

## 8. Sample Output Screenshots
Screenshots can be found in the **demo/** folder:
- admin_menu.png
- student_view.png
- staff_action.png

## 9. Conclusion
This project demonstrates file handling, user roles, and record management in C. 
It is a lightweight but practical console application suitable for academic evaluation.