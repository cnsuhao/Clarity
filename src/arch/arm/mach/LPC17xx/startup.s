    .section ".interruptVector"
    .globl  interruptVector
    .type   interruptVector, %object
interruptVector:
    .word    stack
    .word    resetHandler
    .word    nmiHandler
    .word    hardFaultHandler
    .word    memManageHandler
    .word    busFaultHandler
    .word    usageFaultHandler
    .word    0
    .word    0
    .word    0
    .word    0
    .word    svcHandler
    .word    debugMonHandler
    .word    0
    .word    pendSVHandler
    .word    sysTickHandler

/*ExternalInterrupts*/
    .word    defaultHandler /*16:WatchdogTimer*/
    .word    defaultHandler /*17:Timer0*/
    .word    defaultHandler /*18:Timer1*/
    .word    defaultHandler /*19:Timer2*/
    .word    defaultHandler /*20:Timer3*/
    .word    defaultHandler /*21:UART0*/
    .word    defaultHandler /*22:UART1*/
    .word    defaultHandler /*23:UART2*/
    .word    defaultHandler /*24:UART3*/
    .word    defaultHandler /*25:PWM1*/
    .word    defaultHandler /*26:I2C0*/
    .word    defaultHandler /*27:I2C1*/
    .word    defaultHandler /*28:I2C2*/
    .word    defaultHandler /*29:SPI*/
    .word    defaultHandler /*30:SSP0*/
    .word    defaultHandler /*31:SSP1*/
    .word    defaultHandler /*32:PLL0Lock(MainPLL)*/
    .word    defaultHandler /*33:RealTimeClock*/
    .word    defaultHandler /*34:ExternalInterrupt0*/
    .word    defaultHandler /*35:ExternalInterrupt1*/
    .word    defaultHandler /*36:ExternalInterrupt2*/
    .word    defaultHandler /*37:ExternalInterrupt3*/
    .word    defaultHandler /*38:A/DConverter*/
    .word    defaultHandler /*39:Brown-OutDetect*/
    .word    defaultHandler /*40:USB*/
    .word    defaultHandler /*41:CAN*/
    .word    defaultHandler /*42:GeneralPurposeDMA*/
    .word    defaultHandler /*43:I2S*/
    .word    defaultHandler /*44:Ethernet*/
    .word    defaultHandler /*45:RepetitiveInterruptTimer*/
    .word    defaultHandler /*46:MotorControlPWM*/
    .word    defaultHandler /*47:QuadratureEncoderInterface*/
    .word    defaultHandler /*48:PLL1Lock(USBPLL)*/
    .word    defaultHandler /*49:USBActivity*/
    .word    defaultHandler /*50:CANActivity*/

    .data
val1:
    .4byte   10
val2:
    .4byte   30
result:
    .space   4
    .text
    .thumb
    .thumb_func
    .syntax  unified
    .type    resetHandler, %function
resetHandler:
    ldr      r0, =flashRamStart
    ldr      r1, =ramStart
    ldr      r2, =ramSize
copy:
    ldrb     r4, [r0], #1
    strb     r4, [r1], #1
    subs     r2, r2, #1
    bne      copy

    /*Add and store result.*/
    ldr      r0, =val1
    ldr      r1, =val2

    ldr      r2, [r0]
    ldr      r3, [r1]

    add      r4, r2, r3

    ldr      r0, =result
    str      r4, [r0]
    ldr      r0, =main
    bx       r0
    b        .

    .weak    nmiHandler
    .type    nmiHandler, %function
nmiHandler:
    b        .

    .weak    hardFaultHandler
    .type    hardFaultHandler, %function
hardFaultHandler:
    b        .

    .weak    memManageHandler
    .type    memManageHandler, %function
memManageHandler:
    b        .

    .weak    busFaultHandler
    .type    busFaultHandler, %function
busFaultHandler:
    b        .

    .weak    usageFaultHandler
    .type    usageFaultHandler, %function
usageFaultHandler:
    b        .

    .weak    svcHandler
    .type    svcHandler, %function
svcHandler:
    b        .

    .weak    debugMonHandler
    .type    debugMonHandler, %function
debugMonHandler:
    b        .

    .weak    pendSVHandler
    .type    pendSVHandler, %function
pendSVHandler:
    b        .

    .weak    sysTickHandler
    .type    sysTickHandler, %function
sysTickHandler:
    b        .

    .weak    defaultHandler
    .type    defaultHandler, %function
defaultHandler:
    b        .

    .end
