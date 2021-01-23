#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>

// 廖庆富𤋮熙darren老师    𤋮熙两个字有区别

#define DROP_SAMPLE_TABLE "DROP TABLE IF EXISTS utf8_tbl"        // if EXISTS 好处 是如果表不存在,执行不会报错
#define CREATE_SAMPLE_TABLE "CREATE TABLE utf8_tbl ( \
                                utf8_id INT UNSIGNED AUTO_INCREMENT, \
                                utf8_title VARCHAR(100) NOT NULL,    \
                                PRIMARY KEY (utf8_id)            \
                            )ENGINE=InnoDB DEFAULT CHARSET=utf8" // 要有CHARSET=utf8
#define INSERT_SAMPLE_TABLE "INSERT INTO utf8_tbl ( utf8_title ) VALUES ('廖庆富熙darren老师')"
#define SELECT_SAMPLE_TABLE "SELECT * FROM utf8_tbl"
#define DBNAME "mysql"
#define CHARSET "utf8"

// 先创建对应的数据库： CREATE DATABASE unicode;
// gcc -o utf8_tbl utf8_tbl.c -lmysqlclient -I/usr/include/mysql -L/usr/lib64/mysql/
int main()
{
    MYSQL *mysql;
    MYSQL_RES *res_ptr;
    MYSQL_ROW sqlrow;
    int res, i, j;

    mysql = mysql_init(NULL);
    if (!mysql)
    {
        printf("mysql_init failed\n");
        exit(1);
    }

    // 连接数据库
    mysql = mysql_real_connect(mysql, "localhost", "root", "curtis", DBNAME, 0, NULL, 0);

    if (mysql)
    {
        printf("Connection success\n");
    }
    else
    {
        printf("Connection failed\n");
    }

    // 处理字符问题的函数
    if (mysql_set_character_set(mysql, CHARSET) != 0)
    {
        printf(" mysql_set_character_set error\n");
        mysql_close(mysql);
        exit(1);
    }

    if (mysql_query(mysql, DROP_SAMPLE_TABLE)) //删除表
    {
        fprintf(stderr, " DROP TABLE failed\n");
        fprintf(stderr, " %s\n", mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    if (mysql_query(mysql, CREATE_SAMPLE_TABLE)) // 创建表
    {
        fprintf(stderr, " CREATE TABLE failed\n");
        fprintf(stderr, " %s\n", mysql_error(mysql));
        mysql_close(mysql);
        exit(1);
    }

    if (mysql)
    {
        res = mysql_query(mysql, INSERT_SAMPLE_TABLE);
        if (!res)
        {
            //输出受影响的行数
            printf("Inserted %lu rows\n", (unsigned long)mysql_affected_rows(mysql));
        }
        else
        {
            //打印出错误代码及详细信息
            fprintf(stderr, "Insert error %d: %s\n", mysql_errno(mysql), mysql_error(mysql));
        }
    }
    else
    {
        printf("Connection failed\n");
        exit(1);
    }

    if (mysql)
    {
        res = mysql_query(mysql, SELECT_SAMPLE_TABLE); //查询语句
        if (res)
        {
            printf("SELECT error:%s\n", mysql_error(mysql));
        }
        else
        {
            res_ptr = mysql_store_result(mysql); //取出结果集
            if (res_ptr)
            {
                printf("%lu Rows\n", (unsigned long)mysql_num_rows(res_ptr));
                j = mysql_num_fields(res_ptr);
                while ((sqlrow = mysql_fetch_row(res_ptr)))
                { //依次取出记录
                    for (i = 0; i < j; i++)
                        printf("%s\t", sqlrow[i]); //输出
                    printf("\n");
                }
                if (mysql_errno(mysql))
                {
                    fprintf(stderr, "Retrive error:%s\n", mysql_error(mysql));
                }
            }
            mysql_free_result(res_ptr);
        }
    }
    else
    {
        printf("Connection failed\n");
    }

    mysql_close(mysql);
    return 0;
}