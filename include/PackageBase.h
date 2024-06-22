#pragma once

#include <cstdint>
#include <memory>


namespace fbp {
struct PackageBase
{
public:
	PackageBase() : m_flags(0) {}

	inline bool IsLast() { return CheckFlag(FlagId::END_OF_STREAM); }

private:
	uint32_t m_flags;

	enum class FlagId { END_OF_STREAM };

	bool CheckFlag(FlagId id) { return (m_flags & (uint32_t(1) << static_cast<int>(id))) != 0; }
	bool AddFlag(FlagId id)
	{
		if (CheckFlag(id)) return true;
		m_flags += (uint32_t(1) << static_cast<int>(id));
		return true;
	}

	friend struct PackageEndOfStream;
};


struct PackageEndOfStream : public PackageBase
{
public:
	PackageEndOfStream() : PackageBase()
	{
		AddFlag(FlagId::END_OF_STREAM);
	}
};

inline std::unique_ptr<PackageBase> MakePackageEndOfStream() {
	return std::make_unique<PackageEndOfStream>();
}

}
