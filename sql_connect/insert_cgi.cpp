#include"sql_api.h"

int main()
{
	SqlApi mysql("127.0.0.1",3306);
	mysql.connect();
	mysql.insert("张三","男","11");
	return 0;
}
