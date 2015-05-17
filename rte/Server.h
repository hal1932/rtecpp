#pragma once
#include <string>
#include <functional>

namespace rte {

class Server
{
public:
	Server();
	~Server();

	bool connect(const std::string& ip, int port);
	void disconnect();

	void setReadTimeout(int seconds);
	size_t read(void* buffer, size_t bufferLength);
	void readAsync(void* buffer, size_t bufferLength, std::function<void(size_t)> onReadComplete = nullptr);

	void setWriteTimeout(int seconds);
	size_t write(void* buffer, size_t bufferLength);
	void writeAsync(void* buffer, size_t bufferLength, std::function<void(size_t)> onWriteComplete = nullptr);

private:
	struct Impl;
	Impl* mpImpl;
};

}// namespace rte
