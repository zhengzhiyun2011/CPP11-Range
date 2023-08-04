#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <type_traits>
#include <stdexcept>
using std::cout;
using std::cin;
using std::vector;
using std::deque;
using std::list;
using std::enable_if;
using std::is_integral;
using std::out_of_range;

// 判读类型T是否是整数的enable_if
template <typename T, typename RetType = void>
using enableIsIntType = typename enable_if<is_integral<T>::value, RetType>::type;

// Range类（每一个Range对象都是一次性的）
class Range
{
private:
	class RangeEndIterator;
public:
	class RangeIterator;
	using iterator = RangeIterator;

	// Range类的迭代器类
	class RangeIterator
	{
	public:
		RangeIterator() = delete;
		// 有参构造函数，指定迭代器属于的Range对象
		RangeIterator(Range* pRange) : m_pRange(pRange) {}
		RangeIterator(const RangeIterator& other) = default;
		RangeIterator(RangeIterator&& right) = default;
		RangeIterator& operator=(const RangeIterator& other) = default;
		RangeIterator& operator=(RangeIterator&& right) = default;

		// 相等比较操作符
		bool operator==(const RangeIterator& other)
		{
			// 如果不是“哨兵”迭代器，就直接比较数字，否则返回是否到了“哨兵“迭代器
			if (!other.m_isEndIterator)
				return m_pRange->m_num == other.m_pRange->m_num;
			else
				return m_pRange->m_num == m_pRange->m_maxNum;
		}

		// 小于比较操作符
		bool operator<(const RangeIterator& other)
		{
			// 如果不是“哨兵”迭代器就正常比较，否则永远返回true
			if (!other.m_isEndIterator)
				return m_pRange->m_num < other.m_pRange->m_num;
			else
				return true;
		}

		// 大于比较操作符
		bool operator>(const RangeIterator& other)
		{
			// 如果不是“哨兵”迭代器就正常比较，否则永远返回false
			if (!other.m_isEndIterator)
				return m_pRange->m_num > other.m_pRange->m_num;
			else
				return false;
		}

		// 小于等于比较操作符
		bool operator<=(const RangeIterator& other)
		{
			return !(*this > other);
		}

		// 大于等于比较操作符
		bool operator>=(const RangeIterator& other)
		{
			return !(*this < other);
		}

		// 不等比较操作符
		bool operator!=(const RangeIterator& other)
		{
			if (!other.m_isEndIterator)
				return !(*this == other);
			else    // 对“哨兵”迭代器优化
				return m_pRange->m_num < m_pRange->m_maxNum;
		}

		// 前置自增操作符
		RangeIterator& operator++()
		{
			// 如果下一个数字在范围之内就将数字加一，否则抛出异常
			if (m_pRange->m_num < m_pRange->m_maxNum)
				m_pRange->m_num += m_pRange->m_step;
			else
				throw out_of_range("Cannot set iterator after end!");

			return *this;
		}

		// 后置自增操作符
		RangeIterator operator++(int)
		{
			auto temp = *this;
			++*this;
			return temp;
		}

		// 解引用操作符
		size_t operator*()
		{
			if (m_pRange->m_num < m_pRange->m_maxNum)
				return m_pRange->m_num;
			else
				throw out_of_range("Cannot use the iterator after end!");
		}

		friend RangeEndIterator;
	private:
		Range* m_pRange;    // 指向属于的Range对象的指针
		bool m_isEndIterator = false;    // 是否为哨兵迭代器的标志
	};

	Range() = delete;
	Range(size_t end)
		: m_num(0)
		, m_startNum(0)
		, m_maxNum(end)
		, m_step(1) {}

	Range(size_t start, size_t end, size_t step = 1)
		: m_num(start)
		, m_startNum(start)
		, m_maxNum(end)
		, m_step(step) {}

	Range(const Range& other) = default;
	Range(Range&& other) = default;
	Range& operator=(const Range& other) = default;
	Range& operator=(Range&& right) = default;

	// 将Range转换成vector的函数
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator vector<T>()
	{
		vector<T> result(m_maxNum - m_startNum);
		for (int i = m_startNum; i < m_maxNum; ++i)
			result[i - m_startNum] = i;

		return result;
	}

	// 将Range转换成deque的函数
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator deque<T>()
	{
		deque<T> result(m_maxNum - m_startNum);
		for (int i = m_startNum; i < m_maxNum; ++i)
			result[i - m_startNum] = i;

		return result;
	}

	// 将Range转换成list的函数
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator list<T>()
	{
		list<T> result(m_maxNum - m_startNum);
		for (auto& iter : result)
			iter = next();

		return result;
	}
private:
	// m_num: 当前的数 | m_maxNum: Range最后的数字 + 1 | m_startNum: 第一个数字 | m_step每一步的步长
	size_t m_num, m_maxNum, m_startNum, m_step;

	// RangeEndIterator类型，Range的“哨兵”
	class RangeEndIterator
	{
	public:
		RangeEndIterator() = delete;
		RangeEndIterator(Range* pRange) noexcept : m_pRange(pRange) {}

		// 转换成RangeIterator
		operator RangeIterator()
		{
			RangeIterator result(m_pRange);
			result.m_isEndIterator = true;    // 设置哨兵迭代器的标志为true
			return result;
		}
	private:
		Range* m_pRange;    // 指向属于的Range对象的指针
	};
public:
	// 返回起始迭代器
	RangeIterator begin()
	{
		return RangeIterator{ this };
	}

	// 返回哨兵迭代器
	RangeIterator end()
	{
		return RangeEndIterator{ this };
	}

	// 返回下一个元素
	size_t next()
	{
		// 如果当前数字在范围之内就返回，否则抛出异常
		if (m_num < m_maxNum)
		{
			size_t temp = m_num;
			m_num += m_step;
			return temp;
		}
		else
			throw out_of_range("Cannot set iterator after end!");
	}
};

// range函数，创建一个Range对象并返回
inline Range range(size_t end)
{
	return Range{ end };
}

// 创建具有起始位置的Range对象
inline Range range(size_t start, size_t end, size_t step = 1)
{
	return Range{ start, end, step };
}

int main()
{
	// 创建一个range对象
	Range myRange = range(10);

	// 可以用C++的for each循环遍历
	for (auto i : myRange)
	{
		if (i == 4)
			break;

		cout << i << '\n';
	}

	cout << "the second test: \n";

	// 遍历剩下的
	for (auto i : myRange)
		cout << i << '\n';

	Range range2 = range(2, 10, 2);

	cout << "the third test:\n";

	for (auto i : range2)
		cout << i << '\n';

	return 0;
}