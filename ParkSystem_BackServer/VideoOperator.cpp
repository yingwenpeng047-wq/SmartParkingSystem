#include "VideoOperator.h"

VideoOperator::VideoOperator()
{
	this->connection = DBConnection::getInstance();
	this->con = this->connection->getConnection();
}

VideoOperator::~VideoOperator()
{

}

bool VideoOperator::updateVideo(VIDEO& videoInfo,int user_id)
{
	sql::PreparedStatement* pstmt = this->con->prepareStatement("UPDATE tbl_video SET video_current = ? WHERE video_name = ? AND user_id = ?;");

	pstmt->setInt(1, videoInfo.video_current);
	pstmt->setString(2, videoInfo.video_name);
	pstmt->setInt(3, user_id);

	int ret = pstmt->executeUpdate();
	this->con->commit();

	return (ret > 0);
}

bool VideoOperator::insertVideo(VIDEO& videoInfo, int user_id)
{
	sql::PreparedStatement* pstmt = this->con->prepareStatement("INSERT INTO tbl_video(video_frame,video_name,user_id,video_path,picture_path,video_time,video_current) VALUES(?,?,?,?,?,?,?);");

	time_t now = std::time(nullptr);

	char buffer[11];  // 格式: YYYY_MM_DD + 终止符
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&now));

	pstmt->setInt(1, videoInfo.video_frame);
	pstmt->setString(2, videoInfo.video_name);
	pstmt->setInt(3, user_id);
	pstmt->setString(4, videoInfo.video_path);
	pstmt->setString(5, videoInfo.picture_path);
	pstmt->setString(6, buffer);
	pstmt->setInt(7, videoInfo.video_current);

	int res = pstmt->executeUpdate();
	this->con->commit();

	return (res > 0);
}

VIDEOBACK VideoOperator::queryVideo(VIDEOPLAY& infoSearch, int user_id)
{
	sql::PreparedStatement* pstmt = this->con->prepareStatement("SELECT * FROM tbl_video WHERE "
																"user_id = ? AND video_time LIKE ? LIMIT ? OFFSET ?;");
	int pageSize = 9;	//每页显示的记录数
	int offset = (infoSearch.page - 1) * pageSize;
	string date = infoSearch.date;

	//判断是否为按月查询
	if (strlen(infoSearch.date) < 9)
	{
		date += "-%";
	}


	pstmt->setInt(1, user_id);
	pstmt->setString(2, date);
	pstmt->setInt(3, pageSize);
	pstmt->setInt(4, offset);

	sql::ResultSet* rs = pstmt->executeQuery();
	con->commit();

	VIDEOBACK videoBack;
	int index = 0;
	videoBack.number = 0;

	while (rs->next())
	{
		strcpy(videoBack.video_back[index].video_name, rs->getString("video_name").c_str());
		strcpy(videoBack.video_back[index].video_path, rs->getString("video_path").c_str());
		strcpy(videoBack.video_back[index].picture_path, rs->getString("picture_path").c_str());
		videoBack.video_back[index].video_frame = rs->getInt("video_frame");
		videoBack.video_back[index].video_current = rs->getInt("video_current");
		
		++index;
		
	}
	videoBack.number = index;

	return videoBack;
}

VIDEODATE VideoOperator::queryDay(int user_id)
{
	VIDEODATE videoDate;
	int number = 0;
	time_t now = std::time(nullptr);
	char buffer[11];  // 格式: YYYY-MM-% + 终止符
	std::strftime(buffer, sizeof(buffer), "%Y-%m", std::localtime(&now));

	string time = buffer;
	time += "-%";

	sql::PreparedStatement* pstmt = con->prepareStatement("SELECT DISTINCT video_time FROM tbl_video WHERE video_time LIKE ? AND user_id = ?;");
	pstmt->setString(1,time);
	pstmt->setInt(2, user_id);


	sql::ResultSet* rs = pstmt->executeQuery();
	this->con->commit();

	while (rs->next())
	{
		strcpy(videoDate.video_time[number], rs->getString("video_time").c_str());
		number++;
	}

	videoDate.number = number;

	return videoDate;
}

VIDEODATE VideoOperator::queryMonth(int user_id)
{
	VIDEODATE videoDate;
	int number = 0;
	time_t now = std::time(nullptr);
	char buffer[11];  // 格式: YYYY-%
	std::strftime(buffer, sizeof(buffer), "%Y", std::localtime(&now));

	string time = buffer;
	time += "-%";

	sql::PreparedStatement* pstmt = con->prepareStatement("SELECT DISTINCT SUBSTRING(video_time, 1, 7) AS video_time "
		"FROM tbl_video WHERE video_time LIKE ? AND user_id = ? ; ");
	pstmt->setString(1, time);
	pstmt->setInt(2, user_id);



	sql::ResultSet* rs = pstmt->executeQuery();

	this->con->commit();

	while (rs->next())
	{
		strcpy(videoDate.video_time[number], rs->getString("video_time").c_str());
		number++;
	}

	videoDate.number = number;

	return videoDate;
}
