#include "http_manager.h"
#include <stdio.h>
#include <curl/curl.h>
#include <regex>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;



/* If you want run this program on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t fread_wrapper(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fread(ptr, size, nmemb, stream);
}

//And the same for writing ...
static size_t fwrite_wrapper(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	return fwrite(ptr, size, nmemb, stream);
}




http_manager::http_manager(std::string remote_url, std::string username, std::string password) :
	remote_url_{remote_url}, username_{username}, password_{password}
{
	validate_url();
}

void http_manager::get_file(std::string src_name, std::string dst_path)
{
	fs::path dest_file_path = fs::path(dst_path) / src_name;

	CURL *curl;
	CURLcode res;
	FILE *fd;

	//Open file to upload
	fd = fopen(dest_file_path.c_str(), "wb");
	if(!fd) {
		throw fm_exception("Cannot open file for writing. File: " + dest_file_path.string());
	}

	curl = curl_easy_init();
	if(curl) {
		//Destination URL
		curl_easy_setopt(curl, CURLOPT_URL, (remote_url_ + src_name).c_str());

		//Set where to write data to
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
		//Use custom write function (because of Windows DLL issue)
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fwrite_wrapper);

		//Follow redirects
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		//Ennable support for HTTP2
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
		//We have trusted HTTPS certificate, so set validation on
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
		//Throw exception on HTTP responses >= 400
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		//Set HTTP authentication
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, (username_ + ":" + password_).c_str());

		//Enable verbose for easier tracing
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		//Close file
		fclose(fd);

		//Check for errors
		if(res != CURLE_OK) {
			try {
				fs::remove(dest_file_path);
			} catch(...) {}
			auto error_message = std::string("Failed to download file. Error: ") + curl_easy_strerror(res);
			curl_easy_cleanup(curl);
			throw fm_exception(error_message);
		}

		//Always cleanup
		curl_easy_cleanup(curl);
	}
}

void http_manager::put_file(std::string name)
{
	fs::path source_file(name);
	std::string destination_url = remote_url_ + source_file.filename().string();
	CURL *curl;
	CURLcode res;
	FILE *fd;

	//Open file to upload
	fd = fopen(name.c_str(), "rb");
	if(!fd) {
		throw fm_exception("Cannot open file for reading. File: " + name);
	}

	//Get the file size
	auto filesize = fs::file_size(source_file);

	curl = curl_easy_init();
	if(curl) {
		//Destination URL
		curl_easy_setopt(curl, CURLOPT_URL, destination_url.c_str());

		//Upload mode
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		//Set where to read data from
		curl_easy_setopt(curl, CURLOPT_READDATA, fd);
		//Use custom read function (because of Windows DLL issue)
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, fread_wrapper);

		//Better give size of uploaded file
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)filesize);

		//Follow redirects
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		//Ennable support for HTTP2
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
		//We have trusted HTTPS certificate, so set validation on
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
		//Throw exception on HTTP responses >= 400
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		//Set HTTP authentication
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(curl, CURLOPT_USERPWD, (username_ + ":" + password_).c_str());

		//Enable verbose for easier tracing
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		//Close file
		fclose(fd);

		//Check for errors
		if(res != CURLE_OK) {
			throw fm_exception(std::string("Failed to upload file. Error: ") + curl_easy_strerror(res));
		}

		//Always cleanup
		curl_easy_cleanup(curl);
	}
}

void http_manager::set_data(std::string remote_url, std::string username, std::string password)
{
	remote_url_ = remote_url;
	username_ = username;
	password_ = password;
	validate_url();
}

void http_manager::validate_url()
{
	//'localhost' is also valid url, so now we don't check against regular expression
	//just make sure, that URL has trailing '/'
	if(remote_url_.size() > 0 && remote_url_[remote_url_.size() - 1] != '/') {
		remote_url_.push_back('/');
	}

	//Simple regular expression to validate inserted URL. Note use of raw string. \w <=> [_[:alnum:]]
	/*if(!std::regex_match(remote_url_, std::regex(R"(^https?://\w+(\.\w+)+(/\w+)*(:\d+)?/$)"))) {
		throw fm_exception("URL '" + remote_url_ + "' is not valid.");
	}*/
}

