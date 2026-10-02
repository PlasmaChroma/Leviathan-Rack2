import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;

public class ExportDataBender extends GhidraScript {
    public void run() throws Exception {
        File out=new File(getScriptArgs()[0]); out.mkdirs();
        DecompInterface d=new DecompInterface(); d.openProgram(currentProgram);
        PrintWriter index=new PrintWriter(new File(out,"functions.tsv"),"UTF-8");
        PrintWriter full=new PrintWriter(new File(out,"all_functions.c"),"UTF-8");
        index.println("address\tname\tbody_bytes\tcallers\tcallees\tdecompiled");
        FunctionIterator fs=currentProgram.getFunctionManager().getFunctions(true);
        int count=0,ok=0;
        while(fs.hasNext()&&!monitor.isCancelled()) {
            Function f=fs.next();
            if(f.getEntryPoint().getOffset()<0x08000000L||f.getEntryPoint().getOffset()>=0x0801a954L)continue;
            String hex=f.getEntryPoint().toString();
            DecompileResults r=d.decompileFunction(f,45,monitor);
            String code=r.decompileCompleted()?r.getDecompiledFunction().getC():"/* Decompilation failed: "+r.getErrorMessage()+" */";
            try(PrintWriter p=new PrintWriter(new File(out,hex+".c"),"UTF-8")){p.println("/* "+hex+" "+f.getName()+"; analyst naming is provisional. */");p.println(code);}
            full.println("\n/* === "+hex+" "+f.getName()+" === */\n"+code);
            index.println(hex+"\t"+f.getName()+"\t"+f.getBody().getNumAddresses()+"\t"+f.getCallingFunctions(monitor).size()+"\t"+f.getCalledFunctions(monitor).size()+"\t"+r.decompileCompleted());
            count++;if(r.decompileCompleted())ok++;
        }
        full.close();index.close();d.dispose();println("EXPORT functions="+count+" decompiled="+ok);
        try(PrintWriter listing=new PrintWriter(new File(out,"analyzed_instructions.tsv"),"UTF-8")) {
            listing.println("address\tbytes\tinstruction\tfunction");
            InstructionIterator ii=currentProgram.getListing().getInstructions(true);
            while(ii.hasNext()){
                Instruction ins=ii.next();long a=ins.getAddress().getOffset();
                if(a<0x08000000L||a>=0x0801a954L)continue;
                StringBuilder hex=new StringBuilder();for(byte b:ins.getBytes())hex.append(String.format("%02x",b&255));
                Function f=getFunctionContaining(ins.getAddress());
                listing.println(ins.getAddress()+"\t"+hex+"\t"+ins.toString()+"\t"+(f==null?"":f.getName()));
            }
        }
    }
}
