#include "DBConnection.h"

DBConnection* DBConnection::dbconn = nullptr;
std::mutex DBConnection::conn_mutex;

DBConnection::DBConnection()
{
	this->driver = get_mysql_driver_instance();
	this->path = "tcp://" + localhost + ":" + localport;
	this->conn.reset(this->driver->connect(path, localuser, localpasswd));
	this->conn->setSchema(localschema);
	this->conn->setAutoCommit(0);
	cout << "数据库连接成功！" << endl;
}

DBConnection* DBConnection::getInstance()
{
	if (dbconn == nullptr)
	{
		dbconn = new DBConnection();
	}
	return dbconn;
}

sql::Connection* DBConnection::getConnection() {
	std::lock_guard<std::mutex> lock(this->conn_mutex);  // 加锁
	if (this->conn == nullptr || !this->conn->isValid()) {
		reconnect();
	}
	return this->conn.get();
}

// 数据库重连方法
bool DBConnection::reconnect() {
	try {
		if (this->conn) {
			this->conn->close();
			this->conn.reset();
		}

		this->conn.reset(this->driver->connect(path, localuser, localpasswd));
		this->conn->setSchema(localschema);
		this->conn->setAutoCommit(0);

		cout << "数据库重连成功！" << endl;
		return true;
	}
	catch (sql::SQLException& e) {
		cout << "数据库重连失败: " << e.what() << endl;
		return false;
	}
}


DBConnection::~DBConnection()
{
	disConnection();
}

void DBConnection::disConnection()
{
	if (this->conn)
	{
		this->conn->close();
		this->conn.reset();
	}
}



