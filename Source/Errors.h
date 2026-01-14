#pragma once
#include <stdexcept>
#include <string>

namespace Errors
{
	class GameError : public std::runtime_error
	{
	public:
		explicit GameError(const std::string& message) : std::runtime_error(message) {}
		explicit GameError(std::string&& message) : std::runtime_error(std::move(message)) {}
	};

	class ResourceError : public GameError
	{
	public:
		explicit ResourceError(const std::string& message) : GameError(message) {}
	};
	class FileError : public GameError
	{
	public:
		explicit FileError(const std::string& message) : GameError(message) {}
	};
	class PreconditionError : public GameError
	{
	public:
		explicit PreconditionError(const std::string& message) : GameError(message) {}
	};
	class PostconditionError : public GameError
	{
	public:
		explicit PostconditionError(const std::string& message) : GameError(message) {}
	};
	class InvariantViolation : public GameError
	{
	public:
		explicit InvariantViolation(const std::string& message) : GameError(message) {}
	};



	inline void ensurePrecondition(bool condition, const std::string& message)
	{
		if (!condition) throw PreconditionError(message);
	}

	inline void ensurePostcondition(bool condition, const std::string& message)
	{
		if (!condition) throw PostconditionError(message);
	}

	inline void ensureInvariant(bool condition, const std::string& message)
	{
		if (!condition) throw InvariantViolation(message);
	}


	

	[[noreturn]] inline void failResource(const std::string& message)
	{
		throw ResourceError(message);
	}
}


