#include "LogOperator.h"

LogOperator::LogOperator()
{
    //数据块连接管理器
    this->connection = DBConnection::getInstance();
    //实际的数据库连接对象
    this->con = this->connection->getConnection();
}

LogOperator::~LogOperator()
{
}

bool LogOperator::insertLog(int user_id, string log_operator)
{
    // 获取当前时间点
    auto now = chrono::system_clock::now();
    time_t now_c = chrono::system_clock::to_time_t(now);

    // 转换为本地时间
    struct tm* localTime = std::localtime(&now_c);

    // 输出格式化时间
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

    sql::PreparedStatement* pstmt = con->prepareStatement("INSERT INTO tbl_log(user_id,log_operate,log_time) VALUE(?,?,?);");

    pstmt->setInt(1,user_id);
    pstmt->setString(2,log_operator);
    pstmt->setString(3,buffer);

    int res = pstmt->executeUpdate();
    this->con->commit();
    delete pstmt;

    return (res > 0);
}
