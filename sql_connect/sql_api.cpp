#include"sql_api.h"
	
	
	
SqlApi::SqlApi(const string& host,
			   const int &   port,
		       const string& user,
		   	   const string& passwd,
		   	   const string& database)
		   {
		   	    _mysql= mysql_init(NULL);
				_res =NULL;
				_host=host;
				_user=user;
				_passwd=passwd;
				_database=database;
				_port=port;
		   }
SqlApi::~SqlApi()
{
	mysql_close(_mysql);
}
bool SqlApi::connect()
{
	if(mysql_real_connect(_mysql,_host.c_str(),_user.c_str(),_passwd.c_str(),_database.c_str(),
	                      _port,NULL,0)== NULL)
	 {
		return false;
	 }
	 else
	 {
		return true;
	 }
}

bool SqlApi::insert(const string& name,
				const string& sex,
				const  string & age)
{
	string sql = "INSERT INTO student (name,sex,age) VALUES ('";
	sql += name;
	sql += "','";
	sql += sex;
	sql += "','";
	sql += age;
	sql += "')";
	int ret= mysql_query(_mysql,sql.c_str());
	if(ret != 0 )
	{
		cout<<"insert error!"<<endl;
		return false;
	}
	cout<<"insert success!"<<endl;
	return true;
}
bool SqlApi::select()
{
	const  char  *  sql = "select * from student";
	if(mysql_query(_mysql,sql)== 0)
	{
		_res = mysql_store_result(_mysql);
		if(_res)
		{
			int rows = mysql_num_rows(_res);
			int cols = mysql_num_fields(_res);
			cout<<"rows = "<<rows<<"cols = "<<cols<<endl;
			MYSQL_FIELD* fd ;
			for(;fd = mysql_fetch_field(_res);)
			{
				cout<<fd->name<<" "; 
			}
			cout<<endl;
			for(int  i = 0 ;i < rows;++i)
			{
				MYSQL_ROW  row_res=mysql_fetch_row(_res);
				int  j =0 ;
				for(;j < cols;++j)
				{
					cout<<row_res[j]<<" ";
				}
				cout<<endl;
			}
			return true;
		}
	}
	cout<<"select error!"<<endl;
	return  false;
}

