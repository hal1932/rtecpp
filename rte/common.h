#pragma once

namespace rte {

template<class T>
inline void safeDelete(T* pObj)
{
	if (pObj != nullptr)
	{
		delete pObj;
		pObj = nullptr;
	}
}

inline std::string basename(const std::string& path)
{
	int index = path.find_last_of('/');
	if (index == std::string::npos)
	{
		index = path.find_last_of('\\');
	}

	if (index != std::string::npos)
	{
		return path.substr(index + 1);
	}
	return path;
}

}// namespace rte
