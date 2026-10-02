import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.*;
import ghidra.program.model.mem.*;
import ghidra.program.model.symbol.SourceType;
import java.math.BigInteger;
import java.io.ByteArrayInputStream;

public class SetupDataBender extends GhidraScript {
    public void run() throws Exception {
        Memory mem = currentProgram.getMemory();
        Address start = toAddr(0x08000000L);
        mem.getBlock(start).setExecute(true);
        currentProgram.getProgramContext().setValue(currentProgram.getRegister("TMode"), start, toAddr(0x0801a953L), BigInteger.ONE);
        byte[] init = new byte[0x3e4];
        mem.getBytes(toAddr(0x0801a570L), init);
        mem.createInitializedBlock("RAM_DATA", toAddr(0x24000000L), new ByteArrayInputStream(init), init.length, monitor, false);
        mem.createUninitializedBlock("RAM_BSS", toAddr(0x240003e4L), 0x80000L-0x3e4L, false);
        mem.createUninitializedBlock("DTCM", toAddr(0x20000000L), 0x20000L, false);
        mem.createUninitializedBlock("SDRAM", toAddr(0xc0000000L), 0x4000000L, false);
        mem.createUninitializedBlock("QSPI", toAddr(0x90000000L), 0x800000L, false);
        long[] known = {0x08000a08L,0x08003878L,0x080032fcL,0x080098fcL,0x080188e4L};
        String[] names = {"Reset_Handler", "main", "AudioCallback", "SystemInit_candidate", "libc_init_array_candidate"};
        for (int i=0;i<known.length;i++) { disassemble(toAddr(known[i])); createFunction(toAddr(known[i]),names[i]); }
        for (int off=4;off<0x298;off+=4) {
            long p=Integer.toUnsignedLong(mem.getInt(start.add(off)));
            if ((p&1)==1 && p>=0x080002a0L && p<0x08019c00L) {
                Address a=toAddr(p&~1L); disassemble(a); if(getFunctionAt(a)==null)createFunction(a,null);
            }
        }
        createLabel(toAddr(0x24000d68L),"audio_engine_state",true,SourceType.USER_DEFINED);
        createLabel(toAddr(0x24000ab8L),"control_state",true,SourceType.USER_DEFINED);
        createLabel(toAddr(0x24000cf8L),"clock_state",true,SourceType.USER_DEFINED);
        createLabel(toAddr(0x24000468L),"hardware_state",true,SourceType.USER_DEFINED);
    }
}
