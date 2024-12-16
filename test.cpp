#include <mysql/mysql.h>
#include <iostream>
using namespace std;

int main()
{

    cout << "mysql client version: " << mysql_get_client_info() << endl;
    printf("%s",mysql_get_client_info());
    return 0;
}
