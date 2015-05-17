#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>
#include "common.h"

namespace rte {

template<class TTaskDesc>
class TaskConsumerThread
{
public:
	TaskConsumerThread() :
		mpConsumeThread(nullptr), mIsThreadStop(false),
		mIsEnqueueCompleted(true)
	{}

	~TaskConsumerThread()
	{
		if (mpConsumeThread != nullptr)
		{
			stopSync();
			safeDelete(mpConsumeThread);
		}
	}

	void start(std::function<void(TTaskDesc*)> taskProcessor)
	{
		if (mpConsumeThread != nullptr)
		{
			// 稼働中のタスクだけ終わらせてスレッドを止める
			bool tmp = mIsNeedProcessRestTasksAfterStop;
			stopSync(true);
			safeDelete(mpConsumeThread);
			mIsNeedProcessRestTasksAfterStop = tmp;
		}

		mIsThreadStop = false;
		mpConsumeThread = new std::thread(
			std::bind(&TaskConsumerThread::threadFunc, this, taskProcessor));
	}

	void stop(bool dropRestTasks = true)
	{
		mIsThreadStop = true;
		mIsNeedProcessRestTasksAfterStop = !dropRestTasks;
		mEnqueueEvent.notify_one();
	}

	void stopSync(bool dropRestTasks = true)
	{
		stop(dropRestTasks);
		join();
	}

	void join()
	{
		if (mpConsumeThread->joinable())
		{
			mpConsumeThread->join();
		}
	}

	void addTask(TTaskDesc* pDesc, std::function<void(TTaskDesc*)> onProcessComplete = nullptr)
	{
		if (mIsThreadStop)
		{
			return;
		}

		{
			std::unique_lock<std::mutex> lock(mTaskQueueLock);
			mTaskQueue.push(Task{ pDesc, onProcessComplete });
		}

		{
			std::unique_lock<std::mutex> lock(mEnqueueEventLock);
			mIsEnqueueCompleted = true;
		}
		mEnqueueEvent.notify_one();
	}

private:
	std::thread* mpConsumeThread;
	std::atomic<bool> mIsThreadStop;
	std::atomic<bool> mIsNeedProcessRestTasksAfterStop;

	struct Task
	{
		TTaskDesc* pDesc;
		std::function<void(TTaskDesc*)> onComplete;
	};
	std::queue<Task> mTaskQueue;
	std::mutex mTaskQueueLock;

	std::condition_variable mEnqueueEvent;
	std::mutex mEnqueueEventLock;
	bool mIsEnqueueCompleted;

	void threadFunc(std::function<void(TTaskDesc*)> taskProcessor)
	{
		while (!mIsThreadStop)
		{
			{
				std::unique_lock<std::mutex> lock(mEnqueueEventLock);
				mEnqueueEvent.wait(lock, [&](){ return mIsEnqueueCompleted; });
				mIsEnqueueCompleted = false;
			}

			while (!mTaskQueue.empty())
			{
				if (mIsThreadStop && !mIsNeedProcessRestTasksAfterStop)
				{
					break;
				}

				Task task;
				{
					std::unique_lock<std::mutex> lock(mTaskQueueLock);
					task = mTaskQueue.front();
					mTaskQueue.pop();
				}
				taskProcessor(task.pDesc);
				if (task.onComplete != nullptr)
				{
					task.onComplete(task.pDesc);
				}
			}
		}
	}
};

}// namespace rte
