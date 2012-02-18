#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX 256
#define error(msg) do {printf("error on line %d\n", src_line); printf msg; exit(1);}while(0);

int src_line;

#define INST_LIST \
    X(Add,      NONE,   INTINT, pushi(vm, a + b)) \
    X(Subtract, NONE,   INTINT, pushi(vm, a - b)) \
    X(Multiply, NONE,   INTINT, pushi(vm, a * b)) \
    X(Divide,   NONE,   INTINT, pushi(vm, a / b)) \
    X(Negate,   NONE,   INT, pushi(vm, -a)) \
    X(Remainder,NONE,   INTINT, pushi(vm, a % b)) \
\
    X(FAdd,     NONE,   FLOATFLOAT, pushf(vm, a + b)) \
    X(FSubtract,NONE,   FLOATFLOAT, pushf(vm, a- b)) \
    X(FNegate,  NONE,   FLOAT,      pushf(vm, -a)) \
    X(FMultiply,NONE,   FLOATFLOAT, pushf(vm, a* b)) \
    X(FDivide,  NONE,   FLOATFLOAT, pushf(vm, a/ b)) \
\
    X(And,      NONE,   INTINT, pushi(vm, a && b)) \
    X(Or,       NONE,   INTINT, pushi(vm, a || b)) \
    X(Nand,     NONE,   INTINT, pushi(vm, !(a && b))) \
    X(Nor,      NONE,   INTINT, pushi(vm, !(a || b))) \
    X(Xor,      NONE,   INTINT, pushi(vm, !!(a ^ b))) \
    X(BEqual,   NONE,   INTINT, pushi(vm, !(a ^ b))) \
    X(BNegate,  NONE,   INT,    pushi(vm, !a)) \
\
    X(BTAnd,    NONE,   INTINT, pushi(vm, a & b)) \
    X(BTOr,     NONE,   INTINT, pushi(vm, a | b)) \
    X(BTNand,   NONE,   INTINT, pushi(vm, ~(a & b))) \
    X(BTNor,    NONE,   INTINT, pushi(vm, ~(a | b))) \
    X(BTXor,    NONE,   INTINT, pushi(vm, a ^ b)) \
    X(BTEqual,  NONE,   INTINT, pushi(vm, ~(a ^ b))) \
    X(BTNegate, NONE,   INT,    pushi(vm, ~a)) \
\
    X(ConvertI, NONE,   FLOAT, pushi(vm, a)) \
    X(ConvertF, NONE,   INT,   pushf(vm, a)) \
\
    X(Duplicate,NONE,   ANY,     push(vm, a); push(vm, a)) \
    X(Exchange, NONE,   ANYANY,  push(vm, b); push(vm, a)) \
\
    X(Pop,      NONE,   ANY,     ;) \
    X(PushI,    INT,    NONE,    pushi(vm, pgm[pc].intarg)) \
    X(PushD,    STRING, NONE,    pushi(vm, locate(vm, pgm[pc].strarg))) \
    X(PushF,    FLOAT,  NONE,    pushf(vm, pgm[pc].floatarg)) \
    X(PushPC,   NONE,   NONE,    pushi(vm, pc + 1)) \
    X(PopPC,    NONE,   INT,     pc = a) \
    X(LoadC,    NONE,   INT,     pushi(vm, vm->Mem[a])) \
    X(LoadI,    NONE,   INT,     pushi(vm, *((uint32_t *)(&vm->Mem[a])))) \
    X(LoadF,    NONE,   INT,     pushf(vm, *((double *)(&vm->Mem[a])))) \
    X(StoreC,   NONE,   INTINT,  vm->Mem[a] = (uint8_t)b) \
    X(StoreI,   NONE,   INTINT,  *((uint32_t *)(&vm->Mem[a])) = b) \
    X(StoreF,   NONE,   INTFLOAT, *((double *)(&vm->Mem[a])) = b) \
    X(Memtop,   NONE,   NONE,    pushi(vm, MAX)) \
    X(Jump,     STRING, NONE,    pc = locate(vm, pgm[pc].strarg)) \
    X(JumpFalse,STRING, INT,     if (!a) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpTrue, STRING, INT,     if (a) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpNeg,  STRING, INT,     if (a < 0) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpPos,  STRING, INT,     if (a > 0) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpFNeg, STRING, FLOAT,   if (a < 0.f) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpFPos, STRING, FLOAT,   if (a > 0.f) pc = locate(vm, pgm[pc].strarg)) \
    X(JumpFZero,STRING, FLOAT,   if (a == 0.0) pc = locate(vm, pgm[pc].strarg)) \
