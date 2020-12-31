#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mysql_connect.h"

void bd_connect(MYSQL *bd_descriptor, const char *name_bd){
	//MYSQL bd_descriptor;
	const char *host = HOST_BD;
	const char *user = USER_BD;
	const char *passwd = PASSW_BD;
	printf("name BD = %s\n", name_bd);
	if (!mysql_init(bd_descriptor)){
		printf("Ошибка при выполнении mysql_init()\n");
		exit(1);
	}
	if (!mysql_real_connect(bd_descriptor, host, user, passwd, name_bd,  0, NULL, 0)){
		printf("Соединенине не удалось\n");
		printf("%s\n", mysql_error(bd_descriptor));
		exit(1);
	}
	else{
		printf("Успешное соединенине с БД:%s\n", name_bd);
	}
	if (mysql_query(bd_descriptor, "SET NAMES utf8")){
		printf("Запрос не выполнен.");
		printf("Error %d:%s\n",mysql_errno(bd_descriptor),mysql_error(bd_descriptor));
	}
}

void disconnect(char *name){

}

