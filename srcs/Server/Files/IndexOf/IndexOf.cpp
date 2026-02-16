#include "IndexOf.hpp"
#include <dirent.h>
#include "Number.hpp"
#include "String.hpp"
#include "HttpMethod.hpp"
#include "File.hpp"
#include "webserv.hpp"


const std::string IndexOf::HEAD_TEMPLATE =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"<meta charset=\"UTF-8\">\n"
	"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
	"<title>Index of" INDEXOF_TEMPLATE_LOCATION "</title>\n"
	"<style>\n"
	"body {\n"
	"  font-family: system-ui, sans-serif;\n"
	"  margin: 2rem;\n"
	"  background: #f7f9fb;\n"
	"  color: #333;\n"
	"}\n"
	"h1 {\n"
	"  font-size: 1.6rem;\n"
	"  margin-bottom: 0.25rem;\n"
	"  color: #222;\n"
	"}\n"
	".path {\n"
	"  font-family: monospace;\n"
	"  color: #666;\n"
	"  margin-bottom: 1.5rem;\n"
	"}\n"
	".upload-box {\n"
	"  margin-bottom: 1.5rem;\n"
	"  padding: 1rem;\n"
	"  background: #fff;\n"
	"  border-radius: 8px;\n"
	"  box-shadow: 0 1px 4px rgba(0,0,0,0.08);\n"
	"}\n"
	".btn {\n"
	"  padding: 0.35rem 0.6rem;\n"
	"  font-size: 0.85rem;\n"
	"  border-radius: 4px;\n"
	"  border: none;\n"
	"  cursor: pointer;\n"
	"  transition: background 0.2s ease,\n"
	"				filter 0.2s ease-in-out;\n"
	"}\n"
	".btn:hover { filter: brightness(0.9); }\n"
	".btn-upload {\n"
	"  background: #0066b8;\n"
	"  color: #fff;\n"
	"}\n"
	".btn-upload:hover { background: #004f8c; }\n"
	".btn-delete {\n"
	"  background: #e5533d;\n"
	"  color: #fff;\n"
	"}\n"
	".btn-delete:hover { background: #c43c28; }\n"
	".actions { display: flex; gap: 0.5rem; }\n"
	"table {\n"
	"  border-collapse: collapse;\n"
	"  width: 100%;\n"
	"  background: #fff;\n"
	"  border-radius: 8px;\n"
	"  overflow: hidden;\n"
	"  box-shadow: 0 1px 4px rgba(0,0,0,0.08);\n"
	"}\n"
	"th, td { padding: 0.6rem 0.8rem; }\n"
	"th {\n"
	"  text-align: left;\n"
	"  border-bottom: 2px solid #e3e3e3;\n"
	"  background: #fafafa;\n"
	"}\n"
	"tr:hover { background: #eef6ff; }\n"
	".size, .date { text-align: right; color: #555; }\n"
	".icon { width: 1.6rem; text-align: center; }\n"
	".editbox {\n"
	"  display: grid;\n"
	"  gap: 0.5rem;\n"
	"  margin-top: 1.5rem;\n"
	"  padding: 1rem;\n"
	"  background: #fff;\n"
	"  border-radius: 8px;\n"
	"  box-shadow: 0 1px 4px rgba(0,0,0,0.08);\n"
	"}\n"
	".editbox textarea {\n"
	"  width: 100%;\n"
	"  box-sizing: border-box;\n"
	"  min-height: 500px;\n"
	"  padding: 0.5rem;\n"
	"  font-family: monospace;\n"
	"  font-size: 0.9rem;\n"
	"  border: 1px solid #ddd;\n"
	"  border-radius: 4px;\n"
	"  resize: vertical;\n"
	"}\n"
	".footer {\n"
	"  margin-top: 1.5rem;\n"
	"  font-size: 0.85rem;\n"
	"  color: #777;\n"
	"  text-align: center;\n"
	"  padding-top: 0.5rem;\n"
	"  border-top: 1px solid #ddd;\n"
	"}\n"
	".hidden {\n"
	"  display: none;\n"
	"}\n"
	"</style>\n"
	"<script>\n"
	"function uploadFile() {\n"
	"  const input = document.getElementById('fileInput');\n"
	"  if (!input.files.length) return alert('Select a file');\n"
	"  const data = new FormData();\n"
	"  data.append('file', input.files[0]);\n"
	"  fetch('/uploads', { method: 'POST', body: data })\n"
	"    .then(r => r.ok ? location.reload() : alert('Upload failed'));\n"
	"}\n"
	"function deleteFile(path) {\n"
	"  if (!confirm('Delete ' + path + ' ?')) return;\n"
	"  if (path == '../') path = location.pathname;\n"
	"  fetch(path, { method: 'DELETE' })\n"
	"    .then(r => r.ok ? console.log(r) : alert('Delete failed'));\n"
	"}\n"
	"function fetchFile(path, size) {\n"
	"  if (size > " INDEXOF_MAX_FETCH_SIZE_STR ")\n"
	"    return alert('File too large to edit');\n"
	"  const editbox = document.querySelector('.editbox');\n"
	"  const textarea = document.getElementById('editfile');\n"
	"  fetch(path, { method: 'GET' })\n"
	"    .then(r => r.text())"
	"    .then(content => {\n"
	"  textarea.textContent = content;\n"
	"  textarea.name = path;\n"
	"  editbox.classList.remove('hidden');\n"
	"});\n"
	"}\n"
	"function editFile() {\n"
	"  const textarea = document.getElementById('editfile');\n"
	"  fetch(textarea.name, { method: 'PUT', body: textarea.value })\n"
	"    .then(r => r.ok ? location.reload() : alert('Edit failed'));\n"
	"}\n"
	"</script>\n"
	"</head>\n"
	"<body>\n"
	"<h1>Index of" INDEXOF_TEMPLATE_LOCATION "</h1>\n"
	"<div class=\"path\">" INDEXOF_TEMPLATE_PATH "</div>\n"
	"" INDEXOF_TEMPLATE_UPLOAD_FORM
	"<table>\n"
	"<tr>"
	"<th></th>"
	"<th>" INDEXOF_COL_NAME "</th>"
	"<th class=\"size\">" INDEXOF_COL_SIZE "</th>"
	"<th class=\"date\">" INDEXOF_COL_CHANGED "</th>"
	"<th class=\"date\">" INDEXOF_COL_MODIFIED "</th>"
	"<th>Actions</th>"
	"</tr>\n";