\
    X(Call,     STRING, NONE,    pushi(vm, pc+1); pc = locate(vm, pgm[pc].strarg)) \
    X(Return,   NONE,   INT,     pc = a) \
    X(Halt,     NONE,   NONE,    ;) \
    X(Nop,      NONE,   NONE,    ;) \
    X(Printf,   NONE,   INT,     print(vm, a)) \
    X(Dump, NONE, NONE, dump(vm))


int streq(const char *a, const char *b){
    return strcmp(a, b) == 0;
}

enum arg_type {
    NONE,
    INT,
    FLOAT,
    INTINT,
    FLOATFLOAT,
    INTFLOAT,
    ANY,
    ANYANY,
    STRING
};

enum inst_type {
#define X(inst, source_arg, stack_args, code) inst,
    INST_LIST
#undef X
};

struct inst_decl {
    char *name;
    enum inst_type type;
    enum arg_type arg; 
} inst_types[] = {
#define X(inst, source_arg, stack_args, code) {#inst, inst, source_arg},
    INST_LIST
#undef X
};

struct vm {
    struct label {
        char *name;
        int addr;
    } labels[MAX];

    int nlabels;

    unsigned char Mem[MAX];

    struct stack_entry {
        enum arg_type type;
        int intval;
        double floatval;
    } stack[MAX];

    int sp;
};

void dump(struct vm *vm){
    printf("stack: ");
    int i;
    for(i=1;i<=vm->sp;i++){
        printf("[%d ", i);
        switch(vm->stack[i].type){
        case INT:
            printf("int] %d   ", vm->stack[i].intval);
            break;
        case FLOAT:
            printf("float]%lf   ", vm->stack[i].floatval);
            break;
        }
    }
    printf("\n");
}

void add_label(struct vm *vm, char *name, int addr){
    vm->labels[vm->nlabels].name = strdup(name);
    vm->labels[vm->nlabels].addr = addr;
    vm->nlabels++;
}

int locate(struct vm *vm, const char *label){
    int i;

    for(i=0;i<vm->nlabels;i++){
        if (streq(label, vm->labels[i].name))
            return vm->labels[i].addr;
    }

    error(("label not found: %s\n", label));
}

struct stack_entry pop(struct vm *vm){
    if (vm->sp < 1)
        error(("tried to pop from an empty stack"));

    return vm->stack[vm->sp--];
}

double popf(struct vm *vm){
    struct stack_entry e = pop(vm);

    if (e.type != FLOAT)
        error(("invalid stack data, expected float"));

    return e.floatval;
}

int popi(struct vm *vm){
    struct stack_entry e = pop(vm);

    if (e.type != INT)
        error(("invalid stack data, expected int"));

    return e.intval;
}

int print(struct vm *vm, int addr){
    int arg_count = 0;
    unsigned char *fmt = vm->Mem + addr;
    while (*fmt){
        if (*fmt == '%'){
            if (fmt[1] == 'd'){
                printf("%d", popi(vm));
            } else if (fmt[1] == 'f'){
                printf("%lf", popf(vm));
            }
            arg_count++;
            fmt += 2;
        } else {
            putchar(*fmt);
            fmt++;
        }
    }

    return arg_count;
}

void push(struct vm *vm, struct stack_entry entry){
    vm->sp++;
    vm->stack[vm->sp] = entry;
}

