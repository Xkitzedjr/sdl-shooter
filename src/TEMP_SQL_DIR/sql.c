#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

int main(int arc, char **argv) {
    char *err;
    sqlite3 *db;
    sqlite3_stmt *stmt;

    char name[10][10] = {
    "jim",
    "jon",
    "joe",
    "jane",
    "joul",
    "jannet",
    "jazz",
    "juilet",
    "jannet",
    "tom"
    };

    sqlite3_open("highscores.db", &db);

    int rc = sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS highscores(score INT, name varchar)",
                 NULL,
                 NULL,
                 &err
    );

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
       char command[100] = "insert into highscores VALUES (";
       char I[10];
       char *command2 = ", '";
       char *command3 = "')";

       sprintf(I, "%d", (i+1)*10);

       strcat(command, I);
       strcat(command, command2);
       strcat(command, name[i]);
       strcat(command, command3);

       printf("executing command: %s\n", command);

       rc = sqlite3_exec(db, command, NULL, NULL, &err);

       if (rc != SQLITE_OK) {
           printf("error: %s\n", err);
           exit(EXIT_FAILURE);
       }
    }

    sqlite3_exec(db, "SELECT * FROM highscores", NULL, NULL, &err);

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }
}
