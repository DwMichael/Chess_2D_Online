#pragma once

#include <deque>
#include <mutex>


template<typename T>
class TSQueue {
protected:
	std::mutex muxQueue;
	std::deque<T> deQueue;
public:
	TSQueue() = default;
	TSQueue(const TSQueue<T>&) = delete;

	const T& front() {
		std::scoped_lock lock(muxQueue);
		return deQueue.front();
	}

	const T& back() {
		std::scoped_lock lock(muxQueue);
		return deQueue.back();
	}

	void push_back(const T& item) {
		std::scoped_lock lock(muxQueue);
		deQueue.emplace_back(std::move(item));
	}

	void push_front(const T& item) {
		std::scoped_lock lock(muxQueue);
		deQueue.emplace_front(std::move(item));
	}

	T pop_back() {
		std::scoped_lock lock(muxQueue);
		T tmp = std::move(deQueue.back());
		deQueue.pop_back();
		return tmp;
	}

	T pop_front() {
		std::scoped_lock lock(muxQueue);
		T tmp = std::move(deQueue.front());
		deQueue.pop_front();
		return tmp;
	}

	size_t count() {
		std::scoped_lock lock(muxQueue);
		return deQueue.size();
	}

	void clear() {
		std::scoped_lock lock(muxQueue);
		deQueue.clear();
	}

};