#pragma once

#include <vector>
#include <utility>

namespace forge {

		// 256 to start with
#define FORGE_STACK_SIZE 256

	template <typename T>
	class stack
	{
	public:
		stack() {
			m_data.reserve(FORGE_STACK_SIZE);
		}

		~stack() {

		}

		void push(const T& element) {
			m_data.push_back(std::move(element));
		}

		const T& pop() {
			const T& element = m_data.back();
			m_data.pop_back();
			return element;
		}

		size_t size() const {
			return m_data.size();
		}

		const T& peek(int32_t distance) {
			size_t index = m_data.size() - distance - 1;
			FG_ASSERT(index >= 0 && index < m_data.size(), "Index out of bounds!");
			return m_data.at(index);
		}

	private:
		std::vector<T> m_data;
	};

}