const std::string IndexOf::LINE_TEMPLATE =
	"<tr>"
	"<td class=\"icon\">" INDEXOF_TEMPLATE_ICON "</td>"
	"<td><a href=\"" INDEXOF_TEMPLATE_LINK "\">" INDEXOF_TEMPLATE_NAME "</a></td>"
	"<td class=\"size\">" INDEXOF_TEMPLATE_SIZE_HUMAN "</td>"
	"<td class=\"date\">" INDEXOF_TEMPLATE_CHANGED_HUMAN "</td>"
	"<td class=\"date\">" INDEXOF_TEMPLATE_MODIFIED_HUMAN "</td>"
	"<td class=\"actions\">" INDEXOF_TEMPLATE_DELETE_BUTTON INDEXOF_TEMPLATE_EDIT_BUTTON "</td>"
	"</tr>\n";

const std::string IndexOf::EDIT_BUTTON_TEMPLATE =
	"<button class=\"btn btn-edit\" onclick=\"fetchFile('" INDEXOF_TEMPLATE_LINK "', " INDEXOF_TEMPLATE_SIZE ")\">Edit</button>";

const std::string IndexOf::DELETE_BUTTON_TEMPLATE =
	"<button class=\"btn btn-delete\" onclick=\"deleteFile('" INDEXOF_TEMPLATE_LINK "')\">Delete</button>";

