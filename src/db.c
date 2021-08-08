#include "db.h"

void initDB(void) {
    char *err;
    sqlite3 *db;

    int rc = sqlite3_open("highscore.db", &db);
    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    rc = sqlite3_exec(db,
                          "CREATE TABLE IF NOT EXISTS highscores(score INT, name varchar)",
                          NULL,
                          NULL,
                          &err
    );

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    printf("Database Initialized\n");

    sqlite3_close_v2(db);
}

void addScoreToDB(int score, char *name) {
    sqlite3 *db;
    char *err;
    char command[4028] = "insert into highscores VALUES (";
    char score_str[MAX_SCORE_LEN];
    char *command2 = ", '";
    char *command3 = "')";

    int rc = sqlite3_open("highscore.db", &db);
    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    if (sizeof(score) <= MAX_SCORE_LEN)
        sprintf(score_str, "%d", score);
    else {
        printf("error: score %d too large ", score);
        exit(EXIT_FAILURE);
    }

    strcat(command, score_str);
    strcat(command, command2);
    strcat(command, name);
    strcat(command, command3);

    printf("executing command: %s\n", command);

    rc = sqlite3_exec(db, command, NULL, NULL, &err);

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    sqlite3_close_v2(db);
}
