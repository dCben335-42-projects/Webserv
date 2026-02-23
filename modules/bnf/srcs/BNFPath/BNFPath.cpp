#include "BNFPath.hpp"


BNFPath::BNFPath(
	const ARulePath		 &p,
	const size_t		  idx,
	const Vec<ARulePath> &wordStartPath) :
	rulePath(p),
	prev(NULL),
	isInvalidedPath(false),
	index(idx),
	wordStartPath(wordStartPath)
{
}

BNFPath::BNFPath(
	const ARulePath		 &p,
	BNFPath *const		  pr,
	const size_t		  idx,
	const Vec<ARulePath> &wordStartPath) :
	rulePath(p),
	prev(pr),
	isInvalidedPath(false),
	index(idx),
	wordStartPath(wordStartPath)
{
}

BNFPath::~BNFPath()
{
}


bool BNFPath::compare(const ARulePath &a, const ARulePath &b)
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

BNFPath BNFPath::operator=(const BNFPath &other)
{
	return BNFPath(other.rulePath, other.prev, other.index, other.wordStartPath);
}

bool BNFPath::operator==(const BNFPath &other) const
{
	return compare(rulePath, other.rulePath);
}

void BNFPath::setNext(const ARulePath &p, size_t idx, Vec<ARulePath> wordStartPath)
{
	next.push(BNFPath(p, this, idx, wordStartPath)); // next.push(BNFPath(p, this, idx));
}

bool BNFPath::hasAllNextInvalided()
{
	for (size_t i = 0; i < next.size(); ++i)
	{
		if (!next[i].isInvalidedPath)
			return false;
	}
	return true;
}

void BNFPath::deleteBranch()
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

const BNFPath *BNFPath::getHead() const
{
	const BNFPath *head = this;

	while (head->prev)
	{
		head			  = head->prev;
		size_t validPaths = 0;
		for (size_t i = 0; i < head->next.size(); ++i)
		{
			validPaths += !head->next[i].isInvalidedPath;
			if (validPaths > 1)
				throw std::runtime_error("Ambiguous path resolution.");
		}
	}
	return head;
}


std::ostream &operator<<(std::ostream &os, const BNFPath &data)
{
	os << data.next.size();
	return os;
}