#ifndef BNF_PATH_HPP
#define BNF_PATH_HPP

#include "Map.hpp"
#include "bnf_defs.hpp"

class BNFPath
{
	public:
		BNFPath(const ARulePath &p, size_t idx, Vec<ARulePath> wordStartPath) : rulePath(p), prev(NULL), isInvalidedPath(false), index(idx), wordStartPath(wordStartPath)
		{
		}
		BNFPath(const ARulePath &p, BNFPath *pr, size_t idx, Vec<ARulePath> wordStartPath) : rulePath(p), prev(pr), isInvalidedPath(false), index(idx), wordStartPath(wordStartPath)
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
		Vec<ARulePath>	 wordStartPath;
		static bool		 compare(const ARulePath &a, const ARulePath &b)
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
			return BNFPath(other.rulePath, other.prev, other.index, other.wordStartPath);
		}

		bool operator==(const BNFPath &other) const
		{
			return compare(rulePath, other.rulePath);
		}

		void setNext(const ARulePath &p, size_t idx, Vec<ARulePath> wordStartPath)
		{
			next.push(BNFPath(p, this, idx, wordStartPath)); // next.push(BNFPath(p, this, idx));
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
			const BNFPath *head	 = this;
			size_t		   count = 1;

			Logger::log("index: " + LoggerJson::get(head->index));


			while (head->prev)
			{
				head = head->prev;
				for (size_t i = 0; i < head->next.size(); ++i)
				{
					Logger::log("index: " + LoggerJson::get(head->index));
					head->next[i].isInvalidedPath ? Logger::log("- invalided path") : Logger::log("- valid path");
					// Logger::log("next index: " + LoggerJson::get(head->next[i].index));
					// Logger::log("next rulePath: " + LoggerJson::get(head->next[i].rulePath));
				}
				// Logger::log();
				count++;
			}
			// Logger::log("index: " + LoggerJson::get(head->index));

			Logger::log("Path length: " + LoggerJson::get(count));
			return head;
		}
};

#endif