const std::string IndexOf::UPLOAD_FORM_TEMPLATE =
	"<div class=\"upload-box\">\n"
	"  <input type=\"file\" id=\"fileInput\">\n"
	"  <button class=\"btn btn-upload\" onclick=\"uploadFile()\">Upload</button>\n"
	"</div>\n";

const std::string IndexOf::EDIT_FORM_TEMPLATE =
	"<div class='editbox hidden'>"
	"<textarea id='editfile' name='editfile' rows='4' cols='50'></textarea>"
	"<button class=\"btn btn-submit\" onclick=\"editFile()\">Edit</button>"
	"</div>";

const std::string IndexOf::FOOT_TEMPLATE =
	"</table>\n"
	"" INDEXOF_TEMPLATE_EDIT_FORM
	"<div class=\"footer\">" INDEXOF_TEMPLATE_FILE_COUNT " items &middot; Generated by " INDEXOF_TEMPLATE_SERVER_NAME "</div>\n"
	"</body>\n"
	"</html>\n";




IndexOf::IndexOf(
	const std::string	  &targetPath,
	const std::string	  &location,
	const Set<HttpMethod> *allowedMethods)
{
	m_path		= targetPath;
	m_location	= location;
	m_hasPost	= !allowedMethods || allowedMethods->contains(HttpMethod::POST);
	m_hasPut	= !allowedMethods || allowedMethods->contains(HttpMethod::PUT);
	m_hasDelete = !allowedMethods || allowedMethods->contains(HttpMethod::DELETE);
}

IndexOf::~IndexOf()
{
}

/**
 *
 * @brief Scans the directory at m_path and populates m_files with File objects.
 *
 * @throws std::runtime_error if the directory cannot be opened.
 *
 * @return IndexOf& A reference to the current IndexOf object
 * for method chaining.
 *
 */
IndexOf &IndexOf::scanDirectory()
{
	std::string path = m_path;
	DIR		   *dir	 = opendir(path.c_str());
	if (!dir)
		throw std::runtime_error("Failed to open directory: " + path);

	struct dirent *entry;
	std::string	   fullPath;
	std::string	   entryName;

	while ((entry = readdir(dir)) != NULL)
	{
		entryName = entry->d_name;
		if (entryName == "." || entryName == "..")
			continue;
		m_files.push_back(File(entryName, m_path + "/" + entryName, m_location));
	}
	closedir(dir);


	return *this;
}

/**
 *
 * @brief Generates a single line of the index page.
 *
 * @param icon The icon to be displayed.
 * @param link The link to the file or directory.
 * @param name The name of the file or directory.
 * @param sizeHuman The human-readable size of the file.
 * @param dateHuman The human-readable last modified date of the file.
 *
 * @note This function uses the LINE_TEMPLATE constant to format the line.
 * @see LINE_TEMPLATE
 *
 * @return std::string The generated line.
 */
std::string IndexOf::generateLine(
	const std::string &icon,
	const std::string &link,
	const std::string &name,
	const std::string &sizeHuman,
	const std::string &createdHuman,
	const std::string &dateHuman,
	const std::string &size)
{
	std::string line = LINE_TEMPLATE;

	String::replaceAll(line, INDEXOF_TEMPLATE_ICON, icon);
	String::replaceAll(line, INDEXOF_TEMPLATE_LINK, link);
	String::replaceAll(line, INDEXOF_TEMPLATE_NAME, name);
	String::replaceAll(line, INDEXOF_TEMPLATE_CHANGED_HUMAN, createdHuman);
	String::replaceAll(line, INDEXOF_TEMPLATE_SIZE_HUMAN, sizeHuman);
	String::replaceAll(line, INDEXOF_TEMPLATE_MODIFIED_HUMAN, dateHuman);
	if (name != "..")
	{
		std::string editBtn = m_hasPut ? EDIT_BUTTON_TEMPLATE : "";
		if (m_hasPut)
		{
			String::replaceAll(editBtn, INDEXOF_TEMPLATE_SIZE, size);
			String::replaceAll(editBtn, INDEXOF_TEMPLATE_LINK, link);
		}
		String::replaceAll(line, INDEXOF_TEMPLATE_EDIT_BUTTON, editBtn);

		std::string deleteBtn = m_hasDelete ? DELETE_BUTTON_TEMPLATE : "";
		if (m_hasDelete)
			String::replaceAll(deleteBtn, INDEXOF_TEMPLATE_LINK, link);
		String::replaceAll(line, INDEXOF_TEMPLATE_DELETE_BUTTON, deleteBtn);
	}
	else
	{
		String::replaceAll(line, INDEXOF_TEMPLATE_EDIT_BUTTON, "");
		String::replaceAll(line, INDEXOF_TEMPLATE_DELETE_BUTTON, "");
	}
	return line;
}


