#ifndef KIT_SET_TPP
#define KIT_SET_TPP

#include <set>
#include <cstddef>

template <typename V>
class SetImpl
{
	public:
		typedef std::set<V> set_type;
};

template <typename Derived, typename V>
class SetBase
{
	public:
		/**
		 *
		 * @brief Checks if the set contains the given value.
		 *
		 * @param value The value to be checked.
		 *
		 * @return true if the value exists in the set, false otherwise.
		 *
		 */
		bool contains(const V &value) const
		{
			const Derived &derived = static_cast<const Derived &>(*this);
			return derived.find(value) != derived.end();
		}

		/**
		 *
		 * @brief Adds a value to the set.
		 *
		 * @param value The value to be added.
		 *
		 */
		void push(const V &value)
		{
			static_cast<Derived &>(*this).insert(value);
		}

		/**
		 *
		 * @brief Removes a value from the set.
		 *
		 * @param value The value to be removed.
		 *
		 */
		void erase(const V &value)
		{
			static_cast<Derived &>(*this).erase(value);
		}

		/**
		 *
		 * @brief Gets the number of elements in the set.
		 *
		 * @return The size of the set.
		 *
		 */
		size_t size() const
		{
			return static_cast<const Derived &>(*this).size();
		}

		/**
		 *
		 * @brief Gets the value from the set that matches the given value.
		 *
		 * @param value The value to be searched for.
		 *
		 * @return The value from the set that matches the given value.
		 *
		 * @throws std::out_of_range if the value is not found in the set.
		 *
		 */
		const V &get(const V &value) const
		{
			return const_cast<SetBase<Derived, V> *>(this)->get(value);
		}

		/**
		 *
		 * @brief Gets the value from the set that matches the given value.
		 *
		 * @param value The value to be searched for.
		 *
		 * @return The value from the set that matches the given value.
		 *
		 * @throws std::out_of_range if the value is not found in the set.
		 *
		 */
		V &get(const V &value)
		{
			return const_cast<V &>(static_cast<const Derived &>(*this).find(value)->operator*());
		}


		/**
		 *
		 * @brief Tries to get the value from the set that matches the given value.
		 *
		 * @param value The value to be searched for.
		 * @param out A reference to a variable that will hold the found value.
		 *
		 * @return true if the value is found in the set and out is set, false otherwise.
		 *
		 */
		bool try_get(const V &value, V &out) const
		{
			const Derived					&derived = static_cast<const Derived &>(*this);
			typename Derived::const_iterator it		 = derived.find(value);
			if (it == derived.end())
				return false;
			out = *it;
			return true;
		}

		/**
		 *
		 * @brief Tries to get a pointer to the value from the set that matches the given value.
		 *
		 * @param value The value to be searched for.
		 * @param out A reference to a pointer that will point to the found value.
		 *
		 * @return true if the value is found in the set and out is set to point to it, false otherwise.
		 *
		 */
		bool try_get(const V &value, V *&out)
		{
			Derived					  &derived = static_cast<Derived &>(*this);
			typename Derived::iterator it	   = derived.find(value);
			if (it == derived.end())
				return false;
			out = &(*it);
			return true;
		}
};


template <typename V>
class Set : public SetImpl<V>::set_type,
			public SetBase<Set<V>, V>
{
	public:
		typedef typename SetImpl<V>::set_type base;

		Set() : base()
		{
		}
		Set(const Set &other) : base(other)
		{
		}
		Set &operator=(const Set &other)
		{
			base::operator=(other);
			return *this;
		}
};


#endif
