#include "disasm.hh"

#define MODRM_RM(byte)  ((u8)byte & 0b111)
#define MODRM_REG(byte) (((u8)byte >> 3) & 0b111)
#define MODRM_MOD(byte) (((u8)byte >> 6) & 0b11)

#define SIB_BASE(byte)  ((u8)byte & 0b111)
#define SIB_INDEX(byte) (((u8)byte >> 3) & 0b111)
#define SIB_SCALE(byte) (((u8)byte >> 6) & 0b11)

/* TODO:
*  error on bad operand size if only one size allowed
*  3 byte opcodes
*  add some missing ring 3 instructions
*  support x87, sse, avx instructions?
*  mark default sregs (pop ds etc)
*  add flag for mem [addr] instead of mem [reg]
*/

using enum disasm::operand_type;

namespace disasm {

struct decoder {
    bool parse_prefixes(u32& pref);
    void parse_modrm(insn& ins, u8& reg_op, u8& reg_op2, u8 byte);
    void parse_imm_args(operand& op, bool op_16b);

    inline void advance(u32 n = 1)
    {
        code += n;
        pos += n;
    }

    u8* code{};
    u32 pos{};
};

constexpr static auto no_mod(u8 b1, u8 b2, operand_type t1 = none, operand_type t2 = none)
{
    return insn(b1, b2, 0, t1, t2, none, insn_no_mod);
}

constexpr static auto reg_ext(u8 b1, u8 b2, operand_type t1 = none, operand_type t2 = none,
    operand_type t3 = none)
{
    return insn(b1, b2, 0, t1, t2, t3, insn_reg_ext);
}

constexpr static auto digit(u8 b1, u8 b2, u32 d, operand_type t1 = none, operand_type t2 = none)
{
    return insn(b1, b2, 0, t1, t2, none, d);
}

/* Instructions without a modrm byte. */
static const insn no_mod_tbl[]{
    no_mod(/* "aaa", */ 0x37, 0x00),                           // aaa
    no_mod(/* "aad", */ 0xd5, 0x0a),                           // aad
    no_mod(/* "aam", */ 0xd4, 0x0a),                           // aam
    no_mod(/* "aas", */ 0x3f, 0x00),                           // aas
    no_mod(/* "adc", */ 0x14, 0x00, al, imm8),                 // adc al, imm8
    no_mod(/* "adc", */ 0x15, 0x00, eax, imm16_32),            // adc (e)ax, imm{16,32}
    no_mod(/* "add", */ 0x04, 0x00, al, imm8),                 // add al, imm8
    no_mod(/* "add", */ 0x05, 0x00, eax, imm16_32),            // add (e)ax, imm{16,32}
    no_mod(/* "and", */ 0x24, 0x00, al, imm8),                 // and al, imm8
    no_mod(/* "and", */ 0x25, 0x00, eax, imm16_32),            // and (e)ax, imm{16,32}
    no_mod(/* "bswap", */ 0x0f, 0xc8, builtin32),              // bswap reg32
    no_mod(/* "call", */ 0xe8, 0x00, imm16_32),                // call near rel{16,32}
    no_mod(/* "cwde", */ 0x98, 0x00),                          // cwde
    no_mod(/* "cdq", */ 0x99, 0x00),                           // cdq
    no_mod(/* "clc", */ 0xf8, 0x00),                           // clc
    no_mod(/* "cld", */ 0xfc, 0x00),                           // cld
    no_mod(/* "cmc", */ 0xf5, 0x00),                           // cmc
    no_mod(/* "cmp", */ 0x3c, 0x00, al, imm8),                 // cmp al, imm8
    no_mod(/* "cmp", */ 0x3d, 0x00, eax, imm16_32),            // cmp (e)ax, imm{16,32}
    no_mod(/* "cmpsb", */ 0xa6, 0x00),                         // cmpsb
    no_mod(/* "cmpsd", */ 0xa7, 0x00),                         // cmps{d,w}
    no_mod(/* "cpuid", */ 0x0f, 0xa2),                         // cpuid
    no_mod(/* "daa", */ 0x27, 0x00),                           // daa
    no_mod(/* "das", */ 0x2f, 0x00),                           // das
    no_mod(/* "dec", */ 0x48, 0x00, builtin16_32),             // dec reg{16,32}
    no_mod(/* "in", */ 0xe4, 0x00, al, imm8),                  // in al, imm8
    no_mod(/* "in", */ 0xe5, 0x00, eax, imm16_32),             // in (e)ax, imm{16,32}
    no_mod(/* "in", */ 0xec, 0x00, al, dx),                    // in al, dx
    no_mod(/* "in", */ 0xed, 0x00, eax, dx),                   // in (e)ax, dx
    no_mod(/* "inc", */ 0x40, 0x00, builtin16_32),             // inc reg{16,32}
    no_mod(/* "insb", */ 0x6c, 0x00),                          // ins [edi], dx
    no_mod(/* "insd", */ 0x6d, 0x00),                          // ins{d,w} [edi], dx
    no_mod(/* "int", */ 0xcd, 0x00, imm8),                     // int imm8
    no_mod(/* "int 3", */ 0xcc, 0x00),                         // int 3
    no_mod(/* "into", */ 0xce, 0x00),                          // into
    no_mod(/* "jo", */ 0x70, 0x00, imm8),                      // jo rel8off
    no_mod(/* "jo", */ 0x0f, 0x80, imm16_32),                  // jo rel{16,32}off
    no_mod(/* "jno", */ 0x71, 0x00, imm8),                     // jno rel8off
    no_mod(/* "jno", */ 0x0f, 0x81, imm16_32),                 // jno rel{16,32}off
    no_mod(/* "jb", */ 0x72, 0x00, imm8),                      // jb/jc/jnae rel8off
    no_mod(/* "jb", */ 0x0f, 0x82, imm16_32),                  // jb/jc/jnae rel{16,32}off
    no_mod(/* "jae", */ 0x73, 0x00, imm8),                     // jae/jnb/jnc rel8off
    no_mod(/* "jae", */ 0x0f, 0x83, imm16_32),                 // jae/jnb/jnc rel{16,32}off
    no_mod(/* "je", */ 0x74, 0x00, imm8),                      // je/jz rel8off
    no_mod(/* "je", */ 0x0f, 0x84, imm16_32),                  // je/jz rel{16,32}off
    no_mod(/* "jne", */ 0x75, 0x00, imm8),                     // jne/jnz rel8off
    no_mod(/* "jne", */ 0x0f, 0x85, imm16_32),                 // jne/jnz rel{16,32}off
    no_mod(/* "jbe", */ 0x76, 0x00, imm8),                     // jbe rel8off
    no_mod(/* "jbe", */ 0x0f, 0x86, imm16_32),                 // jbe rel{16,32}off
    no_mod(/* "ja", */ 0x77, 0x00, imm8),                      // ja/jnbe rel8off
    no_mod(/* "ja", */ 0x0f, 0x87, imm16_32),                  // ja/jnbe rel{16,32}off
    no_mod(/* "js", */ 0x78, 0x00, imm8),                      // js rel8off
    no_mod(/* "js", */ 0x0f, 0x88, imm16_32),                  // js rel{16,32}off
    no_mod(/* "jns", */ 0x79, 0x00, imm8),                     // jns rel8off
    no_mod(/* "jns", */ 0x0f, 0x89, imm16_32),                 // jns rel{16,32}off
    no_mod(/* "jp", */ 0x7a, 0x00, imm8),                      // jp/jpe rel8off
    no_mod(/* "jp", */ 0x0f, 0x8a, imm16_32),                  // jp/jpe rel{16,32}off
    no_mod(/* "jnp", */ 0x7b, 0x00, imm8),                     // jnp/jpo rel8off
    no_mod(/* "jnp", */ 0x0f, 0x8b, imm16_32),                 // jnp/jpo rel{16,32}off
    no_mod(/* "jl", */ 0x7c, 0x00, imm8),                      // jl/jnge rel8off
    no_mod(/* "jl", */ 0x0f, 0x8c, imm16_32),                  // jl/jnge rel{16,32}off
    no_mod(/* "jge", */ 0x7d, 0x00, imm8),                     // jge/jnl rel8off
    no_mod(/* "jge", */ 0x0f, 0x8d, imm16_32),                 // jge/jnl rel{16,32}off
    no_mod(/* "jle", */ 0x7e, 0x00, imm8),                     // jle/jng rel8off
    no_mod(/* "jle", */ 0x0f, 0x8e, imm16_32),                 // jle/jng rel{16,32}off
    no_mod(/* "jg", */ 0x7f, 0x00, imm8),                      // jg/jnle rel8off
    no_mod(/* "jg", */ 0x0f, 0x8f, imm16_32),                  // jg/jnle rel{16,32}off
    no_mod(/* "jmp", */ 0xeb, 0x00, imm8),                     // jmp near rel8off
    no_mod(/* "jmp", */ 0xe9, 0x00, imm16_32),                 // jmp near rel{16,32}off
    no_mod(/* "lahf", */ 0x9f, 0x00),                          // lahf
    no_mod(/* "leave", */ 0xc9, 0x00),                         // leave
    no_mod(/* "lodsb", */ 0xac, 0x00, mem8),                   // lods mem8
    no_mod(/* "lodsd", */ 0xad, 0x00, mem16_32),               // lods{d,w} mem{16,32}
    no_mod(/* "mov", */ 0xa0, 0x00, al, m_off),                // mov al, moffset8
    no_mod(/* "mov", */ 0xa1, 0x00, eax, m_off),               // mov (e)ax, moffset{16,32}
    no_mod(/* "mov", */ 0xa2, 0x00, m_off, al),                // mov moffset8, al
    no_mod(/* "mov", */ 0xa3, 0x00, m_off, eax),               // mov moffset{16,32}, (e)ax
    no_mod(/* "mov", */ 0xb0, 0x00, builtin8, imm8),           // mov reg8, imm8
    no_mod(/* "mov", */ 0xb8, 0x00, builtin16_32, imm16_32),   // mov reg{16,32}, imm{16,32}
    no_mod(/* "movsb", */ 0xa4, 0x00, mem8, mem8),             // movsb mem8, mem8
    no_mod(/* "movsd", */ 0xa5, 0x00),                         // movs{d,w} mem{16,32}, mem{16,32}
    no_mod(/* "nop", */ 0x90, 0x00),                           // nop
    no_mod(/* "or", */ 0x0c, 0x00, al, imm8),                  // or al, imm8
    no_mod(/* "or", */ 0x0d, 0x00, eax, imm16_32),             // or (e)ax, imm{16,32}
    no_mod(/* "out", */ 0xe6, 0x00, imm8, al),                 // out imm8, al
    no_mod(/* "out", */ 0xe7, 0x00, imm8, eax),                // out imm8, (e)ax
    no_mod(/* "out", */ 0xee, 0x00, dx, al),                   // out dx, al
    no_mod(/* "out", */ 0xef, 0x00, dx, eax),                  // out dx, (e)ax
    no_mod(/* "outsb", */ 0x6e, 0x00),                         // outsb
    no_mod(/* "outsd", */ 0x6f, 0x00),                         // outs{d,w}
    no_mod(/* "pause", */ 0xf3, 0x90),                         // pause
    no_mod(/* "pop", */ 0x58, 0x00, builtin16_32),             // pop reg{16,32}
    no_mod(/* "pop ds", */ 0x1f, 0x00),                        // pop ds
    no_mod(/* "pop es", */ 0x07, 0x00),                        // pop es
    no_mod(/* "pop ss", */ 0x17, 0x00),                        // pop ss
    no_mod(/* "pop fs", */ 0x0f, 0xa1),                        // pop fs
    no_mod(/* "pop gs", */ 0x0f, 0xa9),                        // pop gs
    no_mod(/* "popad", */ 0x61, 0xa9),                         // popa(d)
    no_mod(/* "popfd", */ 0x9d, 0x00),                         // popf(d)
    no_mod(/* "push", */ 0x50, 0x00, builtin16_32),            // push reg{16,32}
    no_mod(/* "push", */ 0x6a, 0x00, imm8),                    // push imm8
    no_mod(/* "push", */ 0x68, 0x00, imm16_32),                // push imm{16,32}
    no_mod(/* "push cs", */ 0x0e, 0x00),                       // push cs
    no_mod(/* "push ss", */ 0x16, 0x00),                       // push ss
    no_mod(/* "push ds", */ 0x1e, 0x00),                       // push ds
    no_mod(/* "push es", */ 0x06, 0x00),                       // push es
    no_mod(/* "push fs", */ 0x0f, 0xa0),                       // push fs
    no_mod(/* "push gs", */ 0x0f, 0xa8),                       // push gs
    no_mod(/* "pushad", */ 0x60, 0x00),                        // pushad/pusha
    no_mod(/* "pushfd", */ 0x9c, 0x00),                        // pushfd/pushf
    no_mod(/* "rdtsc", */ 0x0f, 0x31),                         // rdtsc
    no_mod(/* "ret", */ 0xc3, 0x00),                           // ret
    no_mod(/* "ret", */ 0xc2, 0x00, imm16),                    // ret imm16
    no_mod(/* "retf", */ 0xcb, 0x00),                          // retf
    no_mod(/* "retf", */ 0xca, 0x00, imm16),                   // retf imm16
    no_mod(/* "sahf", */ 0x9e, 0x00),                          // sahf
    no_mod(/* "sal", */ 0x1c, 0x00, al, imm8),                 // sbb al, imm8
    no_mod(/* "sbb", */ 0x1d, 0x00, eax, imm8),                // sbb (e)ax, imm{16,32}
    no_mod(/* "scasb", */ 0xae, 0x00),                         // scasb
    no_mod(/* "scasd", */ 0xaf, 0x00),                         // scas{d,w}
    no_mod(/* "stc", */ 0xf9, 0x00),                           // stc
    no_mod(/* "std", */ 0xfd, 0x00),                           // std
    no_mod(/* "stosb", */ 0xaa, 0x00),                         // stosb
    no_mod(/* "stosd", */ 0xab, 0x00),                         // stos{d,w}
    no_mod(/* "sub", */ 0x2c, 0x00, al, imm8),                 // sub al, imm8
    no_mod(/* "sub", */ 0x2d, 0x00, eax, imm16_32),            // sub (e)ax, imm{16,32}
    no_mod(/* "test", */ 0xa8, 0x00, al, imm8),                // test al, imm8
    no_mod(/* "test", */ 0xa9, 0x00, eax, imm16_32),           // test (e)ax, imm{16,32}
    no_mod(/* "ud0", */ 0x0f, 0xff),                           // ud0
    no_mod(/* "ud2", */ 0x0f, 0x0b),                           // ud2
    no_mod(/* "xchg", */ 0x90, 0x00, eax, reg16_32),           // xchg (e)ax, reg{16,32}
    no_mod(/* "xlatb", */ 0xd7, 0x00),                         // xlatb
    no_mod(/* "xor", */ 0x34, 0x00, al, imm8),                 // xor al, imm8
    no_mod(/* "xor", */ 0x35, 0x00, eax, imm16_32),            // xor al, imm{16,32}
};

/* /0-7 instructions. */
static const insn ext_tbl[]{
    digit(/* "adc", */ 0x80, 0x00, 2, rm8, imm8),             // adc r/m8, imm8
    digit(/* "adc", */ 0x81, 0x00, 2, imm16_32, imm16_32),    // adc r/m{16,32}, imm{16,32}
    digit(/* "adc", */ 0x83, 0x00, 2, rm16_32, imm8),         // adc r/m{16,32}, imm8
    digit(/* "add", */ 0x80, 0x00, 0, rm8, imm8),             // add r/m8, imm8
    digit(/* "add", */ 0x81, 0x00, 0, rm16_32, imm16_32),     // add r/m{16,32}, imm{16,32}
    digit(/* "add", */ 0x83, 0x00, 0, rm16_32, imm8),         // add r/m{16,32}, imm8
    digit(/* "and", */ 0x80, 0x00, 4, rm8, imm8),             // and r/m8, imm8
    digit(/* "and", */ 0x81, 0x00, 4, rm16_32, imm16_32),     // and r/m{16,32}, imm{16,32}
    digit(/* "and", */ 0x83, 0x00, 4, rm16_32, imm8),         // and r/m{16,32}, imm8
    digit(/* "bt", */ 0x0f, 0xba, 4, rm16_32, imm8),          // bt r/m{16,32}, imm8
    digit(/* "btc", */ 0x0f, 0xba, 7, rm16_32, imm8),         // bts r/m{16,32}, imm8
    digit(/* "btr", */ 0x0f, 0xba, 6, rm16_32, imm8),         // btr r/m{16,32}, imm8
    digit(/* "bts", */ 0x0f, 0xba, 5, rm16_32, imm8),         // bts r/m{16,32}, imm8
    digit(/* "call", */ 0xff, 0x00, 2, rm16_32),              // call near r/m{16,32}
    digit(/* "cmp", */ 0x80, 0x00, 7, rm8, imm8),             // cmp r/m8, imm8
    digit(/* "cmp", */ 0x81, 0x00, 7, rm16_32, imm16_32),     // cmp r/m{16,32}, imm{16,32}
    // add signed operand_type?
    digit(/* "cmp", */ 0x83, 0x00, 7, rm16_32, imm16_32),     // cmp r/m{16,32}, imm{16,32} (signed)
    digit(/* "dec", */ 0xfe, 0x00, 1, rm8),                   // dec r/m8
    digit(/* "dec", */ 0xff, 0x00, 1, rm16_32),               // dec r/m{16,32}
    digit(/* "div", */ 0xf6, 0x00, 6, rm8),                   // div r/m8
    digit(/* "div", */ 0xf7, 0x00, 6, rm16_32),               // div r/m{16,32}
    digit(/* "idiv", */ 0xf6, 0x00, 7, rm8),                  // idiv r/m8
    digit(/* "idiv", */ 0xf7, 0x00, 7, rm16_32),              // idiv r/m{16,32}
    digit(/* "imul", */ 0xf6, 0x00, 5, rm8),                  // imul r/m8
    digit(/* "imul", */ 0xf7, 0x00, 5, rm16_32),              // imul r/m{16,32}
    digit(/* "inc", */ 0xfe, 0x00, 0, rm8),                   // inc r/m8
    digit(/* "inc", */ 0xff, 0x00, 0, rm16_32),               // inc r/m{16,32}
    digit(/* "jmp", */ 0xff, 0x00, 4, rm16_32),               // jmp r/m{16,32}
    digit(/* "mov", */ 0xc6, 0x00, 0, rm8, imm8),             // mov r/m8, imm8
    digit(/* "mov", */ 0xc7, 0x00, 0, rm16_32, imm16_32),     // mov r/m{16,32}, imm{16,32}
    digit(/* "mul", */ 0xf6, 0x00, 4, al, rm8),               // mul al, r/m8
    digit(/* "mul", */ 0xf7, 0x00, 4, ax, rm16_32),           // mul ax, r/m16
    digit(/* "neg", */ 0xf6, 0x00, 3, rm8),                   // neg r/m8
    digit(/* "neg", */ 0xf7, 0x00, 3, rm16_32),               // neg r/m{16,32}
    digit(/* "nop", */ 0x0f, 0x1f, 0, rm16),                  // nop r/m16
    digit(/* "nop", */ 0x0f, 0x1f, 0, rm16_32),               // nop r/m{16,32}
    digit(/* "not", */ 0xf6, 0x00, 2, rm8),                   // not r/m8
    digit(/* "not", */ 0xf7, 0x00, 2, rm16_32),               // not r/m{16,32}
    digit(/* "or", */ 0x80, 0x00, 1, rm8, imm8),              // or r/m8, imm8
    digit(/* "or", */ 0x81, 0x00, 1, rm16_32, imm16_32),      // or r/m{16,32}, imm{16,32}
    digit(/* "or", */ 0x83, 0x00, 1, rm16_32, imm8),          // or r/m{16,32}, imm8
    digit(/* "pop", */ 0x8f, 0x00, 0, rm16_32),               // pop r/m{16,32}
    digit(/* "push", */ 0xff, 0x00, 0, rm16_32),              // push r/m{16,32}
    digit(/* "rcl", */ 0xd0, 0x00, 2, rm8),                   // rcl r/m8, 1
    digit(/* "rcl", */ 0xd2, 0x00, 2, rm8, cl),               // rcl r/m8, cl
    digit(/* "rcl", */ 0xc0, 0x00, 2, rm8, imm8),             // rcl r/m8, imm8
    digit(/* "rcl", */ 0xd1, 0x00, 2, rm16_32),               // rcl r/m{16,32}, 1
    digit(/* "rcl", */ 0xd3, 0x00, 2, rm16_32, cl),           // rcl r/m{16,32}, cl
    digit(/* "rcl", */ 0xc1, 0x00, 2, rm16_32, imm8),         // rcl r/m{16,32}, imm8
    digit(/* "rcr", */ 0xd0, 0x00, 3, rm8),                   // rcr r/m8, 1
    digit(/* "rcr", */ 0xd2, 0x00, 3, rm8, cl),               // rcr r/m8, cl
    digit(/* "rcr", */ 0xc0, 0x00, 3, rm8, imm8),             // rcr r/m8, imm8
    digit(/* "rcr", */ 0xd1, 0x00, 3, rm16_32),               // rcr r/m{16,32}, 1
    digit(/* "rcr", */ 0xd3, 0x00, 3, rm16_32, cl),           // rcr r/m{16,32}, cl
    digit(/* "rcr", */ 0xc1, 0x00, 3, rm16_32, imm8),         // rcr r/m{16,32}, imm8
    digit(/* "rdrand", */ 0x0f, 0xc7, 6, reg16_32),           // rdrand reg{16,32}
    digit(/* "rdseed", */ 0x0f, 0xc7, 7, reg16_32),           // rdseed reg{16,32}
    digit(/* "rol", */ 0xd0, 0x00, 0, rm8),                   // rol r/m8, 1
    digit(/* "rol", */ 0xd2, 0x00, 0, rm8, cl),               // rol r/m8, cl
    digit(/* "rol", */ 0xc0, 0x00, 0, rm8, imm8),             // rol r/m8, imm8
    digit(/* "rol", */ 0xd1, 0x00, 0, rm16_32),               // rol r/m{16,32}, 1
    digit(/* "rol", */ 0xd3, 0x00, 0, rm16_32, cl),           // rol r/m{16,32}, cl
    digit(/* "rol", */ 0xc1, 0x00, 0, rm16_32, imm8),         // rol r/m{16,32}, imm8
    digit(/* "ror", */ 0xd0, 0x00, 1, rm8),                   // ror r/m8, 1
    digit(/* "ror", */ 0xd2, 0x00, 1, rm8, cl),               // ror r/m8, cl
    digit(/* "ror", */ 0xc0, 0x00, 1, rm8, imm8),             // ror r/m8, imm8
    digit(/* "ror", */ 0xd1, 0x00, 1, rm16_32),               // ror r/m{16,32}, 1
    digit(/* "ror", */ 0xd3, 0x00, 1, rm16_32, cl),           // ror r/m{16,32}, cl
    digit(/* "ror", */ 0xc1, 0x00, 1, rm16_32, imm8),         // ror r/m{16,32}, imm8
    digit(/* "sal", */ 0xd0, 0x00, 4, rm8),                   // sal r/m8, 1
    digit(/* "sal", */ 0xd2, 0x00, 4, rm8, cl),               // sal r/m8, cl
    digit(/* "sal", */ 0xc0, 0x00, 4, rm8, imm8),             // sal r/m8, imm8
    digit(/* "sal", */ 0xd1, 0x00, 4, rm16_32),               // sal r/m{16,32}, 1
    digit(/* "sal", */ 0xd3, 0x00, 4, rm16_32, cl),           // sal r/m{16,32}, cl
    digit(/* "sal", */ 0xc1, 0x00, 4, rm16_32, imm8),         // sal r/m{16,32}, imm8
    digit(/* "sar", */ 0xd0, 0x00, 7, rm8),                   // sar r/m8, 1
    digit(/* "sar", */ 0xd2, 0x00, 7, rm8, cl),               // sar r/m8, cl
    digit(/* "sar", */ 0xc0, 0x00, 7, rm8, imm8),             // sar r/m8, imm8
    digit(/* "sar", */ 0xd1, 0x00, 7, rm16_32),               // sar r/m{16,32}, 1
    digit(/* "sar", */ 0xd3, 0x00, 7, rm16_32, cl),           // sar r/m{16,32}, cl
    digit(/* "sar", */ 0xc1, 0x00, 7, rm16_32, imm8),         // sar r/m{16,32}, imm8
    digit(/* "sbb", */ 0x80, 0x00, 3, rm8, imm8),             // sbb r/m8, imm8
    digit(/* "sbb", */ 0x81, 0x00, 3, rm16_32, imm16_32),     // sbb r/m{16,32}, imm{16,32}
    digit(/* "sbb", */ 0x83, 0x00, 3, rm16_32, imm8),         // sbb r/m{16,32}, imm8
    digit(/* "seto", */ 0x0f, 0x90, 0, rm8),                  // seto r/m8
    digit(/* "setno", */ 0x0f, 0x91, 0, rm8),                 // setno r/m8
    digit(/* "setb", */ 0x0f, 0x92, 0, rm8),                  // setb r/m8
    digit(/* "setae", */ 0x0f, 0x93, 0, rm8),                 // setae r/m8
    digit(/* "sete", */ 0x0f, 0x94, 0, rm8),                  // sete r/m8
    digit(/* "setne", */ 0x0f, 0x95, 0, rm8),                 // setne r/m8
    digit(/* "setbe", */ 0x0f, 0x96, 0, rm8),                 // setbe r/m8
    digit(/* "seta", */ 0x0f, 0x97, 0, rm8),                  // seta r/m8
    digit(/* "sets", */ 0x0f, 0x98, 0, rm8),                  // sets r/m8
    digit(/* "setns", */ 0x0f, 0x99, 0, rm8),                 // setns r/m8
    digit(/* "setp", */ 0x0f, 0x9a, 0, rm8),                  // setp r/m8
    digit(/* "setnp", */ 0x0f, 0x9b, 0, rm8),                 // setnp r/m8
    digit(/* "setl", */ 0x0f, 0x9c, 0, rm8),                  // setl r/m8
    digit(/* "setge", */ 0x0f, 0x9d, 0, rm8),                 // setge r/m8
    digit(/* "setle", */ 0x0f, 0x9e, 0, rm8),                 // setle r/m8
    digit(/* "setg", */ 0x0f, 0x9f, 0, rm8),                  // setg r/m8
    digit(/* "shr", */ 0xd0, 0x00, 7, rm8),                   // shr r/m8, 1
    digit(/* "shr", */ 0xd2, 0x00, 5, rm8, cl),               // shr r/m8, cl
    digit(/* "shr", */ 0xc0, 0x00, 5, rm8, imm8),             // shr r/m8, imm8
    digit(/* "shr", */ 0xd1, 0x00, 5, rm16_32),               // shr r/m{16,32}, 1
    digit(/* "shr", */ 0xd3, 0x00, 5, rm16_32, cl),           // shr r/m{16,32}, cl
    digit(/* "shr", */ 0xc1, 0x00, 5, rm16_32, imm8),         // shr r/m{16,32}, imm8
    digit(/* "sub", */ 0x80, 0x00, 5, rm8, imm8),             // sub r/m8, imm8
    digit(/* "sub", */ 0x81, 0x00, 5, rm16_32, imm16_32),     // sub r/m{16,32}, imm{16,32}
    digit(/* "sub", */ 0x83, 0x00, 5, rm16_32, imm8),         // sub r/m{16,32}, imm8
    digit(/* "test", */ 0xf6, 0x00, 0, rm8, imm8),            // test r/m8, imm8
    digit(/* "test", */ 0xf7, 0x00, 0, rm16_32, imm16_32),    // test r/m{16,32}, imm{16,32}
    digit(/* "xor", */ 0x80, 0x00, 6, rm8, imm8),             // xor r/m8, imm8
    digit(/* "xor", */ 0x81, 0x00, 6, rm16_32, imm16_32),     // xor r/m{16,32}, imm{16,32}
    digit(/* "xor", */ 0x83, 0x00, 6, rm16_32, imm8),         // xor r/m{16,32}, imm8
};

/* /r instructions. */
static const insn reg_ext_tbl[]{
    reg_ext(/* "adc", */ 0x10, 0x00, rm8, reg8),                    // adc r/m8, reg8
    reg_ext(/* "adc", */ 0x11, 0x00, rm16_32, reg16_32),            // adc r/m{16,32}, reg{16,32}
    reg_ext(/* "adc", */ 0x12, 0x00, reg8, rm8),                    // adc reg8, r/m8
    reg_ext(/* "adc", */ 0x13, 0x00, reg16_32, rm16_32),            // adc reg{16,32}, r/m{16,32}
    reg_ext(/* "add", */ 0x00, 0x00, rm8, reg8),                    // add r/m8, reg8
    reg_ext(/* "add", */ 0x01, 0x00, rm16_32, reg16_32),            // add r/m{16,32}, reg{16,32}
    reg_ext(/* "add", */ 0x02, 0x00, reg8, rm8),                    // add reg8, r/m8
    reg_ext(/* "add", */ 0x03, 0x00, reg16_32, rm16_32),            // add reg{16,32}, r/m{16,32}
    reg_ext(/* "and", */ 0x20, 0x00, rm8, reg8),                    // and r/m8, reg8
    reg_ext(/* "and", */ 0x21, 0x00, imm16_32),                     // and r/m{16,32}, reg{16,32}
    reg_ext(/* "and", */ 0x22, 0x00, reg8, rm8),                    // and reg8, r/m8
    reg_ext(/* "and", */ 0x23, 0x00, reg16_32, rm16_32),            // and reg{16,32}, r/m{16,32}
    reg_ext(/* "bound", */ 0x62, 0x00, reg16_32, mem16_32),         // bound reg{16,32}, mem{16,32}
    reg_ext(/* "bsf", */ 0x0f, 0xbc, reg16_32, mem16_32),           // bsf reg{16,32}, mem{16,32}
    reg_ext(/* "bsr", */ 0x0f, 0xbd, reg16_32, mem16_32),           // bsr reg{16,32}, mem{16,32}
    reg_ext(/* "bt", */ 0x0f, 0xa3),                                // bt r/m{16,32}, reg{16,32}
    reg_ext(/* "btc", */ 0x0f, 0xbb),                               // btc r/m{16,32}, reg{16,32}
    reg_ext(/* "btr", */ 0x0f, 0xb3),                               // btr r/m{16,32}, reg{16,32}
    reg_ext(/* "bts", */ 0x0f, 0xab),                               // bts r/m, reg
    reg_ext(/* "cmovae", */ 0x0f, 0x43, reg16_32, rm16_32),         // cmovae reg{16,32}, r/m{16,32}
    reg_ext(/* "cmove", */ 0x0f, 0x44, reg16_32, rm16_32),          // cmovz reg{16,32}, r/m{16,32} (= cmovz)
    reg_ext(/* "cmovne", */ 0x0f, 0x45, reg16_32, rm16_32),         // cmovne reg{16,32}, r/m{16,32} (= cmovnz)
    reg_ext(/* "cmovbe", */ 0x0f, 0x46, reg16_32, rm16_32),         // cmovnbe reg{16,32}, r/m{16,32} (= cmovna)
    reg_ext(/* "cmova", */ 0x0f, 0x47, reg16_32, rm16_32),          // cmova reg{16,32}, r/m{16,32} (= cmovnbe)
    reg_ext(/* "cmovs", */ 0x0f, 0x48, reg16_32, rm16_32),          // cmovs reg{16,32}, r/m{16,32}
    reg_ext(/* "cmovns", */ 0x0f, 0x49, reg16_32, rm16_32),         // cmovns reg{16,32}, r/m{16,32}
    reg_ext(/* "cmovp", */ 0x0f, 0x4a, reg16_32, rm16_32),          // cmovp reg{16,32}, r/m{16,32} (= cmovpe)
    reg_ext(/* "cmovnp", */ 0x0f, 0x4b, reg16_32, rm16_32),         // cmovnp reg{16,32}, r/m{16,32} (= cmovpo)
    reg_ext(/* "cmovl", */ 0x0f, 0x4c, reg16_32, rm16_32),          // cmovl reg{16,32}, r/m{16,32} (= cmovnge)
    reg_ext(/* "cmovge", */ 0x0f, 0x4d, reg16_32, rm16_32),         // cmovge reg{16,32}, r/m{16,32} (= cmovnl)
    reg_ext(/* "cmovle", */ 0x0f, 0x4e, reg16_32, rm16_32),         // cmovle reg{16,32}, r/m{16,32} (= cmovng)
    reg_ext(/* "cmovg", */ 0x0f, 0x4f, reg16_32, rm16_32),          // cmovg reg{16,32}, r/m{16,32} (= cmovnle)
    reg_ext(/* "cmp", */ 0x38, 0x00, rm8, reg8),                    // cmp r/m8, reg8
    reg_ext(/* "cmp", */ 0x39, 0x00, rm16_32, reg16_32),            // cmp r/m{16,32}, reg{16,32}
    reg_ext(/* "cmp", */ 0x3a, 0x00, reg8, rm8),                    // cmp reg8, r/m8
    reg_ext(/* "cmp", */ 0x3b, 0x00, reg16_32, rm16_32),            // cmp reg{16,32}, r/m{16,32}
    reg_ext(/* "cmpxchg", */ 0x0f, 0xb0, rm8, reg8),                // cmpxchg r/m8, reg8
    reg_ext(/* "cmpxchg", */ 0x0f, 0xb1),                           // cmpxchg r/m{16,32}, reg{16,32}
    // CMPXCHG8B CMPXCHG16B
    reg_ext(/* "imul", */ 0x0f, 0xaf, reg16_32, rm16_32),           // imul reg{16,32}, r/m{16,32}
    reg_ext(/* "imul", */ 0x6b, 0x00, reg16_32, rm16_32, imm8),     // imul reg{16,32}, r/m{16,32}, imm8
    reg_ext(/* "imul", */ 0x69, 0x00, reg16_32, rm16_32, imm16_32), // imul reg{16,32}, r/m{16,32}, imm{16,32}
    // LDS LES LFS LGS LSS
    reg_ext(/* "lea", */ 0x8d, 0x00, reg16_32, mem16_32),           // lea reg{16,32}, mem
    // LOOP
    // LZCNT
    reg_ext(/* "mov", */ 0x88, 0x00, rm8, reg8),                    // mov r/m8, reg8
    reg_ext(/* "mov", */ 0x89, 0x00, rm16_32, reg16_32),            // mov r/m{16,32}, reg{16,32}
    reg_ext(/* "mov", */ 0x8a, 0x00, reg8, rm8),                    // mov reg8, r/m8
    reg_ext(/* "mov", */ 0x8b, 0x00, reg16_32, rm16_32),            // mov reg{16,32}, r/m{16,32}
    // FIXME - encode both reg16,32 and mem16
    reg_ext(/* "mov", */ 0x8c, 0x00, rm16_32, sreg),                // mov reg{16,32}/mem16, sreg
    reg_ext(/* "mov", */ 0x8e, 0x00, sreg, rm16),                   // mov sreg, r/m16
    reg_ext(/* "movsx", */ 0x0f, 0xbe, reg16_32, rm8),              // movsx reg{16,32}, r/m8
    reg_ext(/* "movsx", */ 0x0f, 0xbf, reg32, rm16),                // movsx reg32, r/m16
    reg_ext(/* "movzx", */ 0x0f, 0xb6, reg16_32, rm8),              // movzx reg{16,32}, r/m8
    reg_ext(/* "movzx", */ 0x0f, 0xb7, reg32, rm16),                // movzx reg32, r/m16
    reg_ext(/* "or", */ 0x08, 0x00, rm8, reg8),                     // or r/m8, reg8
    reg_ext(/* "or", */ 0x09, 0x00, rm16_32, reg16_32),             // or reg{16,32}, r/m{16,32}
    reg_ext(/* "or", */ 0x0a, 0x00, reg8, rm8),                     // or reg8, r/m8
    reg_ext(/* "or", */ 0x0b, 0x00, reg16_32, rm16_32),             // or reg{16,32}, r/m{16,32}
    // POPCNT
    reg_ext(/* "sbb", */ 0x18, 0x00, rm8, reg8),                    // sbb r/m8, reg8
    reg_ext(/* "sbb", */ 0x19, 0x00, rm16_32, reg16_32),            // sbb r/m{16,32}, reg{16,32}
    reg_ext(/* "sbb", */ 0x1a, 0x00, reg16_32, rm8),                // sbb reg{16,32}, rm8
    reg_ext(/* "sbb", */ 0x1b, 0x00, reg16_32, rm16_32),            // sbb reg{16,32}, r/m{16,32}
    reg_ext(/* "shld", */ 0x0f, 0xa4, rm16_32, reg16_32, imm8),     // shld r/m{16,32}, reg{16,32}, imm8
    reg_ext(/* "shld", */ 0x0f, 0xa5, rm16_32, reg16_32, cl),       // shld r/m{16,32}, reg{16,32}, cl
    reg_ext(/* "shrd", */ 0x0f, 0xac, rm16_32, reg16_32, imm8),     // shrd r/m{16,32}, reg{16,32}, imm8
    reg_ext(/* "shrd", */ 0x0f, 0xad, rm16_32, reg16_32, cl),       // shrd r/m{16,32}, reg{16,32}, cl
    reg_ext(/* "sub", */ 0x28, 0x00, rm8, reg8),                    // sub r/m8, reg8
    reg_ext(/* "sub", */ 0x29, 0x00, rm16_32, reg16_32),            // sub r/m{16,32}, reg{16,32}
    reg_ext(/* "sub", */ 0x2a, 0x00, reg8, rm16_32),                // sub reg8, r/m{16,32}
    reg_ext(/* "sub", */ 0x2b, 0x00, reg16_32, rm16_32),            // sub reg{16,32}, r/m{16,32}
    reg_ext(/* "test", */ 0x84, 0x00, rm8, reg8),                   // test r/m8, reg8
    reg_ext(/* "test", */ 0x85, 0x00, rm16_32, reg16_32),           // test r/m{16,32}, reg{16,32}
    reg_ext(/* "ud1", */ 0x0f, 0xb9),                               // ud1
    reg_ext(/* "xadd", */ 0x0f, 0xc0, rm8, reg8),                   // xadd r/m8, reg8
    reg_ext(/* "xadd", */ 0x0f, 0xc1, rm16_32, reg16_32),           // xadd r/m{16,32}, reg{16,32}
    reg_ext(/* "xchg", */ 0x86, 0x00, rm8, reg8),                   // xchg r/m8, reg8
    reg_ext(/* "xchg", */ 0x87, 0x00, rm16_32, reg16_32),           // xchg r/m{16,32}, reg{16,32}
    reg_ext(/* "xor", */ 0x30, 0x00, rm8, reg8),                    // xor r/m8, reg8
    reg_ext(/* "xor", */ 0x31, 0x00, rm16_32, reg16_32),            // xor r/m{16,32}, reg{16,32}
    reg_ext(/* "xor", */ 0x32, 0x00, reg8, rm8),                    // xor reg8, r/m8
    reg_ext(/* "xor", */ 0x33, 0x00, reg16_32, rm16_32),            // xor reg{16,32}, r/m{16,32}
};

static insn lookup(u8* code, u8 byte, u8 byte2)
{
    for (const auto& ins : reg_ext_tbl) {
        if (ins.byte == byte && ins.byte2 == byte2)
            return ins;
    }
    for (const auto& ins : no_mod_tbl) {
        if (ins.byte == byte && ins.byte2 == byte2)
            return ins;
    }
    u8 modrm = *code;
    u8 ext = MODRM_REG(modrm);
    for (const auto& ins : ext_tbl) {
        if (ins.byte == byte && ins.byte2 == byte2
            && (ins.flags & insn_ext_mask) == ext)
            return ins;
    }
    return {};
}

enum class reg_type : u8 {
    r8, r16 = 4, r32 = 8, sreg = 12
};

/* conversion from modrm.reg:
*  r8:   (.reg + 1)
*  r16:  (.reg + 1) << 4
*  r32:  (.reg + 1) << 8
*  sreg: (.reg + 1) << 12
*/
static reg make_reg(u8 r, reg_type type)
{
    if (r == 0xff)
        return reg::none;
    return (reg)((r + 1) << (u8)type);
}

static insn decode_bswap(u8 byte2)
{
    auto ins = no_mod(0x0f, byte2, builtin32);
    ins.op.reg = make_reg(byte2 & 0b111, reg_type::r32);
    ins.length = 2;
    return ins;
}

static insn decode_enter(u8* code)
{
    insn ins{ 0xc8, 0x00, 0x00, imm16, imm8, operand_type::none, 0 };
    ins.op.imm.word = *(u16*)code;
    code += 2;
    ins.op2.imm.byte = *code;
    ins.length = 4;
    return ins;
}

static void fix_naming(insn& ins)
{
    if (ins.name.ends_with("sd"))
        ins.name[ins.name.length() - 1] = 'w';
    else if (ins.name.ends_with("ad") || ins.name.ends_with("fd"))
        ins.name.pop_back();
    else if (ins.name == "cwde")
        ins.name = "cbw";
    else if (ins.name == "cdq")
        ins.name = "cwd";
}

void decoder::parse_imm_args(operand& op, bool op_16b)
{
    if (op.type == imm8) {
        op.imm.byte = *code;
        advance();
    } else if (op.type == imm16 || (op_16b && op.type == imm16_32)) {
        op.imm.word = *(u16*)code;
        advance(2);
    } else if (op.type == imm16_32 || op.type == m_off) {
        op.imm.dword = *(u32*)code;
        advance(4);
    }
}

enum prefix : u8 {
    lock = 0xf0, repne = 0xf2, repe = 0xf3,
    es = 0x26, cs = 0x2e, ss = 0x36, ds = 0x3e, fs = 0x64, gs = 0x65,
    ext_pref = 0x0f,
    op_ovr = 0x66, addr_ovr = 0x67,
};

bool decoder::parse_prefixes(u32& pref)
{
    switch (*code) {
    case lock:                  pref |= pref_lock; return true;
    case repe: /* rep, repz */  pref |= pref_repe; return true;
    case repne: /* repnz */     pref |= pref_repne; return true;
    case es: case cs: case ss:
    case ds: case fs: case gs:  pref |= pref_sreg; return true;
    case op_ovr:                pref |= pref_op_ovr; return true;
    case addr_ovr:              pref |= pref_addr_ovr; return true;
    default: return false;
    }
}

void decoder::parse_modrm(insn& ins, u8& reg_op, u8& reg_op2, u8 byte)
{
    ins.modrm = *code;
    advance();
    u8 mod = MODRM_MOD(ins.modrm);
    u8 rm = MODRM_RM(ins.modrm);
    u8 reg = MODRM_REG(ins.modrm);

    bool addr_16b = ins.prefixes & pref_addr_ovr;

    /* Handle nonexistent/disallowed sregs */
    if (byte == 0x8c) {
        if (reg > 0b101 /* none */)
            ins.flags |= insn_err;
    } else if (byte == 0x8e) {
        if (reg == 0b001 /* cs */ || reg > 0b101 /* none */)
            ins.flags |= insn_err;
    }

    /* Check addressing mode */
    if (mod == 0b11) {
        if (ins.op.is_mem() || ins.op2.is_mem())
            ins.flags |= insn_err;
    } else {
        ins.flags |= insn_use_mem;
        if (rm == 0b100 && !addr_16b)
            ins.flags |= insn_has_sib;
    }

    /* Get first register (+ displacement size) */
    if (mod == 0b00) {
        if (rm == 0b110 && addr_16b)
            ins.disp_size = 2;
        else if (rm == 0b101)
            ins.disp_size = 4;
        else
            reg_op = rm;
    } else {
        reg_op = rm;
        if (mod == 0b01)
            ins.disp_size = 1;
        else if (mod == 0b10)
            ins.disp_size = addr_16b ? 2 : 4;
    }

    /* If /r, get second register */
    if (ins.flags & insn_reg_ext) {
        reg_op2 = reg;
        /* src/dst are "swapped" in the modrm table */
        if (ins.op2.is_rm() || ins.op2.is_mem() || ins.op.type == sreg)
            std::swap(reg_op, reg_op2);
    }

    /* Parse sib byte */
    if (ins.flags & insn_has_sib) {
        ins.sib = *code;
        advance();
        ins.sib_base = make_reg(SIB_BASE(ins.sib), reg_type::r32);
        ins.sib_index = make_reg(SIB_INDEX(ins.sib), reg_type::r32);
        switch (SIB_SCALE(ins.sib)) {
        case 0b00: ins.sib_scale = 1; break;
        case 0b01: ins.sib_scale = 2; break;
        case 0b10: ins.sib_scale = 4; break;
        case 0b11: ins.sib_scale = 8; break;
        }
    }

    /* Get displacement */
    switch (ins.disp_size) {
    case 0: break;
    case 1: ins.disp = *code; break;
    case 2: ins.disp = *(u16*)code; break;
    case 4: ins.disp = *(u32*)code; break;
    }
    advance(ins.disp_size);
}

static void fill_regs(insn& ins, u8 reg_op, u8 reg_op2)
{
    bool op_16b = ins.prefixes & pref_op_ovr;
    auto fill_builtin = [op_16b](operand& op) {
        switch (op.type) {
        case al: op.reg = reg::al; return true;
        case ax: op.reg = reg::ax; return true;
        case eax: op.reg = op_16b ? reg::ax : reg::eax; return true;
        case cl: op.reg = reg::cl; return true;
        case dx: op.reg = reg::dx; return true;
        default: return false;
        }
    };

    auto get_reg_type = [mem = ins.flags & insn_use_mem](operand& op, u32 pref) {
        switch (op.type) {
        case builtin8:
        case reg8:     return reg_type::r8;
        case reg16:    return reg_type::r16;
        case reg32:
        case builtin32:
        case mem8:
        case mem16:
        case mem16_32: return reg_type::r32;
        case builtin16_32:
        case reg16_32: return pref & pref_op_ovr ? reg_type::r16 : reg_type::r32;
        case rm8:      return mem ? reg_type::r32 : reg_type::r8;
        case rm16:     return mem ? reg_type::r32 : reg_type::r16;
        case rm16_32:  return mem ? reg_type::r32 : pref & pref_op_ovr ? reg_type::r16 : reg_type::r32;
        case sreg:     return reg_type::sreg;
        default:       __debugbreak(); return reg_type::r8; // prevent compiler warning
        }
    };
    if (ins.op.type != none && !ins.op.is_imm() && !fill_builtin(ins.op))
        ins.op.reg = make_reg(reg_op, get_reg_type(ins.op, ins.prefixes));
    if ((ins.op2.is_reg() || ins.op2.is_rm() || ins.op2.is_mem()) && !(ins.flags & insn_has_sib) && !fill_builtin(ins.op2))
        ins.op2.reg = make_reg(reg_op2, get_reg_type(ins.op2, ins.prefixes));
    fill_builtin(ins.op3);
}

insn disasm(u8* bytes)
{
    decoder dc{};
    dc.code = bytes;
    dc.pos = 0;

    u32 pref{};
    for (u32 i{}; i <= 15; i++) {
        if (!dc.parse_prefixes(pref))
            break;
        dc.advance();
    }

    u8 byte = *dc.code;
    dc.advance();
    u8 byte2{};
    if (byte == ext_pref || byte == 0xd4 /* aad */ || byte == 0xd5 /* aam */) {
        byte2 = *dc.code;
        dc.advance();
    }

    insn ins{};
    /* Ignore last 3 bits if +rb/+rw/+rd */
    if ((byte >= 0x40 && byte <= 0x5f) || (byte >= 0x90 && byte <= 0x97)
    || (byte >= 0xb8 && byte <= 0xbf))
        ins = lookup(dc.code, byte & 0xf8, byte2);
    /* Kind of a hack but I can't find a better approach for these 2 */
    else if (byte == ext_pref && byte2 >= 0xc8 && byte2 <= 0xcf)
        return decode_bswap(byte2);
    else if (byte == 0xc8)
        return decode_enter(dc.code);
    else
        ins = lookup(dc.code, byte, byte2);

    ins.prefixes = pref;
    bool op_16b = pref & pref_op_ovr;

    if (op_16b) {
        /* Handle prefixes for some mov insns */
        if (byte >= 0xa0 && byte <= 0xa3) {
            if (pref & pref_addr_ovr)
                ins.prefixes |= pref_op_ovr;
            else
                ins.prefixes &= ~pref_op_ovr;
        }
        /* Some insn names change with operand size */
        fix_naming(ins);
    }

    u8 reg_op{ 0xff }, reg_op2{ 0xff };

    if ((ins.flags & 0xf) != insn_no_mod) {
        dc.parse_modrm(ins, reg_op, reg_op2, byte);
    } else if (ins.prefixes & pref_lock) {
        ins.flags |= insn_err;
    } else if (ins.op.type == builtin8 || ins.op.type == builtin16_32) {
        if (byte2)
            reg_op = byte2 & 0b111;
        else
            reg_op = byte & 0b111;
    }

    dc.parse_imm_args(ins.op, op_16b);
    dc.parse_imm_args(ins.op2, op_16b);
    dc.parse_imm_args(ins.op3, op_16b);

    /* Shift insn with 0x1 as default */
    if (ins.byte == 0xd0 || ins.byte == 0xd1) {
        ins.op2.type = imm8;
        ins.op2.imm.byte = 0x1;
    }

    fill_regs(ins, reg_op, reg_op2);

    ins.length = dc.pos;
    if (ins.length > 15)
        ins.flags |= insn_err;

    return ins;
}

}
