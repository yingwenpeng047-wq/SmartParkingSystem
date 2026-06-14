#include "PictureOperator.h"

extern unordered_map<int, User*> userMap;

PictureOperator::PictureOperator()
{
    this->connection = DBConnection::getInstance();
    this->con = this->connection->getConnection();
}

PictureOperator::~PictureOperator()
{

}

bool PictureOperator::insertPicture(string picture_name,string picture_cpath,int fd)
{
    //插入出入场及抓拍图片sql语句
    sql::SQLString sqlstr = "INSERT INTO tbl_picture(user_id,picture_name,picture_cpath,picture_time,picture_spath) VALUES(?,?,?,?,?);";
    sql::PreparedStatement* pstmt;

    // 获取当前时间点
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);


    // 转换为本地时间
    struct tm* localTime = std::localtime(&now_c);

    // 输出格式化时间
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

    pstmt = con->prepareStatement(sqlstr);
 


    pstmt->setInt(1, userMap[fd]->getUserID());
    pstmt->setString(2, picture_name);
    pstmt->setString(3, picture_cpath);
    pstmt->setString(4, buffer);
    pstmt->setString(5, "./" + userMap[fd]->getUserAccount() + "/");

    int ret = pstmt->executeUpdate();

    this->con->commit();

    delete pstmt;

    return ret > 0;
}

int PictureOperator::queryPicture(string picture_name)
{
    sql::PreparedStatement* pstmt = this->con->prepareStatement("SELECT picture_id FROM tbl_picture WHERE picture_name = ?;");
    int picture_id = -1;    //默认没找到图片id
    pstmt->setString(1, picture_name);
    sql::ResultSet* rs = pstmt->executeQuery();


    if (rs->next())
    {
        picture_id = rs->getInt("picture_id");    //找到图片，返回id
    }

    delete pstmt;
    delete rs;

    return picture_id;
}
