#ifndef MACHINE_MODEL_H
#define MACHINE_MODEL_H
#include "memory_info.h"
#include "operation.h"
#include "command_format.h"

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
        mi_ = mi;
    }
    Memory_info get_memory_info() const
    {
        return mi_;
    }
    void run()
    {
        printf("Мы тут выполняем\n");
        while(read_command())
        {
            continue;
            if(execute_command())
            {
            }
            else
            {
                // raise error
            }
        }
    };
    
    bool read_command()
    {
        //auto& b = mi_.block[0];
        //unsigned __int128 ba = b.begin_addr;
        //auto& content = b.content;
        //std::uint8_t* p = content.data();
        //uint32_t* com_ptr = (uint32_t*)p;
        //current_command_ = *com_ptr;
        
        //если мы выходим за границы памяти
        if(mi_.block[0].content.size() < (uint64_t)ctx_.int_ctx.ip + 4)
        {
            printf("mi_.block[0].content.size()=%lu ctx_.int_ctx.ip=%lu выход за границы памяти\n", (uint64_t)mi_.block[0].content.size(), (uint64_t)ctx_.int_ctx.ip + 4);
            return false;
        }
        // берем адрес элемента со смещением и интерпретируем память по нему как uint32_t
        current_command_ = *((uint32_t*)(&(mi_.block[0].content[ctx_.int_ctx.ip])));

        bool t = false;
        switch(current_command_>>28)
        {
            case AH:
                printf("Считана команда AH\n");
                t = ah_format();
                break;
/* пока работаем только с одним типом команд
            case BC:
                printf("Считана команда BC\n");
                //t = bc_format(current_command_);
                break;
            case DE:
                printf("Считана команда DE\n");
                //t = de_format(current_command_);
                break;
            case F:
                printf("Считана команда F\n");
                //t = f_format(current_command_);
                break;
            case G:
                printf("Считана команда G\n");
                //t = g_format(current_command_);
                break;
            case M:
                printf("Считана команда M\n");
                //t = m_format(current_command_);
                break;
*/
            default:
                printf("Этот формат команд не поддерживается\n");
                t = true; // добавлено в целях отладки, чтобы выполнение не прерывалось
                break;
        }
        ctx_.int_ctx.ip += 4;
        return t;
    };
    
    bool ah_format()
    {
        // берем из команды фрагменты, соответсвующие частям AH
        uint32_t op = (current_command_>>20) & 0b11'1111'1111;
        uint32_t imm = current_command_ & 0b1111'1111'1111'1111'1111;
        switch (op)
        {
            case jmpr:
                ctx_.int_ctx.ip = ctx_.int_ctx.ip + imm*4;
                printf("Выполнено jmpr\n");
                break;
            case ret:
                ctx_.int_ctx.ip = ctx_.float_ctx.sp;
                ctx_.float_ctx.sp += 16 + imm;
                printf("Выполнено ret\n");
                break;
            case reta:
                ctx_.int_ctx.ip = ctx_.float_ctx.sp;
                ctx_.float_ctx.sp += 16;
                ctx_.float_ctx.sp += ctx_.int_ctx.r[0];
                printf("Выполнено reta\n");
                break;
/* здесь используются другие команды, которые еще не имеют реализации
            case trap:
                //push r0-r31;
                //push (ctx_.int_ctx.ip);
                //ctx_.int_ctx.ip = (ctx_.int_ctx.r[0] & 0x3FF) * 16;
                break;
            case reti:
                // pop (ctx_.int_ctx.ip);
                //pop r0-r31;
                break;
*/
            default:
                printf("Этот тип операций не поддерживается\n");
                //return false; // закомменчено в целях отладки, чтобы выполнение не прерывалось
                break;
        }
        return true;
    }
    
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
                //uint32_t temp = command;
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

    bool execute (operation op, size_t imm)
    {
        switch (op)
        {
            case jmpr:
                ctx_.int_ctx.ip = ctx_.int_ctx.ip + imm*4;
                break;
            case ret:
                ctx_.int_ctx.ip = ctx_.float_ctx.sp;
                ctx_.float_ctx.sp += 16 + imm;
                break;
            case reta:
                ctx_.int_ctx.ip = ctx_.float_ctx.sp;
                ctx_.float_ctx.sp += 16;
                ctx_.float_ctx.sp += ctx_.int_ctx.r[0];
                break;
            case trap:
                //push r0-r31;
                //push (ctx_.int_ctx.ip);
                //ctx_.int_ctx.ip = (ctx_.int_ctx.r[0] & 0x3FF) * 16;
                break;
            case reti:
                // pop (ctx_.int_ctx.ip);
                //pop r0-r31;
                break;
            default:
                printf("Этот тип операций не поддерживается\n");
                break;
        }
        return true;
    };

    private:
        Context ctx_;
        Memory_info mi_;
        uint32_t current_command_;
};
#endif
