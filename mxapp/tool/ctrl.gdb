
set print pretty on
set print array on

define fp 
    f $arg0
    set $var=$sp
    f $arg0+1
    p /d $sp-$var
end

define fprange
    set $var=$arg0
    while ($var<=$arg1)
        fp $var
        set $var=$var+1
    end
end

def bfsr 
    set $var = *(uint8_t*)0xE000ED29
    if ($var & 0x01)
        printf "取指令访问违规\n"
    end
    if ($var & 0x02)
        printf "精确的数据访问违规\n"
    end
    if ($var & 0x04)
        printf "不精确的数据访问违规\n"
    end
    if ($var & 0x08)
        printf "出栈错误\n"
    end
    if ($var & 0x10)
        printf "入栈错误\n"
    end
    if ($var & 0x80)
        printf "由0x%08x处的指令引起\n", *(uint32_t*)0xE000ED38
        info line *(*(uint32_t*)0xE000ED38)
    else
        printf "无法追踪违规指令\n"
    end
end

def mfsr 
    set $var = *(uint8_t*)0xE000ED28
    if ($var & 0x01)
        printf "取指令访问违规\n"
    end
    if ($var & 0x02)
        printf "数据访问违规\n"
    end
    if ($var & 0x08)
        printf "出栈错误\n"
    end
    if ($var & 0x10)
        printf "入栈错误\n"
    end
    if ($var & 0x80)
        printf "由0x%08x处的指令引起\n", *(uint32_t*)0xE000ED34
        info line *(*(uint32_t*)0xE000ED34)
    else
        printf "无法追踪违规指令\n"
    end
end

def ufsr 
    set $var = *(uint16_t*)0xE000ED2A
    if ($var & 0x0001)
        printf "指令解码错误\n"
    end
    if ($var & 0x0002)
        printf "程序视图切入ARM状态\n"
    end
    if ($var & 0x0008)
        printf "异常返回时视图非法的加载EXC_RETURN到PC\n"
    end
    if ($var & 0x0010)
        printf "程序视图执行协处理器相关指令\n"
    end
    if ($var & 0x0100)
        printf "未对齐\n"
    end
    if ($var & 0x0200)
        printf "除数为零\n"
    end
end

def hfsr
    set $var=*(uint32_t*)0xE000ED2C
    if ($var & 0x00000002)
        printf "取向量时发生硬fault\n"
    end
    if ($var & 0x40000000)
        printf "上访产生硬fault\n"
    end
    if ($var & 0x80000000)
        printf "调试产生硬fault\n"
    end
end

def xf
    set $var = *(uint32_t*)0xE000ED04
    
    set $isr = ($var & 0x000003FF)
    printf "当前活动的ISR : %d\n", $isr  
    if ($isr == 3)
        printf "=====硬fault=====:\n"
        hfsr
    end
    
    set $var = *(uint32_t*)0xE000ED24
    if ($var & 0x00000001)
        printf "=====存储器fault=====:\n"
        mfsr
    end
    if ($var & 0x00000002)
        printf "=====总线fault=====:\n"
        bfsr
    end
    if ($var & 0x00000008)
        printf "=====用法fault=====:\n"
        ufsr
    end
end

def delb 
    delete break $arg0
end

def disb
    disable break $arg0
end

def pf5
    p *(d1f5_t*)(ss->ptr.f5)
end

def pf6
    p *(d1f6_t*)(ss->ptr.f6)
end

target remote localhost:2331
monitor flash device = STM32F207ZG
monitor flash download = 1
monitor flash breakpoints = 1
monitor clrbp
monitor endian little
monitor reset
monitor sleep 100
monitor speed auto
load 
monitor writeu32 0xE000ED08 = 0x00000000
monitor reg r13 = (0x00000000)
monitor reg pc = (0x00000004)
b main

