#include <fb.h>
#include <tar.h>
#include <types.h>
#include <keyboard.h>
/*

    AM8 = Alintosh Machine 8-bit
    AA8 = Alintosh Architure 8-bit

    MIT License

    Alintosh Machine 8-bit is a simple 8-bit machine emulator.
    Honestly, This machine emulator is a hobby, for testing purposes.
    I designed it to be similar to "x86" Machines (PCs and Laptops).
    Similar to x86 means I used Variable Instrustion Length (VIL).
    That is because RISC Architures imposes upon you Constant Number of Instructions.
    Max Instruction Size: 2 Bytes.
    Physical Memory Size: 256 Bytes.

    Opcode Byte:

    1 1_0_1_0_1 0_0
    |    |____   |______
    v         v         v
    #Operands Operation Between R / M ?

    if highest bit of Opcode is set and frist bit and second bit of Opcode are cleared, This mean Next byte is Constant (e.g. 3).
    if highest bit of Opcode is set and frist bit of Opcode is set and second bit of Opcode are cleared and Next byte is smaller than 4,
    This mean Next byte is pointing to Register (e.g. 0 or b).
    else if Next byte isn't smaller than 4 , This mean "InvalidOpcodeError" (Proccessor exits with exit code 6).
    if highest bit of Opcode is set and frist bit of Opcode is cleared and second bit of Opcode is set,
    This mean Next byte is Memory Address (e.g. [0xEE]).
    if highest bit of Opcode is set and frist bit and second bit of Opcode are set and Next byte is smaller than 4,
    This mean Next byte is Memory Address but with Register instead of Constant (e.g. [0]).
    if highest bit of Opcode is cleared, This mean Extended Opcode.

    FLAGS:
    FLAGS Reg. used to store the status of the last operation.

    __________________________________________
    |  Bit  | Description                   |
    |   0   | Equal Flag                    |
    |   1   | Greater Than Flag             |
    |   2   | Smaller Than Flag             |
    |   3   | Carry Flag                    |
    |   4   | Zero Flag                     |
    | 5 - 7 | Unused (yet)                  |

    Supported Main Opcodes:

    mov : 0 (Successful)
    add : 1 (Successful)
    sub : 2 (Successful)
    mul : 3 (Successful)
    and : 4 (Successful)
    or : 5 (Successful)
    lod : 6 (Successful)
    jmp : 7 (Successful)
    cmp : 8 (Successful)
    jcc : 9 - 20 (Successful)
    push : 21 (Successful)
    pop : 22 (Successful)
    call : 23 (Successful)


*/

#define STACK_LENGTH 16

