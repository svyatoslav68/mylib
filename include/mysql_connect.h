#ifndef MYSQL_CONNECT_H
#define MYSQL_CONNECT_H

#include <mysql.h>

#define USER_BD "slava"
#define PASSW_BD "677183"
#define HOST_BD "localhost"

void bd_connect(MYSQL *bd_descriptor, const char *name_bd);
void disconnect(char *name);

#endif // MYSQL_CONNECT_H
