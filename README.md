# SRMS - Fancy Console (C)

**Student Record Management System (SRMS)** — Role-based console application in C.

## Project contents
- `srms_fancy.c` : Source code (C)
- `students.txt` : Sample data (format: `roll|name|marks`)
- `credentials.txt` : Sample credentials (`username password ROLE roll`)
- `demo/` : Screenshots and a short session transcript

## Features
- Role-based login: `ADMIN`, `STAFF`, `USER`, `GUEST`
- ADMIN: Add / Update / Delete / List students; manage credentials
- STAFF: View students, update marks
- USER: View own record (based on roll stored in credentials)
- GUEST: Read-only brief list

## Compile
```bash
gcc srms_fancy.c -o srms_fancy