/* -------- Opcode Definitions -------- */
#define MOV_CONST 0x80 //Successful!
#define MOV_REG 0x81 //Successful!
#define MOV_ADDR 0x82 //Successful!
#define MOV_REG_ADDR 0x83 //Successful!
#define ADD_CONST 0x84 //Successful!
#define ADD_REG 0x85 //Successful!
#define ADD_ADDR 0x86 //Successful!
#define ADD_REG_ADDR 0x87 //Successful!
#define SUB_CONST 0x88 //Successful!
#define SUB_REG 0x89 //Successful!
#define SUB_ADDR 0x8A //Successful!
#define SUB_REG_ADDR 0x8B //Successful!
#define MUL_CONST 0x8C //Successful!
#define MUL_REG 0x8D //Successful!
#define MUL_ADDR 0x8E //Successful!
#define MUL_REG_ADDR 0x8F //Successful!
#define AND_CONST 0x90 //Successful!
#define AND_REG 0x91 //Successful!
#define AND_ADDR 0x92 //Successful!
#define AND_REG_ADDR 0x93 //Successful!
#define OR_CONST 0x94 //Successful!
#define OR_REG 0x95 //Successful!
#define OR_ADDR 0x96 //Successful!
#define OR_REG_ADDR 0x97 //Successful!
#define LOD_CONST 0x98 //Successful!
#define LOD_REG 0x99 //Successful!
#define LOD_ADDR 0x9A //Successful!
#define LOD_REG_ADDR 0x9B //Successful!
#define JMP_CONST 0x9C //Successful!
#define JMP_REG 0x9D //Successful!
#define JMP_ADDR 0x9E //Successful!
#define JMP_REG_ADDR 0x9F //Successful!
#define CMP_CONST 0xA0 //Successful!
#define CMP_REG 0xA1 //Successful!
#define CMP_ADDR 0xA2 //Successful!
#define CMP_REG_ADDR 0xA3 //Successful!
#define JE 0xA4 //Successful!
#define JNE 0xA8 //Successful!
#define JG 0xAC //Successful!
#define JNG 0xB0 //Successful!
#define JS 0xB4 //Successful!
#define JNS 0xB8 //Successful!
#define JGE 0xBC //Successful!
#define JSE 0xC0 //Successful!
#define JZ 0xC4 //Successful!
#define JNZ 0xC8 //Successful!
#define JC 0xCC //Successful!call
#define JNC 0xD0 //Successful!
#define PUSH_CONST 0xD4 //Successful!
#define PUSH_REG 0xD5 //Successful!
#define PUSH_ADDR 0xD6 //Successful!
#define PUSH_REG_ADDR 0xD7 //Successful!
#define POP_REG 0xD9 //Successful!
#define POP_ADDR 0xDA //Successful!
#define POP_REG_ADDR 0xDB //Successful!
#define CALL_CONST 0xDC //Successful!
#define CALL_REG 0xDD //Successful!
#define CALL_ADDR 0xDE //Successful!
#define CALL_REG_ADDR 0xDF //Successful!
#define BREAK 0x00 //Successful!
#define RET 0x01 //Successful!
#define NOT 0x02 //Successful!
#define INC 0x03 //Successful!
#define DEC 0x04 //Successful!
#define CPUCALL 0x05 //Successful!
static uint8_t ram[256];
static uint8_t regs[4] = {1, 0, 0, 0};
static uint8_t flags = 0;
static uint8_t ip = 0;
static uint8_t mp = 0;
//static uint8_t cr = 0;
static uint8_t sp = 0xFF;
//static uint8_t skip = 0;

static void print_ram(uint8_t *ram) {
    for (uint8_t i = 0; i < (20 * 4); i++) {
        if (i == mp) {
            fb_putc('[');
            fb_puts_hex(ram[i], 1);
            fb_putc(']');
            continue;
        }
        fb_putc(' ');
        fb_puts_hex(ram[i], 1);
        fb_putc(' ');
        if ((i + 1) % 20 == 0) {
            fb_putc('\n');
        }
    }
}

static uint8_t is_carry(uint8_t a, uint8_t b, uint8_t operation) {
	switch (operation) {
		case 0: //ADD
	        return (((uint16_t)a & 0xFF) + ((uint16_t)b & 0xFF)) > 0xFF;
            break;
		case 1: //MUL
            return (((uint16_t)a & 0xFF) * ((uint16_t)b & 0xFF)) > 0xFF;
            break;
    }
    return 0;
}

static void CPUCall() {
    if (regs[0] == 0) fb_puts((char *)&ram[regs[1]]); //CPUPrint
    if (regs[0] == 1) regs[0] = (uint8_t)keyboard_get_char(); //CPURegisterInput
    if (regs[0] == 2) ram[regs[1]] = (uint8_t)keyboard_get_char(); //CPUBufferInput
}

static void sleep(int seconds)
{
    for (int j = 0; j < seconds; j++) {
        for (int i = 0; i < 0x7FFFFFF; i++);
    }
}

