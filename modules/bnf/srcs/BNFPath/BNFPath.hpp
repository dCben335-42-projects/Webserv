#ifndef BNF_PATH_HPP
#define BNF_PATH_HPP

#include "Map.hpp"
#include "../Rules/ARule.hpp"


class BNFPath
{
	public:
		const ARulePath &rulePath;
		BNFPath			*prev;
		Vec<BNFPath>	 next;
		bool			 isInvalidedPath;
		size_t			 index;
		Vec<ARulePath>	 wordStartPath;

		BNFPath(
			const ARulePath		 &p,
			const size_t		  idx,
			const Vec<ARulePath> &wordStartPath);
		BNFPath(
			const ARulePath		 &p,
			BNFPath *const		  pr,
			const size_t		  idx,
			const Vec<ARulePath> &wordStartPath);
		~BNFPath();




		static bool compare(const ARulePath &a, const ARulePath &b);

		BNFPath operator=(const BNFPath &other);
		bool	operator==(const BNFPath &other) const;

		const BNFPath *getHead() const;
		bool		   hasAllNextInvalided();

		void setNext(const ARulePath &p, size_t idx, Vec<ARulePath> wordStartPath);

		void deleteBranch();
};


#endif