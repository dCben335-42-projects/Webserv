#ifndef UPLOAD_HPP
#define UPLOAD_HPP

#include <string>

class Upload
{
	private:
		bool		m_isOn;
		std::string m_uploadDir;

	public:
		Upload();
		Upload(const Upload &other);

		Upload &operator=(const Upload &other);

		const std::string &getUploadDir() const;
		bool			   isOn() const;

		void setUploadDir(const std::string &uploadDir);
		void setOn(bool isOn);
};

std::ostream &operator<<(std::ostream &os, const Upload &upload);

#endif