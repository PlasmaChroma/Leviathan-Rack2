import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.nio.charset.StandardCharsets;

public class NameDataBender extends GhidraScript {
    public void run() throws Exception {
        int count=0;
        for(String line:Files.readAllLines(Paths.get(getScriptArgs()[0]),StandardCharsets.UTF_8)) {
            if(line.isEmpty()||line.startsWith("address"))continue;
            String[] cols=line.split("\t");
            long address=Long.parseUnsignedLong(cols[0].replace("0x",""),16);
            Function f=getFunctionAt(toAddr(address));
            if(f==null){disassemble(toAddr(address));f=createFunction(toAddr(address),null);}
            if(f!=null){f.setName(cols[1],SourceType.USER_DEFINED);f.setComment("Analysis-assigned semantic name; not a recovered original source symbol. Verify prototypes and FP conditional expressions against ARM instructions and emulator evidence.");count++;}
        }
        println("Named "+count+" function candidates.");
    }
}
