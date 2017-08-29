#include"sql_api.h"

int main()
{
	SqlApi sql("127.0.0.1",3306);
	if(sql.connect())
		sql.select();
	else
		cout<<"connect error!"<<endl;
	return 0;
}
