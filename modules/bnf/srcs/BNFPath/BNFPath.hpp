#ifndef BNF_PATH_HPP
#define BNF_PATH_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

class BNFPath
{
	public:
		BNFPath(const ARulePath &p, size_t idx) : rulePath(p), prev(NULL), isInvalidedPath(false), index(idx)
		{
		}
		BNFPath(const ARulePath &p, BNFPath *pr, size_t idx) : rulePath(p), prev(pr), isInvalidedPath(false), index(idx)
		{
		}
		~BNFPath()
		{
		}
		const ARulePath &rulePath;
		BNFPath			*prev;
		Vec<BNFPath>	 next;
		bool			 isInvalidedPath;
		size_t			 index;

		static bool compare(const ARulePath &a, const ARulePath &b)
		{
			if (a.size() != b.size())
				return false;
			for (size_t i = 0; i < a.size(); ++i)
			{
				if (a[i] != b[i])
					return false;
			}
			return true;
		}

		BNFPath operator=(const BNFPath &other)
		{
			return BNFPath(other.rulePath, other.prev, other.index);
		}

		bool operator==(const BNFPath &other) const
		{
			return compare(rulePath, other.rulePath);
		}

		void setNext(const ARulePath &p, size_t idx)
		{
			next.push(BNFPath(p, this, idx));
		}

		bool hasAllNextInvalided()
		{
			for (size_t i = 0; i < next.size(); ++i)
			{
				if (!next[i].isInvalidedPath)
					return false;
			}
			return true;
		}

		void deleteBranch()
		{
			BNFPath *current = this;
			BNFPath *parent	 = current->prev;

			current->isInvalidedPath = true;
			while (parent)
			{
				current->isInvalidedPath = true;
				if (parent->next.size() > 1)
				{
					if (parent->hasAllNextInvalided())
						parent->deleteBranch();
					return;
				}
				current = parent;
				parent	= parent->prev;
			}
		}

		const BNFPath *getHead() const
		{
			const BNFPath *head = this;

			while (head->prev)
				head = head->prev;
			return head;
		}
};

#endif