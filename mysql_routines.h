#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

struct collect_score_result {
  int number_of_rows;
  struct user_score *user_scores;
};

struct user_score {
  char user_name[20];
  int score;
};

struct game_display_result {
  int number_of_games;
  struct game_info *game_infos;
};

struct game_info {
  int game_id;
  char creatorUserName[10];
};

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

struct collect_score_result collect_score(int game_id) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(
      sqlQuery,
      "SELECT UserName, COUNT(Key_id) FROM SleepOne JOIN User ON "
      "User.User_id=SleepOne.User_id WHERE Game_id = %d GROUP BY User.User_id",
      game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  int row_num = mysql_num_rows(query_result);
  MYSQL_ROW row;
  struct user_score *items =
      (struct user_score *)malloc(sizeof(struct user_score) * row_num);

  for (int i = 0; i < row_num; i++) {
    row = mysql_fetch_row(query_result);
    strcpy(items[i].user_name, row[0]);
    items[i].score = atoi(row[1]);
  }

  struct collect_score_result result = {row_num, items};

  mysql_close(con);
  mysql_free_result(query_result);

  return result;
}

void game_change_status(int game_id, char status[20]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "UPDATE Game SET Status='%s' WHERE Game_id=%d", status,
          game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  mysql_close(con);
}

int game_create(int user_id, char password[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery,
          "INSERT INTO Game(Creator_id, Status, Password) VALUES(%d, 'queued', "
          "'%s');",
          user_id, password);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  int game_id = mysql_insert_id(con);

  sprintf(sqlQuery, "SELECT * FROM Game WHERE Game_id=%d;", game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(query_result);

  sprintf(sqlQuery, "INSERT INTO GameGroup VALUES(%d, %d);", atoi(row[1]),
          game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  mysql_close(con);
  mysql_free_result(query_result);

  return game_id;
}

struct game_display_result game_display(char status[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "123Aa123_",
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery,
          "SELECT Game_id, UserName FROM Game JOIN User ON "
          "Game.Creator_id=User.User_id WHERE Status='%s'",
          status);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  int row_num = mysql_num_rows(query_result);
  MYSQL_ROW row;
  struct game_info *items =
      (struct game_info *)malloc(sizeof(struct game_info) * row_num);

  for (int i = 0; i < row_num; i++) {
    row = mysql_fetch_row(query_result);
    items[i].game_id = atoi(row[0]);
    strcpy(items[i].creatorUserName, row[1]);
  }

  struct game_display_result result = {row_num, items};

  mysql_close(con);
  mysql_free_result(query_result);

  return result;
}

int get_sleep_count(int game_id, int key_id) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user",'123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery,
          "SELECT SUM(Sleep_count) FROM SleepOne WHERE Game_id = %d AND "
          "Key_id= %d GROUP BY Key_id",
          game_id, key_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(query_result);

  int result = atoi(row[0]);

  mysql_close(con);
  mysql_free_result(query_result);

  return result;
}

int *get_user_id_from_group(int game_id) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "SELECT User_id FROM GameGroup WHERE Game_id=%d;", game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  int row_num = mysql_num_rows(query_result);
  MYSQL_ROW row;

  int *result = (int *)malloc(row_num * sizeof(int));

  for (int i = 0; i < row_num; i++) {
    row = mysql_fetch_row(query_result);
    result[i] = atoi(row[0]);
  }

  mysql_close(con);
  mysql_free_result(query_result);

  return result;
}

void increase_score(int id, int increment) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "UPDATE User SET Score = Score + %d WHERE User_id = %d;",
          id, increment);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  mysql_close(con);
}

bool increment_sleep_count(int game_id, int user_id, int key_id) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user",'123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery,
          "UPDATE SleepOne SET Sleep_count = Sleep_count+1 WHERE User_id=%d "
          "AND Game_id=%d AND Key_id=%d",
          user_id, game_id, key_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
    return false;
  }

  mysql_close(con);

  return true;
}

bool join_game(int game_id, int user_id, char password[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "SELECT * FROM Game WHERE Game_id=%d AND Password='%s';",
          game_id, password);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(query_result);

  if (row == NULL) {
    return false;
  }

  sprintf(sqlQuery, "INSERT INTO GameGroup VALUES(%d, %d);", user_id, game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  mysql_close(con);
  mysql_free_result(query_result);

  return true;
}

// Return true if entered game password was correct and user was joined to the
// group Otherwise false
bool password_check_game(int game_id, int user_id, char password[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "SELECT * FROM Game WHERE Game_id=%d AND Password='%s';",
          game_id, password);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);

  if (mysql_fetch_row(query_result) == NULL) {
    return false;
  }

  sprintf(sqlQuery, "INSERT INTO GameGroup VALUES(%d, %d);", user_id, game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  mysql_close(con);
  mysql_free_result(query_result);

  return true;
}


char *password_check_nickname(char userName[50], char password[50]) {

  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "123Aa123_", "dontdisturb",
                         0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "SELECT * FROM User WHERE UserName='%s' AND Password='%s';",
          userName, password);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(query_result);

  if (row == NULL) {
    return "Wrong username or password";
  } else {
    return row[0];
  }
  mysql_close(con);
  mysql_free_result(query_result);
}

int get_user_id(char nickname[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "SELECT User_id From user where UserName='%s'", nickname);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  MYSQL_RES *query_result = mysql_store_result(con);
  MYSQL_ROW row = mysql_fetch_row(query_result);

  if (row == NULL) {
    return "Wrong username or password";
  } else {
    return row[0];
  }
  mysql_close(con);
  mysql_free_result(query_result);
}

int sleepone_create(int key_id, int user_id, int game_id) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery, "INSERT INTO SleepOne VALUES(%d, %d, %d);", key_id, user_id,
          game_id);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  int id = mysql_insert_id(con);

  mysql_close(con);

  return id;
}

int user_create(char userName[50], char password[50]) {
  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", '123Aa123_',
                         "dontdisturb", 0, NULL, 0) == NULL) {
    finish_with_error(con);
  }

  sprintf(sqlQuery,
          "INSERT INTO User(UserName, Password, Score) VALUES('%s', '%s', 0);",
          userName, password);
  if (mysql_query(con, sqlQuery)) {
    finish_with_error(con);
  }

  int id = mysql_insert_id(con);

  mysql_close(con);

  return id;
}
