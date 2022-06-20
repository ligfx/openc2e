#pragma once

#include <memory>
#include <stddef.h>

class TypeErasedSharedPtr {
  public:
	TypeErasedSharedPtr() {}

	template <typename T>
	TypeErasedSharedPtr(std::shared_ptr<T> t) {
		static_assert(sizeof(t) == sizeof(storage_), "");
		deleter = deleter_for<T>();
		new (&storage_) std::shared_ptr<T>(std::move(t));
	}
	TypeErasedSharedPtr(const TypeErasedSharedPtr&) = delete;
	TypeErasedSharedPtr& operator=(const TypeErasedSharedPtr&) = delete;

	TypeErasedSharedPtr(TypeErasedSharedPtr&& other)
		: TypeErasedSharedPtr() {
		swap(other);
	}
	TypeErasedSharedPtr& operator=(TypeErasedSharedPtr&& other) {
		reset();
		swap(other);
		return *this;
	}

	~TypeErasedSharedPtr() {
		reset();
	}

	template <typename T>
	T* get() const {
		if (deleter == deleter_for<T>()) {
			const std::shared_ptr<T>* t = reinterpret_cast<const std::shared_ptr<T>*>(storage_);
			return t->get();
		}
		return nullptr;
	}

	void swap(TypeErasedSharedPtr& other) {
		memcpy(storage_, other.storage_, sizeof(storage_));
		deleter = other.deleter;
		other.deleter = nullptr;
	}

	void reset() {
		if (deleter) {
			deleter(storage_);
			deleter = nullptr;
		}
	}


  private:
	using deleter_t = void (*)(uint8_t*);

	template <typename T>
	static deleter_t deleter_for() {
		return [](uint8_t* storage) {
			std::shared_ptr<T>* t = reinterpret_cast<std::shared_ptr<T>*>(storage);
			t->~shared_ptr<T>();
		};
	}

	deleter_t deleter = nullptr;
	alignas(std::shared_ptr<void>) uint8_t storage_[sizeof(std::shared_ptr<void>)];
};