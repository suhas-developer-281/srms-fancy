/*
 srms_fancy.c
 Fancy Console SRMS (Role-based) - ANSI colors + boxed menus
 Compile: gcc srms_fancy.c -o srms_fancy
 Run: ./srms_fancy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define STUDENT_FILE "students.txt"
#define CREDENTIAL_FILE "credentials.txt"
#define MAXLINE 256

/* ANSI color helpers */
#define COL_RESET "\x1b[0m"
#define COL_RED   "\x1b[31m"
#define COL_GREEN "\x1b[32m"
#define COL_YELLOW "\x1b[33m"
#define COL_CYAN  "\x1b[36m"
#define COL_BOLD  "\x1b[1m"

char currentRole[16] = {0};
char currentUser[64] = {0};
int currentUserRoll = 0;

/* prototypes */
int loginSystem(void);
void mainMenu(void);
void adminMenu(void);
void staffMenu(void);
void userMenu(void);
void guestMenu(void);
void showBanner(void);
void showBoxed(const char *title);
void pauseForUser(void);
void promptString(const char *prompt, char *buf, int n);
int promptInt(const char *prompt);
float promptFloat(const char *prompt);

/* student ops */
void displayAllStudents(void);
void displayStudentsBrief(void);
int findStudentByRoll(int roll, char *lineOut);
void addStudent(void);
void updateStudentMarks(void);
void updateStudentRecord(void);
void deleteStudent(void);

/* credentials */
void addCredential(void);
void showAllCredentials(void);

/* helpers */
void stripNewline(char *s);

int main(void){
    showBanner();
    if (loginSystem()) {
        mainMenu();
    } else {
        printf(COL_RED "Access Denied. Exiting...\n" COL_RESET);
    }
    return 0;
}

void showBanner(void) {
    printf(COL_CYAN COL_BOLD);
    printf("**********************************************\n");
    printf("*            S R M S - Fancy Demo            *\n");
    printf("*   Role Based Student Record Management     *\n");
    printf("**********************************************\n");
    printf(COL_RESET);
    printf("\n");
}

int loginSystem(void) {
    char username[64], password[64];
    char fileUser[64], filePass[64], fileRole[16];
    int fileRoll = 0;
    FILE *fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) {
        printf(COL_RED "Error: credentials.txt not found. Create it with an admin user.\n" COL_RESET);
        return 0;
    }
    promptString("Username: ", username, sizeof(username));
    promptString("Password: ", password, sizeof(password));
    while (fscanf(fp, "%63s %63s %15s %d", fileUser, filePass, fileRole, &fileRoll) == 4) {
        if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
            strcpy(currentRole, fileRole);
            strcpy(currentUser, fileUser);
            currentUserRoll = fileRoll;
            fclose(fp);
            printf(COL_GREEN "Login successful! Welcome %s (Role: %s)\n" COL_RESET, currentUser, currentRole);
            return 1;
        }
    }
    fclose(fp);
    printf(COL_RED "Invalid credentials.\n" COL_RESET);
    return 0;
}

void mainMenu(void) {
    if (strcmp(currentRole, "ADMIN") == 0) adminMenu();
    else if (strcmp(currentRole, "STAFF") == 0) staffMenu();
    else if (strcmp(currentRole, "USER") == 0) userMenu();
    else guestMenu();
}

void showBoxed(const char *title) {
    printf(COL_YELLOW "+------------------------------------------+\n");
    printf("| %-40s |\n", title);
    printf("+------------------------------------------+\n" COL_RESET);
}

void adminMenu(void) {
    int ch;
    do {
        showBoxed("ADMIN MENU");
        printf("1. Add student\t2. List students\t3. Update record\n");
        printf("4. Update marks\t5. Delete student\t6. Add credential\n");
        printf("7. Show credentials\t0. Logout\n");
        ch = promptInt("Select: ");
        switch(ch){
            case 1: addStudent(); break;
            case 2: displayAllStudents(); break;
            case 3: updateStudentRecord(); break;
            case 4: updateStudentMarks(); break;
            case 5: deleteStudent(); break;
            case 6: addCredential(); break;
            case 7: showAllCredentials(); break;
            case 0: printf(COL_CYAN "Logging out...\n" COL_RESET); break;
            default: printf(COL_RED "Invalid option\n" COL_RESET);
        }
    } while(ch != 0);
}