/**
 *
 * @brief Generates the head section of the index page.
 *
 * @note This function uses the HEAD_TEMPLATE constant to format the head section.
 * @see HEAD_TEMPLATE
 *
 * @return std::string The generated head section.
 *
 */
std::string IndexOf::generateHead()
{
	std::string		  head		 = HEAD_TEMPLATE;
	const std::string uploadForm = m_hasPost ? UPLOAD_FORM_TEMPLATE : "";

	String::replaceAll(head, INDEXOF_TEMPLATE_LOCATION, String::htmlEscape(m_location));
	String::replaceAll(head, INDEXOF_TEMPLATE_UPLOAD_FORM, uploadForm);
	String::replaceAll(head, INDEXOF_TEMPLATE_PATH, String::htmlEscape(m_path));
	return head;
}


/**
 *
 * @brief Generates the foot section of the index page.
 *
 * @note This function uses the FOOT_TEMPLATE constant to format the foot section.
 * @see FOOT_TEMPLATE
 *
 * @return std::string The generated foot section.
 *
 */
std::string IndexOf::generateFoot()
{
	std::string		  foot	   = FOOT_TEMPLATE;
	const std::string editForm = m_hasPut ? EDIT_FORM_TEMPLATE : "";

	String::replaceAll(foot, INDEXOF_TEMPLATE_EDIT_FORM, editForm);
	String::replaceAll(foot, INDEXOF_TEMPLATE_FILE_COUNT, Number::toString(m_files.size()));
	String::replaceAll(foot, INDEXOF_TEMPLATE_SERVER_NAME, SERVER_NAME);
	return foot;
}

/**
 *
 * @brief Generates the complete index page.
 *
 * @note This function uses the HEAD_TEMPLATE and FOOT_TEMPLATE constants to format the page.
 * @see HEAD_TEMPLATE
 * @see FOOT_TEMPLATE
 *
 * @note It also calls generateLine() for each file in the directory to create the individual lines.
 * @see generateLine()
 *
 * @return IndexOf& A reference to the current IndexOf object
 * for method chaining.
 *
 */
IndexOf &IndexOf::generate()
{
	m_page = generateHead();

	if (m_location != "/")
		m_page += generateLine("📁", "../", "..", "-", "-", "-", "");
	for (Vec<File>::const_iterator it = m_files.begin(); it != m_files.end(); ++it)
	{
		m_page += generateLine(
			it->getIcon(),
			it->getUri(),
			it->getName(),
			it->getSizeHuman(),
			it->getCreatedAtHuman(),
			it->getLastModifiedHuman(),
			Number::toString(it->getSize()));
	}
	m_page += generateFoot();
	return *this;
}


/**
 *
 * @brief Gets the generated index page.
 *
 * @return std::string The generated index page.
 *
 */
std::string IndexOf::getPage() const
{
	return m_page;
}

/**
 *
 * @brief Gets the list of files in the directory.
 *s
 * @return const Vec<File>& A constant reference to the vector of File objects.
 *
 */
const Vec<File> &IndexOf::getFiles() const
{
	return m_files;
}