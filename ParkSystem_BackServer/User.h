#pragma once
#include <iostream>
using namespace std;

class User
{
public:
    //user_status 默认1表示账号可用，0表示账号不可用
    User();
    User(int user_id, string user_account, string user_name, string user_pwd, int user_status = 1); 
    ~User();

    // Getter 方法声明
    int getUserID() const;
    string getUserAccount() const;
    string getUserName() const;
    string getUserPwd() const;
    int getUserStatus() const;

    // Setter 方法声明
    void setUserID(int user_id);
    void setUserAccount(const string& user_account);
    void setUserName(const string& user_name);
    void setUserPwd(const string& user_pwd);
    void setUserStatus(int user_status);

private:
	int user_id;
	string user_account;
	string user_name;
	string user_pwd;
	int user_status;
};