int AM8(char *filename) {
    char *codefile = tar_read(filename);
    /*
    if (codefile == NULL) {
        fb_puts("Cannot open file: ");
        fb_puts(filename);
        fb_putc('\n');
        return 1;
    }
    */
    for (uint16_t i = 0; i < 256; i++)
    {
        ram[i] = (uint8_t)codefile[i];
    }

    uint8_t opcode_byte = 0;

    while (1) {
        print_ram(ram);
        fb_puts("memptr: ");
        fb_puts_hex(((int) mp) & 0xFF, 1);
        //printf("#Instruction Executed (#IE) is : %d\n", ((int) ip) & 0xFF);
        if (*(uint16_t *)&ram[mp] == 0x8100) mp++;
        opcode_byte = ram[mp];
        //printf("Current Opcode Byte (COB) is : %d\n", ((int) opcode_byte) & 0xFF);
        /* Extended opcode (highest bit cleared) */
        if (!(opcode_byte & 128)) {
            switch (opcode_byte) {
                case 0:
                    //printf("Breakpoint reached.\n");
                    return 0;
                case RET:
                    if (sp == 0) {
                        sp = 0xFF;
                    } else {
                        sp++;
                    }
                    mp = ram[sp];
                    ram[sp] = 0;
                    break;
                case NOT:
                    regs[0] = ~regs[0];
                    mp++;
                    break;
                case INC:
                    regs[0]++;
                    mp++;
                    break;
                case DEC:
                    regs[0]--;
                    mp++;
                    break;
                case CPUCALL:
                    CPUCall();
                    mp++;
                    break;
            }
            sleep(1);
            continue;
        }

        /* -------- Operand helpers for the new opcode layout -------- */
        uint8_t operand = 0; /* value of the operand (const, reg index, addr, reg index) */
        uint8_t mode = 0;

        switch (opcode_byte) {
            /* ==================== MOV ==================== */
            case MOV_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case MOV_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case MOV_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case MOV_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== ADD ==================== */
            case ADD_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case ADD_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case ADD_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case ADD_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== SUB ==================== */
            case SUB_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case SUB_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case SUB_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case SUB_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== MUL ==================== */
            case MUL_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case MUL_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case MUL_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case MUL_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== AND ==================== */
            case AND_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case AND_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case AND_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case AND_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== OR ==================== */
            case OR_CONST:       mode = 0; mp++; operand = ram[mp]; break;
            case OR_REG:         mode = 1; mp++; operand = ram[mp]; break;
            case OR_ADDR:        mode = 2; mp++; operand = ram[mp]; break;
            case OR_REG_ADDR:    mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== LOD ==================== */
            case LOD_REG:         mode = 1; mp++; operand = ram[mp]; break;
            case LOD_ADDR:        mode = 2; mp++; operand = ram[mp]; break;
            case LOD_REG_ADDR:    mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== JMP ==================== */
            case JMP_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case JMP_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case JMP_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case JMP_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== CMP ==================== */
            case CMP_CONST:      mode = 0; mp++; operand = ram[mp]; break;
            case CMP_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case CMP_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case CMP_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== JCC ==================== */
            case JE:             mode = 0; mp++; operand = ram[mp]; break;
            case JNE:            mode = 0; mp++; operand = ram[mp]; break;
            case JG:             mode = 0; mp++; operand = ram[mp]; break;
            case JNG:            mode = 0; mp++; operand = ram[mp]; break;
            case JS:             mode = 0; mp++; operand = ram[mp]; break;
            case JNS:            mode = 0; mp++; operand = ram[mp]; break;
            case JGE:            mode = 0; mp++; operand = ram[mp]; break;
            case JSE:            mode = 0; mp++; operand = ram[mp]; break;
            case JZ:             mode = 0; mp++; operand = ram[mp]; break;
            case JNZ:            mode = 0; mp++; operand = ram[mp]; break;
            case JC:             mode = 0; mp++; operand = ram[mp]; break;
            case JNC:            mode = 0; mp++; operand = ram[mp]; break;
            /* ==================== PUSH ==================== */
            case PUSH_CONST:     mode = 0; mp++; operand = ram[mp]; break;
            case PUSH_REG:       mode = 1; mp++; operand = ram[mp]; break;
            case PUSH_ADDR:      mode = 2; mp++; operand = ram[mp]; break;
            case PUSH_REG_ADDR:  mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== POP ==================== */
            case POP_REG:        mode = 1; mp++; operand = ram[mp]; break;
            case POP_ADDR:       mode = 2; mp++; operand = ram[mp]; break;
            case POP_REG_ADDR:   mode = 3; mp++; operand = ram[mp]; break;
            /* ==================== CALL ==================== */
            case CALL_CONST:     mode = 0; mp++; operand = ram[mp]; break;
            case CALL_REG:       mode = 1; mp++; operand = ram[mp]; break;
            case CALL_ADDR:      mode = 2; mp++; operand = ram[mp]; break;
            case CALL_REG_ADDR:  mode = 3; mp++; operand = ram[mp]; break;

            default:
                fb_puts("InvalidOpcodeError\n");
                return 6;
        }

        /* Validate register operand for modes 1 and 3 */
        //if (validate_reg_operand(opcode_byte, mp) != 0) {
        //    return 6;
        //}

        switch (opcode_byte) {

            /* ==================== MOV ==================== */
            case MOV_CONST:
            case MOV_REG:
            case MOV_ADDR:
            case MOV_REG_ADDR:
                switch (mode) {
                    case 0: regs[0] = operand; break;
                    case 1: regs[0] = regs[operand]; break;
                    case 2: regs[0] = ram[operand]; break;
                    case 3: regs[0] = ram[regs[operand]]; break;
                }
                if (regs[0] == 0) flags |= 0x10;
                break;

            /* ==================== ADD ==================== */
            case ADD_CONST:
            case ADD_REG:
            case ADD_ADDR:
            case ADD_REG_ADDR: {
                uint8_t rhs;
                switch (mode) {
                    case 0: rhs = operand; break;
                    case 1: rhs = regs[operand]; break;
                    case 2: rhs = ram[operand]; break;
                    case 3: rhs = ram[regs[operand]]; break;
                    default: rhs = 0; break;
                }
                if (is_carry(regs[0], rhs, 0)) {
                    flags |= 8;
                } else {
                    flags &= ~8;
                }
                regs[0] += rhs;
                if (regs[0] == 0) flags |= 0x10;
                break;
            }

            /* ==================== SUB ==================== */
            case SUB_CONST:
            case SUB_REG:
            case SUB_ADDR:
            case SUB_REG_ADDR: {
                uint8_t rhs;
                switch (mode) {
                    case 0: rhs = operand; break;
                    case 1: rhs = regs[operand]; break;
                    case 2: rhs = ram[operand]; break;
                    case 3: rhs = ram[regs[operand]]; break;
                    default: rhs = 0; break;
                }
                regs[0] -= rhs;
                if (regs[0] == 0) flags |= 0x11;
                break;
            }

            /* ==================== MUL ==================== */
            case MUL_CONST:
            case MUL_REG:
            case MUL_ADDR:
            case MUL_REG_ADDR: {
                uint8_t rhs;
                switch (mode) {
                    case 0: rhs = operand; break;
                    case 1: rhs = regs[operand]; break;
                    case 2: rhs = ram[operand]; break;
                    case 3: rhs = ram[regs[operand]]; break;
                    default: rhs = 0; break;
                }
                if (is_carry(regs[0], rhs, 1)) {
                    flags |= 8;
                } else {
                    flags &= ~8;
                }
                regs[0] = regs[0] * rhs;
                if (regs[0] == 0) flags |= 0x10;
                break;
            }

            /* ==================== AND ==================== */
            case AND_CONST:
            case AND_REG:
            case AND_ADDR:
            case AND_REG_ADDR:
                fb_puts("Hello!\n");
                switch (mode) {
                    case 0: regs[0] &= operand; break;
                    case 1: regs[0] &= regs[operand]; break;
                    case 2: regs[0] &= ram[operand]; break;
                    case 3: regs[0] &= ram[regs[operand]]; break;
                }
                break;

            /* ==================== OR ==================== */
            case OR_CONST:
            case OR_REG:
            case OR_ADDR:
            case OR_REG_ADDR:
                switch (mode) {
                    case 0: regs[0] |= operand; break;
                    case 1: regs[0] |= regs[operand]; break;
                    case 2: regs[0] |= ram[operand]; break;
                    case 3: regs[0] |= ram[regs[operand]]; break;
                }
                break;

            /* ==================== LOD ==================== */
            case LOD_REG:
            case LOD_ADDR:
            case LOD_REG_ADDR:
                switch (mode) {
                    case 0: fb_puts("InvailidOpcodeError\n"); return 6; break;
                    case 1: regs[operand] =  regs[0]; break;
                    case 2: ram[operand] = regs[0]; break;
                    case 3: ram[regs[operand]] = regs[0]; break;
                }
                break;

            /* ==================== JMP ==================== */
            case JMP_CONST:
            case JMP_REG:
            case JMP_ADDR:
            case JMP_REG_ADDR:
                switch (mode) {
                    case 0: mp = operand - 1; break;
                    case 1: mp = regs[operand] - 1; break;
                    case 2: mp = ram[operand] - 1; break;
                    case 3: mp = ram[regs[operand]] - 1; break;
                }
                break;

            /* ==================== CMP ==================== */
            case CMP_CONST:
            case CMP_REG:
            case CMP_ADDR:
            case CMP_REG_ADDR: {
                uint8_t rhs;
                switch (mode) {
                    case 0: rhs = operand; break;
                    case 1: rhs = regs[operand]; break;
                    case 2: rhs = ram[operand]; break;
                    case 3: rhs = ram[regs[operand]]; break;
                    default: rhs = 0; break;
                }
                if (regs[0] == rhs) {
                    flags |= 1;
                } else if (regs[0] > rhs) {
                    flags |= 2;
                } else if (regs[0] < rhs) {
                    flags |= 4;
                }
                if (regs[0] == 0) flags |= 0x10;
                break;
            }

            /* ==================== JCC ==================== */
            case JE:
            case JNE:
            case JG:
            case JNG:
            case JS:
            case JNS:
            case JGE:
            case JSE:
            case JZ:
            case JNZ:
            case JC:
            case JNC: {
                uint8_t taken = 0;
                switch (opcode_byte) {
                    case JE:  taken = (flags & 1); break;
                    case JNE: taken = !(flags & 1); break;
                    case JG:  taken = (flags & 2); break;
                    case JNG: taken = !(flags & 2); break;
                    case JS:  taken = (flags & 4); break;
                    case JNS: taken = !(flags & 4); break;
                    case JGE: taken = ((flags & 1) || (flags & 2)); break;
                    case JSE: taken = ((flags & 1) || (flags & 4)); break;
                    case JZ:  taken = (flags & 16); break;
                    case JNZ: taken = !(flags & 16); break;
                    case JC:  taken = (flags & 8); break;
                    case JNC: taken = !(flags & 8); break;
                    default: taken = 0; break;
                }
                if (taken) {
                    mp = operand - 1;
                }
                break;
            }

            /* ==================== PUSH ==================== */
            case PUSH_CONST:
            case PUSH_REG:
            case PUSH_ADDR:
            case PUSH_REG_ADDR: {
                uint8_t val;
                switch (mode) {
                    case 0: val = operand; break;
                    case 1: val = regs[operand]; break;
                    case 2: val = ram[operand]; break;
                    case 3: val = ram[regs[operand]]; break;
                    default: val = 0; break;
                }
                ram[sp] = val;
                if (sp < (sp - STACK_LENGTH + 1)) {
                    sp = 0xF0;
                } else {
                    sp--;
                }
                break;
            }

            /* ==================== POP ==================== */
            case POP_REG:
            case POP_ADDR:
            case POP_REG_ADDR:
                if (sp == 0) {
                    sp = 0xFF;
                } else {
                    sp++;
                }
                switch (mode) {
                    case 1: regs[operand] = ram[sp]; break;
                    case 2: ram[operand] = ram[sp]; break;
                    case 3: ram[regs[operand]] = ram[sp]; break;
                }
                ram[sp] = 0;
                break;

            /* ==================== CALL ==================== */
            case CALL_CONST:
            case CALL_REG:
            case CALL_ADDR:
            case CALL_REG_ADDR: {
                uint8_t target;
                switch (mode) {
                    case 0: target = operand; break;
                    case 1: target = regs[operand]; break;
                    case 2: target = ram[operand]; break;
                    case 3: target = ram[regs[operand]]; break;
                    default: target = 0; break;
                }
                ram[sp] = mp + 1;
                if (sp < (sp - STACK_LENGTH + 1)) {
                    sp = 0xF0;
                } else {
                    sp--;
                }
                mp = target - 1;
                break;
            }
        }
        mp++;
        ip++;
        sleep(1);
    }
    mp = 0;
    return 0;
}
