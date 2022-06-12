#pragma once

#include <typeindex>
#include <typeinfo>


class AnyPtr {
  public:
	AnyPtr(){};

	AnyPtr(std::nullptr_t)
		: AnyPtr(){};

	template <typename T>
	AnyPtr(T* ptr)
		: m_ptr(ptr), m_type(typeid(T)) {}

	operator bool() const {
		return m_ptr != nullptr;
	}

	template <typename T>
	T* get() {
		if (m_type == typeid(T)) {
			return reinterpret_cast<T*>(m_ptr);
		}
		return nullptr;
	}

  private:
	void* m_ptr = nullptr;
	std::type_index m_type = typeid(void);
};

class UniqueAnyPtr {
  public:
	UniqueAnyPtr() = default;

	template <typename T>
	UniqueAnyPtr(T* ptr)
		: m_ptr(ptr), m_deleter(deleter<T>()) {}

	UniqueAnyPtr(const UniqueAnyPtr& other) = delete;
	UniqueAnyPtr& operator=(const UniqueAnyPtr& other) = delete;

	UniqueAnyPtr(UniqueAnyPtr&& other) {
		m_ptr = other.m_ptr;
		m_deleter = other.m_deleter;
		;
		other.m_ptr = nullptr;
		other.m_deleter = nullptr;
	}
	UniqueAnyPtr& operator=(UniqueAnyPtr&& other) {
		reset();
		m_ptr = other.m_ptr;
		m_deleter = other.m_deleter;
		other.m_ptr = nullptr;
		other.m_deleter = nullptr;
		return *this;
	}

	~UniqueAnyPtr() {
		reset();
	}

	operator bool() const {
		return m_ptr;
	}

	template <typename T>
	T* get() {
		return m_ptr.get<T>();
	}

	void reset() {
		if (!m_ptr) {
			return;
		}
		m_deleter(m_ptr);
		m_ptr = nullptr;
		m_deleter = nullptr;
	}

  private:
	using deleter_t = void (*)(AnyPtr);

	template <typename T>
	static deleter_t deleter() {
		return [](AnyPtr ptr) { if (T *t = ptr.get<T>()) { delete t; } };
	}

	AnyPtr m_ptr;
	deleter_t m_deleter = nullptr;
};