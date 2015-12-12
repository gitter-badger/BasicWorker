
#ifndef CODEX_WORKER_HTTP_MANAGER_H
#define CODEX_WORKER_HTTP_MANAGER_H

#include <string>
#include <set>
#include "file_manager_base.h"


class http_manager : public file_manager_base {
public:
    http_manager();
    virtual ~http_manager() {}
    virtual void get_file(std::string src_name, std::string dst_path);
    virtual void put_file(std::string name);
private:
    std::string save_dir_;
    std::set<std::string> index_;
};

#endif //CODEX_WORKER_HTTP_MANAGER_H
