
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
        printf "ȡָ�����Υ��\n"
    end
    if ($var & 0x02)
        printf "��ȷ�����ݷ���Υ��\n"
    end
    if ($var & 0x04)
        printf "����ȷ�����ݷ���Υ��\n"
    end
    if ($var & 0x08)
        printf "��ջ����\n"
    end
    if ($var & 0x10)
        printf "��ջ����\n"
    end
    if ($var & 0x80)
        printf "��0x%08x����ָ������\n", *(uint32_t*)0xE000ED38
        info line *(*(uint32_t*)0xE000ED38)
    else
        printf "�޷�׷��Υ��ָ��\n"
    end
end

def mfsr 
    set $var = *(uint8_t*)0xE000ED28
    if ($var & 0x01)
        printf "ȡָ�����Υ��\n"
    end
    if ($var & 0x02)
        printf "���ݷ���Υ��\n"
    end
    if ($var & 0x08)
        printf "��ջ����\n"
    end
    if ($var & 0x10)
        printf "��ջ����\n"
    end
    if ($var & 0x80)
        printf "��0x%08x����ָ������\n", *(uint32_t*)0xE000ED34
        info line *(*(uint32_t*)0xE000ED34)
    else
        printf "�޷�׷��Υ��ָ��\n"
    end
end

def ufsr 
    set $var = *(uint16_t*)0xE000ED2A
    if ($var & 0x0001)
        printf "ָ��������\n"
    end
    if ($var & 0x0002)
        printf "������ͼ����ARM״̬\n"
    end
    if ($var & 0x0008)
        printf "�쳣����ʱ��ͼ�Ƿ��ļ���EXC_RETURN��PC\n"
    end
    if ($var & 0x0010)
        printf "������ͼִ��Э���������ָ��\n"
    end
    if ($var & 0x0100)
        printf "δ����\n"
    end
    if ($var & 0x0200)
        printf "����Ϊ��\n"
    end
end

def hfsr
    set $var=*(uint32_t*)0xE000ED2C
    if ($var & 0x00000002)
        printf "ȡ����ʱ����Ӳfault\n"
    end
    if ($var & 0x40000000)
        printf "�Ϸò���Ӳfault\n"
    end
    if ($var & 0x80000000)
        printf "���Բ���Ӳfault\n"
    end
end

def xf
    set $var = *(uint32_t*)0xE000ED04
    
    set $isr = ($var & 0x000003FF)
    printf "��ǰ���ISR : %d\n", $isr  
    if ($isr == 3)
        printf "=====Ӳfault=====:\n"
        hfsr
    end
    
    set $var = *(uint32_t*)0xE000ED24
    if ($var & 0x00000001)
        printf "=====�洢��fault=====:\n"
        mfsr
    end
    if ($var & 0x00000002)
        printf "=====����fault=====:\n"
        bfsr
    end
    if ($var & 0x00000008)
        printf "=====�÷�fault=====:\n"
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

