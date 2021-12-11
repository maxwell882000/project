#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

void finish_with_error(MYSQL *con) {
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

char *password_check_nickname(char userName[50], char password[50]) {

  char sqlQuery[1000];

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) {
    fprintf(stderr, "%s\n", mysql_error(con));
    exit(1);
  }

  if (mysql_real_connect(con, "localhost", "user", "123Aa123_", "dontdisturb",
                         0, NULL,
                         0) == NULL) {
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

int main() {
  char nickname[50] = "bekzod";
  char password[50] = "password";
  char *s = password_check_nickname(nickname, password);
  printf("%s", s);
  exit(0);
}