void pushi(struct vm *vm, int val){
    struct stack_entry e = {.intval = val,
                            .type = INT};
    push(vm, e);
}

void pushf(struct vm *vm, double val){
    struct stack_entry e = {.floatval = val,
                            .type = FLOAT};
    push(vm, e);
}

struct inst_decl *lookup_inst(char *name){
    unsigned i;

    for(i=0;i<sizeof(inst_types)/sizeof(inst_types[0]);i++){
        if (streq(name, inst_types[i].name))
            return &inst_types[i];
    }
    
    return NULL;
}

struct pgm_entry {
    enum inst_type type;
    int intarg;
    double floatarg;
    char *strarg;
    int src_line;
};

#define grab_args_INTFLOAT \
    double b = popf(vm); \
    int a = popi(vm);

#define grab_args_INTINT \
    int b = popi(vm); \
    int a = popi(vm);

#define grab_args_INT \
    int a = popi(vm);

#define grab_args_FLOAT \
    double a = popf(vm);

#define grab_args_FLOATFLOAT \
    double b = popf(vm); \
    double a = popf(vm);

#define grab_args_NONE

#define grab_args_ANY \
    struct stack_entry a = pop(vm);

#define grab_args_ANYANY \
    struct stack_entry b = pop(vm); \
    struct stack_entry a = pop(vm);

void run(struct vm *vm, struct pgm_entry *pgm){
    int pc = 0;
    int is_jump;

    while (pgm[pc].type != Halt){
        is_jump = (pgm[pc].type == Jump);

        src_line = pgm[pc].src_line;
fprintf(stderr,"line: ");
char buf[1024];sprintf(buf, "cat minuserror.asm | head -n %d | tail -n 1", src_line);system(buf); 

        switch(pgm[pc].type){
#define X(inst, source_arg, stack_args, code) \
        case inst: \
            { \
            grab_args_##stack_args; \
            code; \
            } \
            break;
INST_LIST
#undef X
        }
dump(vm);
        if (!is_jump){
            pc++;
        }
    }
}

int main(){
    int intarg;
    double floatarg;
    char buf[MAX];
    int pgm_ptr = 0;
    int mem_ptr = 0;
    int src_line = 1;

    struct inst_decl *inst;
    struct pgm_entry newentry;
    struct pgm_entry pgm[MAX];

    struct vm vm = {{0}};

    while (scanf("%s", buf) == 1){
        if (streq(buf, "Label")){
            scanf("%s", buf);
            add_label(&vm, buf, pgm_ptr);
        } else if (streq(buf, "DLabel")){
            scanf("%s", buf);
            add_label(&vm, buf, mem_ptr);
        } else if (streq(buf, "DataZ")){
            scanf("%d", &intarg);
            memset(vm.Mem + mem_ptr, '\0', intarg);
            mem_ptr += intarg;
        } else if (streq(buf, "DataC")){
            scanf("%d", &intarg);
            vm.Mem[mem_ptr] = intarg;
            mem_ptr++;
        } else if (streq(buf, "DataI")){
            scanf("%d", &intarg);
            *(uint32_t *)(vm.Mem+mem_ptr) = intarg;
        } else if (streq(buf, "DataF")){
            scanf("%lf", &floatarg);
            *(double *)(vm.Mem+mem_ptr) = floatarg;
        } else if ((inst = lookup_inst(buf))){
            switch (inst->arg){
            case INT:
                scanf("%d", &newentry.intarg);
                break;
            case STRING:
                scanf("%s", buf);
                newentry.strarg = strdup(buf);
                break;
            case FLOAT:
                scanf("%lf", &newentry.floatarg);
                break;
            }
            newentry.type = inst->type;
            newentry.src_line = src_line;
            pgm[pgm_ptr++] = newentry;
        } else {
            error(("unrecognized instruction: %s\n", buf));
        }
        fgets(buf, sizeof(buf), stdin); /* eat the rest of the line */
        src_line++;
    }
            
    run(&vm, pgm);

    return 0;
}
