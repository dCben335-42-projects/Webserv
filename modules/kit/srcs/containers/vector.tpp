#ifndef VECTOR_TPP
#define VECTOR_TPP

#include <vector>
#include <cstddef>

template <typename V>
class VecImpl
{
	public:
		typedef std::vector<V> vector_type;
};

template <typename Derived, typename V>
class VecBase
{
	public:
		bool has(size_t index) const
		{
			return index < static_cast<const Derived &>(*this).size();
		}

		bool contains(const V &value) const
		{
			const Derived &derived = static_cast<const Derived &>(*this);
			for (size_t i = 0; i < derived.size(); ++i)
			{
				if (derived[i] == value)
					return true;
			}
			return false;
		}

		V &get(size_t index)
		{
			return static_cast<Derived &>(*this)[index];
		}

		const V &get(size_t index) const
		{
			return static_cast<const Derived &>(*this)[index];
		}

		void set(size_t index, const V &value)
		{
			static_cast<Derived &>(*this)[index] = value;
		}

		bool try_get(size_t index, V &out) const
		{
			if (index >= static_cast<const Derived &>(*this).size())
				return false;
			out = static_cast<const Derived &>(*this)[index];
			return true;
		}

		bool try_get(size_t index, V *&out)
		{
			if (index >= static_cast<Derived &>(*this).size())
				return false;
			out = &static_cast<Derived &>(*this)[index];
			return true;
		}

		void push(const V &value)
		{
			static_cast<Derived &>(*this).push_back(value);
		}
};

template <typename V>
class Vec
	: public VecImpl<V>::vector_type,
	  public VecBase<Vec<V>, V>
{
	public:
		typedef typename VecImpl<V>::vector_type base;

		Vec() : base()
		{
		}
		Vec(const Vec &other) : base(other)
		{
		}
		Vec &operator=(const Vec &other)
		{
			base::operator=(other);
			return *this;
		}
		~Vec()
		{
		}
};


#endif