#pragma once

#include <cstdint>


namespace fbp {
class PackageBase
{
public:
	PackageBase() : m_flags(0) {}

	inline bool isLast() { return checkFlag(FlagId::END_OF_STREAM); }

private:
	uint32_t m_flags;

	enum class FlagId { END_OF_STREAM };

	bool checkFlag(FlagId id) { return (m_flags & (uint32_t(1) << static_cast<int>(id))) != 0; }
	bool addFlag(FlagId id)
	{
		if (checkFlag(id)) return true;
		m_flags += (uint32_t(1) << static_cast<int>(id));
		return true;
	}

	friend class PackageEndOfStream;
};


class PackageEndOfStream : public PackageBase
{
public:
	PackageEndOfStream() : PackageBase()
	{
		addFlag(FlagId::END_OF_STREAM);
	}
};

}

