
#ifndef CODEX_WORKER_FILE_MANAGER_BASE_H
#define CODEX_WORKER_FILE_MANAGER_BASE_H

#include <string>
#include <exception>


/**
 * Base class for file manager.
 * File manager can get you a copy of file to some directory or put a file somewhere.
 * For more info, see derived classes.
 */
class file_manager_base {
public:
    virtual ~file_manager_base() {}
    /**
     * Get the file.
     * @param src_name Name of the file to retrieve. Mostly this is sha1sum of the file.
     *                 No slashes are allowed.
     * @param dst_path Path to directory, where the file will be copied. Name of the file
     *                 will not change.
     */
    virtual void get_file(std::string src_name, std::string dst_path) = 0;
    /**
     * Put the file.
     * @param name Name of the file, which should be put somewhere. Possible use cases are
     *             insert this file to cache or send this file to remote server.
     */
    virtual void put_file(std::string name) = 0;
};



/**
 * Generic file manager exception.
 */
class fm_exception : public std::exception {
public:
	fm_exception() : what_{"Generic file manager exception"} {}
	fm_exception(std::string what) : what_{what} {}
	virtual ~fm_exception() {}
	virtual const char* what() const noexcept
	{
		return what_.c_str();
	}
protected:
	std::string what_;
};

/**
 * Exception for failed copy operation on files.
 */
class fm_copy_error : public fm_exception {
public:
	fm_copy_error() : fm_exception("Cannot copy file.") {}
	fm_copy_error(std::string what) : fm_exception(what) {}
	virtual ~fm_copy_error () {}
};

/**
 * Exception for failed directory creation.
 */
class fm_create_directory_error : public fm_exception {
public:
	fm_create_directory_error() : fm_exception("Cannot create directory.") {}
	fm_create_directory_error(std::string what) : fm_exception(what) {}
	virtual ~fm_create_directory_error() {}
};

/**
 * Exception for failed file creation.
 */
class fm_create_file_error : public fm_exception {
public:
	fm_create_file_error() : fm_exception("Cannot create file.") {}
	fm_create_file_error(std::string what) : fm_exception(what) {}
	virtual ~fm_create_file_error() {}
};

/**
 * Exception for connection error..
 */
class fm_connection_error : public fm_exception {
public:
	fm_connection_error() : fm_exception("Cannot access file on remote server.") {}
	fm_connection_error(std::string what) : fm_exception(what) {}
	virtual ~fm_connection_error() {}
};


#endif //CODEX_WORKER_FILE_MANAGER_BASE_H
