#include "OperatorFactory.h"

OperatorFactory* OperatorFactory::of = nullptr;

OperatorFactory* OperatorFactory::getInstance()
{
	if (OperatorFactory::of == nullptr)
	{
		OperatorFactory::of = new OperatorFactory();
	}
	return of;
}

OperatorFactory::~OperatorFactory()
{
	delete this->uo;
	delete this->ro;
	delete this->fr;  
	delete this->po; 
	delete this->lo;  
	delete this->vo;  
}

void* OperatorFactory::createRepository(RepositoryType type)
{
	switch (type)
	{
		case OperatorFactory::USER:
			if (this->uo == nullptr)
			{
				this->uo = new UserOperator();
			}
			return this->uo;
		case OperatorFactory::CARRECORD:
			if (this->ro == nullptr)
			{
				this->ro = new RecordOperator();
			}
			return this->ro;
		case OperatorFactory::FILELOG:
			if (this->fr == nullptr)
			{
				this->fr = new FileLogRecord();
			}
			return this->fr;
		case OperatorFactory::PICTURECORED:
			if (this->po == nullptr)
			{
				this->po = new PictureOperator();
			}
			return this->po;
		case OperatorFactory::LOG:
			if (this->lo == nullptr)
			{
				this->lo = new LogOperator();
			}
			return this->lo;
		case OperatorFactory::VIDEORECORD:
			if (this->vo == nullptr)
			{
				this->vo = new VideoOperator();
			}
			return this->vo;
		default:
			break;
	}
}

OperatorFactory::OperatorFactory()
{
	this->uo = nullptr;
	this->ro = nullptr;
	this->fr = nullptr;
	this->po = nullptr;
	this->lo = nullptr;
	this->vo = nullptr;
}
