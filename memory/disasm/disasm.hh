#pragma once

#include <cstdint>
#include <string>

namespace disasm {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;

enum insn_flag : u32 {
    /* Internal usage only - assigned on construction */
    insn_ext_mask = (0b111),
    insn_reg_ext = (1 << 3),
    insn_no_mod = (insn_ext_mask | insn_reg_ext),

    /* Added while decoding */
    insn_use_mem = (1 << 5),
    insn_has_sib = (1 << 6),
    insn_err = (1 << 7),
};

enum prefix_flag : u8 {
    pref_lock = (1 << 0),
    pref_repe = (1 << 1),
    pref_repne = (1 << 2),
    pref_sreg = (1 << 3),
    pref_op_ovr = (1 << 4),
    pref_addr_ovr = (1 << 5),
};

enum class reg : u32 {
    /* 8 bit */
    al = 1, cl, dl, bl, ah, ch, dh, bh,
    /* 16 bit */
    ax = al << 4, cx = cl << 4, dx = dl << 4, bx = bl << 4,
    sp = ah << 4, bp = ch << 4, si = dh << 4, di = bh << 4,
    /* 32 bit */
    eax = al << 8, ecx = cl << 8, edx = dl << 8, ebx = bl << 8,
    esp = ah << 8, ebp = ch << 8, esi = dh << 8, edi = bh << 8,
    /* Segment */
    es = al << 12, cs = cl << 12, ss = dl << 12,
    ds = bl << 12, fs = ah << 12, gs = ch << 12,

    none = 0xffffffff,
};

enum class operand_type : u32 {
    none,
    imm8, imm16, imm16_32,
    rm8, rm16, rm16_32,
    mem8, mem16, mem16_32, m_off,
    builtin8, builtin16_32, builtin32,
    reg8, reg16, reg16_32, reg32,
    sreg,
    al, ax, eax,
    cl, dx
};

struct operand {
    constexpr operand() = default;

    operand_type type{};
    reg reg{ reg::none };
    union {
        u8 byte;
        s8 sbyte;
        u16 word;
        s16 sword;
        u32 dword;
        s32 sdword;
    } imm;

    bool is_imm() const
    {
        return type == operand_type::imm8 || type == operand_type::imm16 || type == operand_type::imm16_32;
    }

    bool is_rm() const
    {
        return type == operand_type::rm8 || type == operand_type::rm16 || type == operand_type::rm16_32;
    }

    bool is_reg() const
    {
        return type >= operand_type::reg8 && type <= operand_type::dx;
    }

    bool is_mem() const
    {
        return type == operand_type::mem8 || type == operand_type::mem16 || type == operand_type::mem16_32;
    }
};

struct insn {
    constexpr insn() = default;
    constexpr insn(u8 b1, u8 b2, u8 b3, operand_type t1, operand_type t2, operand_type t3, u8 f)
        : byte(b1), byte2(b2), byte3(b3), flags(f)
    {
        op.type = t1;
        op2.type = t2;
        op3.type = t3;
    }

    const auto& dst() const { return op; }
    const auto& src() const { return op2; }
    bool error() const { return flags & insn_err; }

    std::string name{ "???" };   // mnemonic
    u8 byte{};                   // 1st opcode byte
    u8 byte2{};                  // 2nd opcode byte
    u8 byte3{};                  // 3rd opcode byte
    operand op{};                // 1st operand (dst if 2)
    operand op2{};               // 2nd operand (src)
    operand op3{};               // 3rd operand
    u8 modrm{};                  // modrm byte
    u8 sib{};                    // scale index byte
    u8 sib_scale{};              // sib scale
    reg sib_index{ reg::none };  // sib index register
    reg sib_base{ reg::none };   // sib base register
    u8 disp_size{};              // displacement size
    s32 disp{};                  // displacement (also immediate memory operands)
    u32 prefixes{};              // prefix bytes
    u32 flags{};                 // flags
    u32 length{};                // total bytes
};

insn disasm(u8* bytes);

}
