#ifndef MUR128M_H
#include <memory>
#include "../include/parsing_result_t.h"
struct Mur128M
{
public:
    Mur128M();
    Mur128M(const Mur128M &)=default;
    ~Mur128M()=default;
    Mur128M(const parsing_result_t& image);
    void set_image(const parsing_result_t& image);
    parsing_result_t get_image() const;
    void run();
private:
    struct Impl;
    std::shared_ptr<Impl> pimpl_;
};
#endif
