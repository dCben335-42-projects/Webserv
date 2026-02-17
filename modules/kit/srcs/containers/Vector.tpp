#ifndef KIT_VECTOR_TPP
#define KIT_VECTOR_TPP

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
		/**
		 *
		 * @brief Checks if the vector contains the given index.
		 *
		 * @param index The index to be checked.
		 *
		 * @return true if the index exists in the vector, false otherwise.
		 *
		 */
		bool has(size_t index) const
		{
			return index < static_cast<const Derived &>(*this).size();
		}

		/**
		 *
		 * @brief Checks if the vector contains the given value.
		 *
		 * @param value The value to be checked.
		 *
		 * @return true if the value exists in the vector, false otherwise.
		 *
		 */
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

		/**
		 *
		 * @brief Gets the value at the given index in the vector.
		 *
		 * @param index The index of the value to be retrieved.
		 *
		 * @return The value at the given index.
		 *
		 */
		V &get(size_t index)
		{
			return static_cast<Derived &>(*this)[index];
		}

		/**
		 *
		 * @brief Gets the value at the given index in the vector.
		 *
		 * @param index The index of the value to be retrieved.
		 *
		 * @return The value at the given index.
		 *
		 */
		const V &get(size_t index) const
		{
			return static_cast<const Derived &>(*this)[index];
		}

		/**
		 *
		 * @brief Sets the value at the given index in the vector.
		 *
		 * @param index The index of the value to be set.
		 * @param value The value to be set at the given index.
		 *
		 */
		void set(size_t index, const V &value)
		{
			static_cast<Derived &>(*this)[index] = value;
		}

		void remove(size_t index)
		{
			static_cast<Derived &>(*this).erase(
				static_cast<Derived &>(*this).begin() + index);
		}

		/**
		 *
		 * @brief Tries to get the value at the given index.
		 *
		 * @param index The index to search for in the vector.
		 * @param out A reference to a variable that will hold the value if found.
		 *
		 * @return true if the index exists in the vector and out is set to the value, false otherwise.
		 *
		 */
		bool try_get(size_t index, V &out) const
		{
			if (index >= static_cast<const Derived &>(*this).size())
				return false;
			out = static_cast<const Derived &>(*this)[index];
			return true;
		}

		/**
		 *
		 * @brief Tries to get a pointer to the value at the given index.
		 *
		 * @param index The index to search for in the vector.
		 * @param out A reference to a pointer that will point to the value if found.
		 *
		 * @return true if the index exists in the vector and out is set to point to the value, false otherwise.
		 *
		 */
		bool try_get(size_t index, V *&out)
		{
			if (index >= static_cast<Derived &>(*this).size())
				return false;
			out = &static_cast<Derived &>(*this)[index];
			return true;
		}

		/**
		 *
		 * @brief Adds a value to the end of the vector.
		 *
		 * @param value The value to be added.
		 *
		 */
		void push(const V &value)
		{
			static_cast<Derived &>(*this).push_back(value);
		}

		/**
		 *
		 * @brief Finds the index of the given value in the vector.
		 *
		 * @param value The value to search for in the vector.
		 *
		 * @return The index of the value if found, -1 otherwise.
		 *
		 */
		size_t indexOf(const V &value) const
		{
			const Derived &derived = static_cast<const Derived &>(*this);
			for (size_t i = 0; i < derived.size(); ++i)
			{
				if (derived[i] == value)
					return i;
			}
			return static_cast<size_t>(-1);
		}


		/**
		 *
		 * @brief Adds a value to the front of the vector.
		 *
		 * @param value The value to be added.
		 *
		 */
		void push_front(const V &value)
		{
			static_cast<Derived &>(*this).insert(
				static_cast<Derived &>(*this).begin(), value);
		}

		/**
		 *
		 * @brief Reverts the order of elements in the vector.
		 *
		 * @note This method modifies the vector in place.
		 *
		 */
		void revert()
		{
			Derived &derived = static_cast<Derived &>(*this);
			size_t	 left	 = 0;
			size_t	 right	 = derived.size() - 1;

			while (left < right)
			{
				std::swap(derived[left], derived[right]);
				++left;
				--right;
			}
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
		void operator+=(const Vec &other)
		{
			base::insert(base::end(), other.begin(), other.end());
		}
		~Vec()
		{
		}
};


#endif
