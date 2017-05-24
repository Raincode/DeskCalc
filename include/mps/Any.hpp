#pragma once

#include <string>
#include <utility>

// #include "util.hpp"

struct A {};

namespace detail {
	class BaseValue {
	public:
		virtual ~BaseValue() { }

		virtual const std::type_info& type() const = 0;

		virtual BaseValue* clone() const = 0;

		virtual void print(std::ostream& os) const = 0;
	};

	template<class T>
	class Value : public BaseValue {
	public:
		explicit Value(const T& value)
			: val{ value }
		{
		}

		explicit Value(T&& value)
			: val{ std::forward<T>(value) }
		{
		}

		const std::type_info& type() const override
		{
			return typeid(val);
		}

		BaseValue* clone() const override
		{
			return new Value{ val };
		}

		void print(std::ostream& os) const override
		{
			os << val;
		}

		const T val;
	};
}	/* namespace detail */

class Any {
public:
	Any() = default;

	~Any()
	{
		delete ptr;
	}

	template<class T>
	Any(const T& val)
		: ptr{ new detail::Value<T>{ val } }
	{
	}

	Any(const char* str)
		: ptr{ new detail::Value<std::string>{ str } }
	{
	}

	Any(const Any& a)
		: ptr{ a.ptr ? a.ptr->clone() : a.ptr }
	{
	}

	Any& operator=(Any copy)
	{
		swap(*this, copy);
		return *this;
	}

	Any(Any&& from)
		: Any{}
	{
		swap(*this, from);
	}

	template<class T>
	Any& operator=(const T& val)
	{
		swap(Any{ val }, *this);
		return *this;
	}

	template <class T>
	Any& operator=(T&& val)
	{
		swap(Any{ static_cast<T&&>(val) }, *this);
		return *this;
	}

	friend void swap(Any& first, Any& second) noexcept
	{
		using std::swap;
		swap(first.ptr, second.ptr);
	}

	const std::type_info& type() const
	{
		return ptr ? ptr->type() : typeid(void);
	}

	bool empty() const
	{
		return !ptr;
	}

	void clear()
	{
		swap(Any{}, *this);
	}

	template<class T>
	const T& as() const
	{
		if (type() != typeid(T)) throw std::bad_cast{};
		return static_cast<detail::Value<T>*>(ptr)->val;
	}

	friend std::ostream& operator<<(std::ostream& os, const Any& any)
	{
		if (!any.empty()) {
			any.ptr->print(os);
		}
		else {
			os << "empty";
		}
		return os;
	}

	template<class T>
	operator const T&() const
	{
		return as<T>();
	}

private:
	detail::BaseValue* ptr{};
};