#ifndef INDEXOF_HPP
#define INDEXOF_HPP

#include "Vec.hpp"
#include "Set.hpp"
#include <string>
#include "constants.hpp"

#define INDEXOF_TEMPLATE_LOCATION "{{ LOCATION }}"
#define INDEXOF_TEMPLATE_PATH "{{ PATH }}"
#define INDEXOF_TEMPLATE_ICON "{{ ICON }}"
#define INDEXOF_TEMPLATE_LINK "{{ LINK }}"
#define INDEXOF_TEMPLATE_NAME "{{ NAME }}"
#define INDEXOF_TEMPLATE_SIZE "{{ SIZE }}"
#define INDEXOF_TEMPLATE_SIZE_HUMAN "{{ SIZE_HUMAN }}"
#define INDEXOF_TEMPLATE_MODIFIED_HUMAN "{{ MODIFIED_HUMAN }}"
#define INDEXOF_TEMPLATE_CHANGED_HUMAN "{{ CHANGED_HUMAN }}"
#define INDEXOF_TEMPLATE_FILE_COUNT "{{ FILE_COUNT }}"
#define INDEXOF_TEMPLATE_SERVER_NAME "{{ SERVER_NAME }}"

#define INDEXOF_TEMPLATE_UPLOAD_FORM "{{ UPLOAD_FORM }}"
#define INDEXOF_TEMPLATE_EDIT_FORM "{{ EDIT_FORM }}"

#define INDEXOF_TEMPLATE_EDIT_BUTTON "{{ EDIT_BUTTON }}"
#define INDEXOF_TEMPLATE_DELETE_BUTTON "{{ DELETE_BUTTON }}"

#define INDEXOF_COL_NAME "Name"
#define INDEXOF_COL_SIZE "Size"
#define INDEXOF_COL_CHANGED "Changed"
#define INDEXOF_COL_MODIFIED "Modified"

#define INDEXOF_MAX_FETCH_SIZE_NB (2 * MB)
#define INDEXOF_MAX_FETCH_SIZE_STR TOSTRING(INDEXOF_MAX_FETCH_SIZE_NB)

class File;
class HttpMethod;

class IndexOf
{
	private:
		static const std::string HEAD_TEMPLATE;
		static const std::string LINE_TEMPLATE;
		static const std::string FOOT_TEMPLATE;

		static const std::string UPLOAD_FORM_TEMPLATE;
		static const std::string EDIT_FORM_TEMPLATE;

		static const std::string EDIT_BUTTON_TEMPLATE;
		static const std::string DELETE_BUTTON_TEMPLATE;

		std::string m_page;
		std::string m_path;
		std::string m_location;
		Vec<File>	m_files;

		bool m_hasPost;
		bool m_hasPut;
		bool m_hasDelete;

		std::string generateLine(
			const std::string &icon,
			const std::string &link,
			const std::string &name,
			const std::string &sizeHuman,
			const std::string &createdHuman,
			const std::string &dateHuman,
			const std::string &size);

		std::string generateHead();
		std::string generateFoot();



	public:
		IndexOf(
			const std::string	  &targetPath,
			const std::string	  &location,
			const Set<HttpMethod> *allowedMethods);
		~IndexOf();

		std::string		 getPage() const;
		const Vec<File> &getFiles() const;

		IndexOf &generate();
		IndexOf &scanDirectory();
};

#endif
