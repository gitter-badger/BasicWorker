
#ifndef CODEX_WORKER_FILE_MANAGER_H
#define CODEX_WORKER_FILE_MANAGER_H

#include "file_manager_base.h"
#include "cache_manager.h"
#include "http_manager.h"

class file_manager : public file_manager_base {
public:
    file_manager();
    virtual ~file_manager() {}
    virtual void get_file(std::string src_name, std::string dst_path);
    virtual void put_file(std::string name);
private:
    cache_manager cache_man_;
    http_manager http_man_;
};

#endif //CODEX_WORKER_FILE_MANAGER_H
