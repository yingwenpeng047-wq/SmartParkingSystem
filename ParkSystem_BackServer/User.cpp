#include "User.h"

User::User()
{
    this->user_id = 0;
    this->user_account = "";
    this->user_name = "";
    this->user_pwd = "e10adc3949ba59abbe56e057f20f883e";    //123456经过MD5加密后的数据
    this->user_status = 1;
}

User::User(int user_id,string user_account, string user_name, string user_pwd, int user_status)
{
    this->user_id = user_id;
    this->user_account = user_account;
    this->user_name = user_name;
    this->user_pwd = user_pwd;
    this->user_status = user_status;
}

User::~User()
{
}

int User::getUserID() const
{
    return this->user_id;
}

string User::getUserAccount() const
{
    return this->user_account;
}

string User::getUserName() const
{
    return this->user_name;
}

string User::getUserPwd() const
{
    return this->user_pwd;
}

int User::getUserStatus() const
{
    return this->user_status;
}

void User::setUserID(int user_id)
{
    this->user_id = user_id;
}

void User::setUserAccount(const string& user_account)
{
    this->user_account = user_account;
}

void User::setUserName(const string& user_name)
{
    this->user_name = user_name;
}

void User::setUserPwd(const string& user_pwd)
{
    this->user_pwd = user_pwd;
}

void User::setUserStatus(int user_status)
{
    this->user_status = user_status;
}
