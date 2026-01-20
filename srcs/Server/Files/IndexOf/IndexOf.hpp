#ifndef INDEXOF_HPP
#define INDEXOF_HPP

#include <string>
#include "Vec.hpp"

#include "File.hpp"

class IndexOf
{
	private:
		static const std::string HEAD_TEMPLATE;
		static const std::string LINE_TEMPLATE;
		static const std::string FOOT_TEMPLATE;

		std::string m_page;
		std::string m_path;
		Vec<File>	m_files;

		std::string generateLine(
			const std::string &icon,
			const std::string &link,
			const std::string &name,
			const std::string &sizeHuman,
			const std::string &createdHuman,
			const std::string &dateHuman);

		IndexOf &generate();
		IndexOf &scanDirectory(const std::string &path);

	public:
		IndexOf(const std::string &root, const std::string &path);
		~IndexOf();



		std::string		 getPage() const;
		const Vec<File> &getFiles() const;
};

#endif
