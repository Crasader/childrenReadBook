#include "YYXFunctionQueue.h"

YYXFunctionQueue* YYXFunctionQueue::instance = nullptr;

YYXFunctionQueue::YYXFunctionQueue()
{
}

YYXFunctionQueue::~YYXFunctionQueue()
{
}

YYXFunctionQueue* YYXFunctionQueue::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new YYXFunctionQueue();
	}
	return instance;
}

void YYXFunctionQueue::start()
{
	if (block)
		return;
	if (runQueue.empty())
	{
		block = true;
		return;
	}
	block = false;
	auto it = runQueue.front();
	if (it)
	{
		it();
	}
	if (!runQueue.empty())
		runQueue.pop();
	block = true;
	if (!block)
		start();
}

void YYXFunctionQueue::clear()
{
	while (!runQueue.empty())
	{
		runQueue.pop();
	}
}

void YYXFunctionQueue::add(function<void()> runFun)
{
	if(runFun)
		runQueue.push(runFun);
}

void YYXFunctionQueue::interdict()
{
	block = true;
}

void YYXFunctionQueue::resumption()
{
	block = false;
}