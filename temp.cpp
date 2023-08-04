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

// �ж�����T�Ƿ���������enable_if
template <typename T, typename RetType = void>
using enableIsIntType = typename enable_if<is_integral<T>::value, RetType>::type;

// Range�ࣨÿһ��Range������һ���Եģ�
class Range
{
private:
	class RangeEndIterator;
public:
	class RangeIterator;
	using iterator = RangeIterator;

	// Range��ĵ�������
	class RangeIterator
	{
	public:
		RangeIterator() = delete;
		// �вι��캯����ָ�����������ڵ�Range����
		RangeIterator(Range* pRange) : m_pRange(pRange) {}
		RangeIterator(const RangeIterator& other) = default;
		RangeIterator(RangeIterator&& right) = default;
		RangeIterator& operator=(const RangeIterator& other) = default;
		RangeIterator& operator=(RangeIterator&& right) = default;

		// ��ȱȽϲ�����
		bool operator==(const RangeIterator& other)
		{
			// ������ǡ��ڱ�������������ֱ�ӱȽ����֣����򷵻��Ƿ��ˡ��ڱ���������
			if (!other.m_isEndIterator)
				return m_pRange->m_num == other.m_pRange->m_num;
			else
				return m_pRange->m_num == m_pRange->m_maxNum;
		}

		// С�ڱȽϲ�����
		bool operator<(const RangeIterator& other)
		{
			// ������ǡ��ڱ����������������Ƚϣ�������Զ����true
			if (!other.m_isEndIterator)
				return m_pRange->m_num < other.m_pRange->m_num;
			else
				return true;
		}

		// ���ڱȽϲ�����
		bool operator>(const RangeIterator& other)
		{
			// ������ǡ��ڱ����������������Ƚϣ�������Զ����false
			if (!other.m_isEndIterator)
				return m_pRange->m_num > other.m_pRange->m_num;
			else
				return false;
		}

		// С�ڵ��ڱȽϲ�����
		bool operator<=(const RangeIterator& other)
		{
			return !(*this > other);
		}

		// ���ڵ��ڱȽϲ�����
		bool operator>=(const RangeIterator& other)
		{
			return !(*this < other);
		}

		// ���ȱȽϲ�����
		bool operator!=(const RangeIterator& other)
		{
			if (!other.m_isEndIterator)
				return !(*this == other);
			else    // �ԡ��ڱ����������Ż�
				return m_pRange->m_num < m_pRange->m_maxNum;
		}

		// ǰ������������
		RangeIterator& operator++()
		{
			// �����һ�������ڷ�Χ֮�ھͽ����ּ�һ�������׳��쳣
			if (m_pRange->m_num < m_pRange->m_maxNum)
				m_pRange->m_num += m_pRange->m_step;
			else
				throw out_of_range("Cannot set iterator after end!");

			return *this;
		}

		// ��������������
		RangeIterator operator++(int)
		{
			auto temp = *this;
			++*this;
			return temp;
		}

		// �����ò�����
		size_t operator*()
		{
			if (m_pRange->m_num < m_pRange->m_maxNum)
				return m_pRange->m_num;
			else
				throw out_of_range("Cannot use the iterator after end!");
		}

		friend RangeEndIterator;
	private:
		Range* m_pRange;    // ָ�����ڵ�Range�����ָ��
		bool m_isEndIterator = false;    // �Ƿ�Ϊ�ڱ��������ı�־
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

	// ��Rangeת����vector�ĺ���
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator vector<T>()
	{
		vector<T> result(m_maxNum - m_startNum);
		for (int i = m_startNum; i < m_maxNum; ++i)
			result[i - m_startNum] = i;

		return result;
	}

	// ��Rangeת����deque�ĺ���
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator deque<T>()
	{
		deque<T> result(m_maxNum - m_startNum);
		for (int i = m_startNum; i < m_maxNum; ++i)
			result[i - m_startNum] = i;

		return result;
	}

	// ��Rangeת����list�ĺ���
	template <typename T, enableIsIntType<T>* = nullptr>
	explicit operator list<T>()
	{
		list<T> result(m_maxNum - m_startNum);
		for (auto& iter : result)
			iter = next();

		return result;
	}
private:
	// m_num: ��ǰ���� | m_maxNum: Range�������� + 1 | m_startNum: ��һ������ | m_stepÿһ���Ĳ���
	size_t m_num, m_maxNum, m_startNum, m_step;

	// RangeEndIterator���ͣ�Range�ġ��ڱ���
	class RangeEndIterator
	{
	public:
		RangeEndIterator() = delete;
		RangeEndIterator(Range* pRange) noexcept : m_pRange(pRange) {}

		// ת����RangeIterator
		operator RangeIterator()
		{
			RangeIterator result(m_pRange);
			result.m_isEndIterator = true;    // �����ڱ��������ı�־Ϊtrue
			return result;
		}
	private:
		Range* m_pRange;    // ָ�����ڵ�Range�����ָ��
	};
public:
	// ������ʼ������
	RangeIterator begin()
	{
		return RangeIterator{ this };
	}

	// �����ڱ�������
	RangeIterator end()
	{
		return RangeEndIterator{ this };
	}

	// ������һ��Ԫ��
	size_t next()
	{
		// �����ǰ�����ڷ�Χ֮�ھͷ��أ������׳��쳣
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

// range����������һ��Range���󲢷���
inline Range range(size_t end)
{
	return Range{ end };
}

// ����������ʼλ�õ�Range����
inline Range range(size_t start, size_t end, size_t step = 1)
{
	return Range{ start, end, step };
}

int main()
{
	// ����һ��range����
	Range myRange = range(10);

	// ������C++��for eachѭ������
	for (auto i : myRange)
	{
		if (i == 4)
			break;

		cout << i << '\n';
	}

	cout << "the second test: \n";

	// ����ʣ�µ�
	for (auto i : myRange)
		cout << i << '\n';

	Range range2 = range(2, 10, 2);

	cout << "the third test:\n";

	for (auto i : range2)
		cout << i << '\n';

	return 0;
}