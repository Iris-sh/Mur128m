#include "../include/mur128m.h"
#include "../include/machine_model.h"
struct Mur128M::Impl 
{
public:
    Impl()=default;
    Impl(const Mur128M &);//=default;
    ~Impl()=default;
    Impl(const parsing_result_t& image):
    image_{image}{}
    
    void set_image(const parsing_result_t & image)
    {
        image_ = image;
    }
    parsing_result_t get_image() const
    {
        return image_;
    }
    void run();
private:
    parsing_result_t image_;
    void convert_image_to_model();
    Machine_model model_;
};

Mur128M::Mur128M():pimpl_(std::make_shared<Impl>()){}

void Mur128M::set_image(const parsing_result_t& image)
{
    pimpl_->set_image(image);
}

parsing_result_t Mur128M::get_image()const
{
    return pimpl_->get_image();
}

void Mur128M::run()
{
    pimpl_->run();
}

void Mur128M::Impl::run()
{
    convert_image_to_model();
    model_.run();
}

void Mur128M::Impl::convert_image_to_model()
{
    Context ctx(
        image_.header_.begin_addr_ + image_.header_.ram_size_ - 1,
        image_.header_.begin_addr_ + image_.header_.volume_constants_ + image_.header_.variable_size_ - 1);
    Mem_block mb(image_.header_.begin_addr_, image_.header_.ram_size_, image_.content_);
    Memory_info mi;
    mi.block.push_back(mb);
    model_ = Machine_model(ctx, mi);
}
