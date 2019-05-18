#ifndef MACHINE_MODEL_H
#define MACHINE_MODEL_H
#include "memory_info.h"
#include "operation.h"

constexpr unsigned num_of_float_regs=32;
constexpr unsigned num_of_int_regs=32;

struct Float_context 
{
    __float128 f[num_of_float_regs] = {0.0};
    __int128 sp = 0;
};

struct Int_context
{
    __int128 r[num_of_int_regs] = {0};
    __int128 ip = 0;
};

constexpr unsigned bp=30;
constexpr unsigned sp=31;

struct Context
{
    Float_context float_ctx;
    Int_context int_ctx;
    Context()=default;
    Context(__int128 sp, __int128 ip) {
        float_ctx = Float_context();
        float_ctx.sp = sp;
        int_ctx = Int_context();
        int_ctx.ip = ip;
    }
};

struct M_fmt{
    unsigned unused : 2;
    unsigned sf     : 3;
    unsigned ix     : 5;
    unsigned b      : 5;
    unsigned r      : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct G_fmt{
    unsigned r4     : 5;
    unsigned r3     : 5;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct F_fmt{
    unsigned unused : 5;
    unsigned r3     : 5;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct DE_fmt{
    unsigned imm    : 10;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct BC_fmt{
    unsigned imm    : 15;
    unsigned r      : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct AH_fmt{
    unsigned imm    : 20;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

class Machine_model
{
public:
    Machine_model()=default;
    Machine_model(const Machine_model&)=default;
    ~Machine_model()=default;
    
    void set_context(const Context& ctx)
    {
        ctx_ = ctx;
    }
    Context get_context() const
    {
        return ctx_;
    }
    void set_memory_info(const Memory_info& mi)
    {
        mi_=mi;
    }
    Memory_info get_memory_info() const
    {
        return mi_;
    }
    //void run(){}
    /*
    void run()
    {
        convert_image_to_model();
        for(;;)
        {
            bool t = read_and_execute();
            if(!t)
            {
                break;
            }
        }
        
    };
    */
    void run()
    {
        //convert_image_to_model();
        while(read_command())
        {
            if(execute_command())
            {
                ctx_.int_ctx.ip += 4;
            }
            else
            {
                // raise error
            }
        }
    };
    
    bool read_command()
    {
        auto& b = mi_.block[0];
        unsigned __int128 ba = b.begin_addr;
        auto& content = b.content;
        std::uint8_t* p = content.data();
        uint32_t* com_ptr = (uint32_t*)p;
        current_command_ = *com_ptr;

        bool t = false;        
        switch(current_command_>>28)
        {
            case AH:
                //t = ah_format(current_command_);
                break;
            case BC:
                //t = bc_format(current_command_);
                break;
            case DE:
                //t = de_format(current_command_);
                break;
            case F:
                //t = f_format(current_command_);
                break;
            case G:
                //t = g_format(current_command_);
                break;
            case M:
                //t = m_format(current_command_);
                break;
        }
        return t;
    };
    
    bool execute_command()
    {
        switch(current_command_>>30)
        {
            case 0:
                //non_integer_non_moving();
                break;
            case 1:
                //float_non_moving();
                break;
            case 2:
                //control_transfer();
                break;
            case 3:
                //moving();
                break;
            default:
                break;
        }
        return true;
    };
    
    bool read_and_execute()
    {
        auto& b=mi_.block[0];
        unsigned __int128 ba=b.begin_addr;
        unsigned __int128 ip=ctx_.int_ctx.ip;
        auto& content = b.content;
        size_t idx=ip-ba;//
        std::uint8_t*p=content.data();
        uint32_t* com_ptr=(uint32_t*)p;
        uint32_t command=*com_ptr;
        uint32_t command_type=command>>30;
        switch(command_type)
        {
            case 0:
                //non_integer_non_moving();
                break;
            case 1:
                //float_non_moving();
                break;
            case 2:
                //control_transfer();
                break;
            case 3:
                //moving();
                break;
            default:
                break;
        }
        return true;
    }
    
    enum class Arg_type
    {
        Register, Imm
    };
    
    struct Argument
    {
        Arg_type kind_ = Arg_type::Register;
        int32_t val_ = 0;
    };
    
    struct Args
    {
        Argument args_[4];
        int32_t num_of_args_;
    };
    
    enum Special_int_non_moving
    {
        DIVMODIU=0b1'0000,
        DIVMODIS=0b1'0001,
        NOT_REG=0b10'1000,
        NOT_IMM=0b10'1001
    };
   
    
    void non_integer_non_moving(uint32_t command)
    {
        uint32_t operation = (command>>20)& 0b11'1111'1111;
        Args arguments;
        switch(operation) {
            case DIVMODIU:
            case DIVMODIS:
            {
                arguments.num_of_args_ = 4;
                uint32_t temp = command;
//                 for(int i = 3; i >= 0; --i)
//                 {
//                     uint32_t reg = temp & 0b1'1111;
//                     temp >>= 5;
//                     Argument arg;
//                     arg.kind_ = Arg_type::Register;
//                     arg.val_ = reg;
//                     arguments.args_[i] = arg;
//                 };
                if((operation) == DIVMODIU) {
                    unsigned __int128 first = ctx_.int_ctx.r[arguments.args_[2].val_];
                    unsigned __int128 second = ctx_.int_ctx.r[arguments.args_[3].val_];
                    unsigned __int128 reg1 = first/second;
                    unsigned __int128 reg2 = first%second;  
                    ctx_.int_ctx.r[arguments.args_[0].val_] = reg1;
                    ctx_.int_ctx.r[arguments.args_[1].val_] = reg2;
                }
            }
            default:
                break;
        }
    }
    
    Machine_model(const Context& ctx, const Memory_info& mi)
    {
        ctx_ = ctx;
        mi_ = mi;
    }

enum FMT_kind
{
    AH, BC, DE, F, G, M
};
bool execute (operation op)
{
    switch (op)
        {
            case jmpr:
                //ip <- ip + imm*4;
            case ret:
                //ip <- *sp;
                //sp += 16 + imm;
            case reta:
                //ip <- *sp;
                //sp += 16;
                //sp += reg;
            case trap:
                //push r0-r31;
                //push ip;
                //ip <- (reg & 0x3FF) * 16;
            case reti:
                //pop ip;
                //pop r0-r31;
            default:
            break;
        
        }
    
};

    private:
        Context ctx_;
        Memory_info mi_;
        uint32_t current_command_;
};
#endif
