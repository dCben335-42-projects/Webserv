#ifndef ACCEPT_HPP
#define ACCEPT_HPP

#include <string>


class Accept
{
	private:
		std::string m_mediaType;
		float		m_qValue;

	public:
		Accept(const std::string &entry);
		~Accept();

		const std::string &getMediaType() const;
		float			   getQValue() const;


		static bool compare(const Accept &a, const Accept &b);
};


std::ostream &operator<<(std::ostream &o, const Accept &accept);

#endif