void staffMenu(void) {
    int ch;
    do {
        showBoxed("STAFF MENU");
        printf("1. Brief list\t2. Full list\t3. Update marks\t0. Logout\n");
        ch = promptInt("Select: ");
        switch(ch){
            case 1: displayStudentsBrief(); break;
            case 2: displayAllStudents(); break;
            case 3: updateStudentMarks(); break;
            case 0: printf(COL_CYAN "Logging out...\n" COL_RESET); break;
            default: printf(COL_RED "Invalid option\n" COL_RESET);
        }
    } while(ch != 0);
}

void userMenu(void) {
    int ch;
    do {
        showBoxed("USER MENU");
        printf("1. View my record\t0. Logout\n");
        ch = promptInt("Select: ");
        switch(ch){
            case 1: {
                char line[MAXLINE];
                if (currentUserRoll == 0) { printf(COL_YELLOW "No roll assigned to this user.\n" COL_RESET); }
                else if (findStudentByRoll(currentUserRoll, line)) {
                    printf(COL_GREEN "My record: %s\n" COL_RESET, line);
                } else { printf(COL_RED "My record not found.\n" COL_RESET); }
                pauseForUser();
                break;
            }
            case 0: printf(COL_CYAN "Logging out...\n" COL_RESET); break;
            default: printf(COL_RED "Invalid option\n" COL_RESET);
        }
    } while(ch != 0);
}

void guestMenu(void) {
    int ch;
    do {
        showBoxed("GUEST MENU");
        printf("1. Brief list\t0. Logout\n");
        ch = promptInt("Select: ");
        switch(ch){
            case 1: displayStudentsBrief(); break;
            case 0: printf(COL_CYAN "Logging out...\n" COL_RESET); break;
            default: printf(COL_RED "Invalid option\n" COL_RESET);
        }
    } while(ch != 0);
}

/* Student functions */

void addStudent(void) {
    int roll = promptInt("Roll: ");
    char buf[MAXLINE];
    if (findStudentByRoll(roll, buf)) { printf(COL_YELLOW "Already exists.\n" COL_RESET); return; }
    char name[100];
    promptString("Name: ", name, sizeof(name));
    float marks = promptFloat("Marks: ");
    FILE *fp = fopen(STUDENT_FILE, "a");
    if (!fp) { printf(COL_RED "Error opening students file.\n" COL_RESET); return; }
    fprintf(fp, "%d|%s|%.2f\n", roll, name, marks);
    fclose(fp);
    printf(COL_GREEN "Added successfully.\n" COL_RESET);
}

void displayAllStudents(void) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf(COL_YELLOW "No students file.\n" COL_RESET); return; }
    char buf[MAXLINE];
    printf(COL_BOLD "Roll\tName\t\tMarks\n" COL_RESET);
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            printf("%d\t%-20s\t%.2f\n", r, name, m);
        }
    }
    fclose(fp);
    pauseForUser();
}

void displayStudentsBrief(void) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) { printf(COL_YELLOW "No students file.\n" COL_RESET); return; }
    char buf[MAXLINE];
    printf(COL_BOLD "Roll\tName\n" COL_RESET);
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            printf("%d\t%-20s\n", r, name);
        }
    }
    fclose(fp);
    pauseForUser();
}

int findStudentByRoll(int roll, char *lineOut) {
    FILE *fp = fopen(STUDENT_FILE, "r");
    if (!fp) return 0;
    char buf[MAXLINE]; int found = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            if (r == roll) { if (lineOut) { stripNewline(buf); strcpy(lineOut, buf); } found = 1; break; }
        }
    }
    fclose(fp);
    return found;
}

void updateStudentMarks(void) {
    int roll = promptInt("Roll to update marks: ");
    FILE *fp = fopen(STUDENT_FILE, "r"); if (!fp) { printf(COL_YELLOW "No students file.\n" COL_RESET); return; }
    FILE *tmp = fopen("tmp_students.txt", "w"); if (!tmp) { fclose(fp); printf(COL_RED "Temp file error.\n" COL_RESET); return; }
    char buf[MAXLINE]; int updated = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            if (r == roll) { float nm = promptFloat("New marks: "); fprintf(tmp, "%d|%s|%.2f\n", r, name, nm); updated = 1; }
            else fprintf(tmp, "%d|%s|%.2f\n", r, name, m);
        }
    }
    fclose(fp); fclose(tmp);
    if (updated) { remove(STUDENT_FILE); rename("tmp_students.txt", STUDENT_FILE); printf(COL_GREEN "Marks updated.\n" COL_RESET); }
    else { remove("tmp_students.txt"); printf(COL_YELLOW "Roll not found.\n" COL_RESET); }
}

