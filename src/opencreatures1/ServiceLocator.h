#pragma once

#include "common/TypeErasedSharedPtr.h"

#include <vector>

class ServiceLocator {
  public:
	template <typename T>
	T* get() const {
		size_t i = index<T>();
		if (services.size() > i) {
			return services[i].get<T>();
		}
		return nullptr;
	}

	template <typename T>
	void insert(std::shared_ptr<T> obj) {
		size_t i = index<T>();
		// TODO: error if we already have a service?
		if (services.size() <= i) {
			services.resize(i + 1);
		}
		services[i] = obj;
	}

	template <typename T, typename... Args>
	void emplace(Args... args) {
		insert(std::make_shared<T>(args...));
	}

	void clear() {
		services.clear();
	}

  private:
	static size_t next_index;

	template <typename T>
	static size_t index() {
		static const size_t index_ = next_index++;
		return index_;
	}

	std::vector<TypeErasedSharedPtr> services;
};

static ServiceLocator g_services;