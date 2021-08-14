#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <map>
#include <string>
#include <iostream>

int callback(void *data, int argc, char **argv, char **ColName) {
    int i;
    std::string name;
    int score;

    std::map<int, std::string> *scoreList = (std::map<int, std::string>*)(data);

    for (i = 0; i < argc; i+=2) {
        printf("%s = %s\n", ColName[i], argv[i] ? argv[i] : "NO_DATA");
        score = atoi((argv[i] ? argv[i] : "0"));
        name = (argv[i+1] ? argv[i+1] : "NO_DATA");
        scoreList->insert(std::pair<int, std::string>(score, name));
    }

    //printf("\n");
    return 0;
}

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

    /*
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
*/

    std::map<int, std::string> scoreList;

    sqlite3_exec(db, "SELECT * FROM highscores", callback, &scoreList, &err);

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    for (auto it = scoreList.begin(); it != scoreList.end(); it++) {
        std::cout << it->first << " => " << it->second << std::endl;
    }
}