void updateStudentRecord(void) {
    int roll = promptInt("Roll to update: ");
    FILE *fp = fopen(STUDENT_FILE, "r"); if (!fp) { printf(COL_YELLOW "No students file.\n" COL_RESET); return; }
    FILE *tmp = fopen("tmp_students.txt", "w"); if (!tmp) { fclose(fp); printf(COL_RED "Temp file error.\n" COL_RESET); return; }
    char buf[MAXLINE]; int updated = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            if (r == roll) {
                char newName[128]; promptString("New name: ", newName, sizeof(newName));
                float nm = promptFloat("New marks: ");
                fprintf(tmp, "%d|%s|%.2f\n", r, newName, nm); updated = 1;
            } else fprintf(tmp, "%d|%s|%.2f\n", r, name, m);
        }
    }
    fclose(fp); fclose(tmp);
    if (updated) { remove(STUDENT_FILE); rename("tmp_students.txt", STUDENT_FILE); printf(COL_GREEN "Record updated.\n" COL_RESET); }
    else { remove("tmp_students.txt"); printf(COL_YELLOW "Roll not found.\n" COL_RESET); }
}

void deleteStudent(void) {
    int roll = promptInt("Roll to delete: ");
    FILE *fp = fopen(STUDENT_FILE, "r"); if (!fp) { printf(COL_YELLOW "No students file.\n" COL_RESET); return; }
    FILE *tmp = fopen("tmp_students.txt", "w"); if (!tmp) { fclose(fp); printf(COL_RED "Temp file error.\n" COL_RESET); return; }
    char buf[MAXLINE]; int found = 0;
    while (fgets(buf, sizeof(buf), fp)) {
        int r; char name[128]; float m;
        if (sscanf(buf, "%d|%127[^|]|%f", &r, name, &m) == 3) {
            if (r == roll) { found = 1; continue; }
            fprintf(tmp, "%d|%s|%.2f\n", r, name, m);
        }
    }
    fclose(fp); fclose(tmp);
    if (found) { remove(STUDENT_FILE); rename("tmp_students.txt", STUDENT_FILE); printf(COL_GREEN "Deleted.\n" COL_RESET); }
    else { remove("tmp_students.txt"); printf(COL_YELLOW "Roll not found.\n" COL_RESET); }
}

/* Credentials */
void addCredential(void) {
    char username[64], password[64], role[16]; int roll = 0;
    promptString("New username: ", username, sizeof(username));
    promptString("New password: ", password, sizeof(password));
    promptString("Role (ADMIN/STAFF/USER/GUEST): ", role, sizeof(role));
    for (int i=0; role[i]; ++i) role[i] = toupper((unsigned char)role[i]);
    if (strcmp(role, "USER") == 0) roll = promptInt("Assign roll: ");
    FILE *fp = fopen(CREDENTIAL_FILE, "a"); if (!fp) { printf(COL_RED "Cannot open credentials.\n" COL_RESET); return; }
    fprintf(fp, "%s %s %s %d\n", username, password, role, roll);
    fclose(fp); printf(COL_GREEN "Credential added.\n" COL_RESET);
}
void showAllCredentials(void) {
    FILE *fp = fopen(CREDENTIAL_FILE, "r"); if (!fp) { printf(COL_YELLOW "No credentials file.\n" COL_RESET); return; }
    char u[64], p[64], r[16]; int rr;
    printf(COL_BOLD "User\tRole\tRoll\n" COL_RESET);
    while (fscanf(fp, "%63s %63s %15s %d", u, p, r, &rr) == 4) {
        printf("%s\t%s\t%d\n", u, r, rr);
    }
    fclose(fp);
    pauseForUser();
}

/* helpers */
void stripNewline(char *s){ char *p = strchr(s, '\n'); if (p) *p = 0; }
void pauseForUser(void){ printf(COL_CYAN "\nPress Enter to continue..." COL_RESET); getchar(); }
void promptString(const char *prompt, char *buf, int n) {
    printf("%s", prompt); if (!fgets(buf, n, stdin)) { buf[0]=0; return; } stripNewline(buf);
}
int promptInt(const char *prompt) {
    char b[64]; int x;
    while (1) { printf("%s", prompt); if (!fgets(b, sizeof(b), stdin)) return 0; if (sscanf(b,"%d",&x)==1) return x; printf(COL_YELLOW "Please enter an integer.\n" COL_RESET); }
}
float promptFloat(const char *prompt) {
    char b[64]; float f;
    while (1) { printf("%s", prompt); if (!fgets(b, sizeof(b), stdin)) return 0.0f; if (sscanf(b,"%f",&f)==1) return f; printf(COL_YELLOW "Please enter a number.\n" COL_RESET); }
}
