#include "UserOperator.h"

extern unordered_map<int, User*> userMap;

UserOperator::UserOperator()
{
	this->res = 0;
	this->connection = DBConnection::getInstance();
	this->con = this->connection->getConnection();
}

UserOperator::~UserOperator()
{

}

bool UserOperator::queryUser(string& user_account)
{
    //开启事务
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM tbl_user WHERE user_account = ?;");
    pstmt->setString(1, user_account);
    sql::ResultSet* rs = pstmt->executeQuery(); //执行查询语句
    this->con->commit();

    if (rs->next())
    {
        return true;
    }
    else
    {
        //未查询到用户
        return false;
    }
}

//用户注册
bool UserOperator::registerUser(string user_account,string user_name)
{
    bool ret = false;
    string user_pwd = "e10adc3949ba59abbe56e057f20f883e";   //MD5加密后的123456

    sql::PreparedStatement* pstmt2 = con->prepareStatement("INSERT INTO tbl_user(user_account,user_name,user_pwd) VALUES (?,?,?);");
    pstmt2->setString(1, user_account);
    pstmt2->setString(2, user_name);
    pstmt2->setString(3, user_pwd);
    //执行更新
    this->res = pstmt2->executeUpdate();
    this->con->commit();
    this->res > 0 ? ret = true : ret = false;
    //释放资源
    delete pstmt2;

    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM tbl_user WHERE user_account = ?;");
    pstmt->setString(1, user_account);

    sql::ResultSet* rs = pstmt->executeQuery();

    rs->next();
    int user_id = rs->getInt("user_id");
    
    LogOperator lo;
    lo.insertLog(user_id, "注册");

    return ret;
}

string UserOperator::loginUser(USER& user,int fd)
{
    //使用?占位符，防止SQL注入
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT * FROM tbl_user WHERE user_account = ? AND user_pwd = ?;");
    pstmt->setString(1, user.account);
    pstmt->setString(2, user.pwd);

    //执行查询
    sql::ResultSet* rs = pstmt->executeQuery();
    this->con->commit();

    string user_name;
    //如果有查询到结果，至少一行
    if (rs->next())
    {
        user_name = rs->getString("user_name");
        string user_pwd = rs->getString("user_pwd");
        string user_account = rs->getString("user_account");
        int user_id = rs->getInt("user_id");
        int user_status = rs->getInt("user_status");
        cout << "查询到结果 user_name = " << user_name << endl;

        User* loginUser = new User(user_id, user_account, user_name, user_pwd, user_status);
        //保存当前已登录的客户端用户id
        if (userMap.count(fd) > 0)
        {
            //删除原有用户
            delete userMap[fd];
            userMap.erase(fd);
            //保存新登录的用户
            userMap[fd] = loginUser;
        }
        else
        {
            userMap[fd] = loginUser;
            cout << "userid = " << user_id << endl;
            cout << "userMap[fd].userId = " << userMap[fd]->getUserID() << endl;
        }
    }
    else
    {
        user_name = "";
        cout << "未查找到结果" << endl;
    }
    return user_name;
}
