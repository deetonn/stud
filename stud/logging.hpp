#pragma once

#include "forward.hpp"

#include <iostream>
#include <string>
#include <functional>

_STD_API_BEGIN

class _Stdout_io {
public:
	void write(const char* data) {
		std::cout << data;
	}
	void writeln(const char* data) {
		std::cout << data << '\n';
	}
};
class _Stderr_io {
public:
	void write(const char* data) {
		std::cerr << data;
	}
	void writeln(const char* data) {
		std::cerr << data << '\n';
	}
};

template <class _IoWrap, class... Ts>
class logger_context {
public:
	using io = _IoWrap;
	using log_fmt_fn = std::function<std::string(Ts...)>;
private:
	log_fmt_fn _Log_fmt_builder;
	io _Io;
public:
	constexpr logger_context() {
		_Log_fmt_builder = [](Ts... args) {
			throw std::exception("You must has a format function! call logger.with_log_fmt(...)");
			return "";
		};
	}
	constexpr logger_context(const log_fmt_fn& fmt_f) noexcept {
		with_log_fmt(fmt_f);
	}

	constexpr void with_log_fmt(const log_fmt_fn& fmt_f) noexcept {
		_Log_fmt_builder = fmt_f;
	}

	constexpr void log(Ts&&... args) noexcept {
		const auto _Built_fmt = _Log_fmt_builder(std::forward<Ts>(args)...);
		_Io.write(_Built_fmt.c_str());
	}

	constexpr void logln(Ts&&... args) noexcept {
		const auto _Built_fmt = _Log_fmt_builder(std::forward<Ts>(args)...);
		_Io.writeln(_Built_fmt.c_str());
	}

	constexpr _IoWrap& wrapper() noexcept {
		return _Io;
	}
};

template <class ...Ts>
using stdout_logger = logger_context<_Stdout_io, Ts...>;

template <class ...Ts>
using stderr_logger = logger_context<_Stderr_io, Ts...>;

enum class LogLevel {
	info,
	warning,
	error
};

constexpr auto log_level_to_string(LogLevel level) {
	switch (level) {
	case LogLevel::info:
		return "INFO";
	case LogLevel::warning:
		return "WARN";
	case LogLevel::error:
		return "ERROR";
	}
}

using default_logger = logger_context<_Stdout_io, LogLevel, const std::string&>;

_STD_INLINE default_logger make_logger() {
	auto logger = default_logger{};
	logger.with_log_fmt([](LogLevel level, const std::string& message) {
		auto postfix_msg = log_level_to_string(level);
		return std::format("[{}] {}", postfix_msg, message);
	});
	return logger;
}

_STD_API_END