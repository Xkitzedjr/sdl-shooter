#include "db.hpp"

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
    char *command2 = (char*)", '";
    char *command3 = (char*)"')";

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

#ifdef DEBUG
    printf("executing command: %s\n", command);
#endif

    rc = sqlite3_exec(db, command, NULL, NULL, &err);

    if (rc != SQLITE_OK) {
        printf("error: %s\n", err);
        exit(EXIT_FAILURE);
    }

    sqlite3_close_v2(db);
}

void readFromDB(std::map<int, std::string, std::greater<int>> &scores) {
    sqlite3 *db;
    char *err;
    char *getScores = (char*)"SELECT * FROM highscores;";

    int rc = sqlite3_open("highscore.db", &db);
    if (rc != SQLITE_OK) {
        printf("error %s\n", err);
        exit(EXIT_FAILURE);
    }

    rc = sqlite3_exec(db, getScores, callback, &scores, &err);
    if (rc != SQLITE_OK) {
        printf("error %s\n", err);
        exit(EXIT_FAILURE);
    }

    sqlite3_close(db);
}

static int callback(void *scores_voidptr, int argc, char **data, char **ColName) {
    int i;
    std::string name;
    int score;
    std::map<int, std::string, std::greater<int>> *scores = (std::map<int, std::string, std::greater<int>>*)(scores_voidptr);

    for (i = 0; i < argc; i += 2) {
        score = atoi(data[i] ? data[i] : "0");
        if (data[i+1])
          name = data[i+1];
        else
          name = "NONAME";
#ifdef DEBUG
        std::cout << "adding " << score << ":" << name << " to score map" << std::endl;
#endif
        scores->insert(std::pair<int, std::string>(score, name));
    }

    return 0;
}
