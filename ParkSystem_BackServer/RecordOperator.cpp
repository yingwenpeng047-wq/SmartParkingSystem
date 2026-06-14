#include "RecordOperator.h"

extern unordered_map<int, int>  userMap;

RecordOperator::RecordOperator()
{
    this->connection = DBConnection::getInstance();
    this->con = this->connection->getConnection();
}

RecordOperator::~RecordOperator()
{

}

INFOBACK RecordOperator::queryRecord(string number, string in_time, string out_time,int page,int user_id)
{
    int pageSize = 10; // 每页显示的记录数
    int offset = (page - 1) * pageSize; // 计算偏移量
    sql::PreparedStatement* pstmt;
    sql::SQLString sqlstr;

    //入库时间为空
    if (in_time.empty())
        in_time = "2000-01-01 00:00:00";

    //车牌为空
    if (number.empty())
        number = "%";

    //出库时间为空
    if (out_time.empty())
    {
        out_time = "2070-12-31 23:59:59";
        pstmt = con->prepareStatement("SELECT * FROM view_car_info "
                 "WHERE number LIKE ? AND (intime BETWEEN ? AND  ?) AND user_id = ? "
                 "LIMIT ? OFFSET ?;");
        pstmt->setString(1, number);
        pstmt->setString(2, in_time);
        pstmt->setString(3, out_time);
        pstmt->setInt(4, user_id);
        pstmt->setInt(5, pageSize);
        pstmt->setInt(6, offset);
    }
    else    //出库时间非空
    {
        pstmt = con->prepareStatement("SELECT * FROM view_car_info "
                 "WHERE number LIKE ? AND (intime BETWEEN ? AND  ?) AND (outtime BETWEEN ? AND ?) AND user_id = ? "
                 "LIMIT ? OFFSET ?;");
        pstmt->setString(1, number);
        pstmt->setString(2, in_time);
        pstmt->setString(3, out_time);
        pstmt->setString(4, in_time);
        pstmt->setString(5, out_time);
        pstmt->setInt(6, user_id);
        pstmt->setInt(7, pageSize);
        pstmt->setInt(8, offset);
    }


    sql::ResultSet* rs = pstmt->executeQuery();
    this->con->commit();

    INFOBACK infoBack;
    bzero(&infoBack, sizeof(INFOBACK));
    int index = 0;
    infoBack.number = index;


    while (rs->next())
    {
        CARINFO carInfo;
        infoBack.picture_back[index].cost = rs->getInt("cost");
        string inPath = rs->getString("inpath");
        string outPath = rs->getString("outpath");
        string inTime = rs->getString("intime");
        string outTime = rs->getString("outtime");
        string number = rs->getString("number");
        strcpy(infoBack.picture_back[index].in_picture_path, inPath.c_str());
        strcpy(infoBack.picture_back[index].out_picture_path, outPath.c_str());
        strcpy(infoBack.picture_back[index].in_time, inTime.c_str());
        strcpy(infoBack.picture_back[index].out_time, outTime.c_str());
        strcpy(infoBack.picture_back[index].number, number.c_str());
        index++;
    }

    infoBack.number = index;

    for (int i = 0; i < infoBack.number; i++)
    {
        cout << "车牌信息:" << infoBack.picture_back[i].number << endl;
        cout << "入场时间:" << infoBack.picture_back[i].in_time << endl;
        cout << "出场时间:" << infoBack.picture_back[i].out_time<< endl;
        cout << "入场图片:" << infoBack.picture_back[i].in_picture_path << endl;
        cout << "出场图片:" << infoBack.picture_back[i].out_picture_path << endl;
        cout << "停车金额:" << infoBack.picture_back[i].cost << endl;
        cout << "=======================" << endl;
    }


    return infoBack;
}

bool RecordOperator::insertRecord(string car_plate, string car_intime, string car_inplace, int in_img_id)
{
    sql::PreparedStatement* pstmt = con->prepareStatement("INSERT INTO tbl_car_info(car_plate,car_intime,car_inplace,in_img_id) VALUES (?,?,?,?);");

    cout << "car_plate = " << car_plate << endl;
    cout << "car_intime = " << car_intime << endl;
    cout << "car_inplace = " << car_inplace << endl;
    cout << "in_img_id = " << in_img_id << endl;


    pstmt->setString(1, car_plate);
    pstmt->setString(2, car_intime);
    pstmt->setString(3, car_inplace);
    pstmt->setInt(4, in_img_id);

    int ret = pstmt->executeUpdate();
    this->con->commit();

    delete pstmt;

    return (ret > 0);
}

bool RecordOperator::updateRecord(string car_plate, string car_outtime, string car_outplace, int out_img_id,int car_cost)
{
    cout << "car_plate = " << car_plate << endl;

    int car_id = -1;
    sql::PreparedStatement* pstmt2 = con->prepareStatement("SELECT car_id FROM tbl_car_info WHERE car_plate = ? AND car_outtime IS NULL ORDER BY car_intime DESC LIMIT 1;");
    pstmt2->setString(1, car_plate);
    sql::ResultSet* rs = pstmt2->executeQuery();

    if (rs->next())
    {
        car_id = rs->getInt("car_id");
    }
    else
    {
        delete pstmt2;
        delete rs;
        return false;
    }

    sql::PreparedStatement* pstmt = con->prepareStatement("UPDATE tbl_car_info SET car_outtime = ?, car_outplace = ?, out_img_id = ?, car_cost = ? WHERE car_plate = ? AND car_outtime IS NULL AND car_id = ?;");
    pstmt->setString(1, car_outtime);
    pstmt->setString(2, car_outplace);
    pstmt->setInt(3, out_img_id);
    pstmt->setInt(4, car_cost);
    pstmt->setString(5, car_plate);
    pstmt->setInt(6, car_id);

    int ret = pstmt->executeUpdate();
    this->con->commit();
    delete pstmt;
    delete pstmt2;
    delete rs;

    return (ret > 0);
}

string RecordOperator::queryTime(string number)
{
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT car_intime FROM tbl_car_info WHERE car_plate = ? AND car_outtime IS NULL ORDER BY car_intime DESC LIMIT 1;");
    pstmt->setString(1, number);

    sql::ResultSet* rs = pstmt->executeQuery();
    con->commit();

    if (rs->next())
    {
        return rs->getString("car_intime");
    }
    else
    {
        return "";
    }
}

TABLES RecordOperator::queryInRecord(int user_id)
{
    sql::PreparedStatement* pstmt = con->prepareStatement("SELECT intime,inplace,number FROM view_car_info WHERE user_id = ? ORDER BY intime DESC LIMIT 4;");
    pstmt->setInt(1, user_id);

    sql::ResultSet* rs = pstmt->executeQuery();
    con->commit();
    TABLES table;
    int index = 0;

    while (rs->next())
    {
        UPDATATABLE upTable;

        strcpy(upTable.plate, rs->getString("number").c_str());
        strcpy(upTable.time, rs->getString("intime").c_str());
        strcpy(upTable.locate, rs->getString("inplace").c_str());
        table.table[index] = upTable;
        index++;
    }
    table.number = index;

    return table;
}

