package test.github.lindenb.jnihts.samtools;

import com.github.lindenb.jnihts.samtools.NativeHts;
import com.github.lindenb.jnihts.samtools.NativeSamReader;
import com.github.lindenb.jnihts.samtools.NativeSamReaderFactory;

import htsjdk.samtools.SAMRecord;

public class TestNativeSamReaderFactory {
	
	public static void main(String[] args) throws Exception{
		if( !NativeHts.isEnabled()) {
			System.err.println("Cannot load library");
			System.exit(-1);
		}
		System.err.println("OK load library");
		NativeSamReaderFactory factory = new NativeSamReaderFactory();
		NativeSamReader r= factory.open("/commun/data/users/lindenb/jeter.bam");
		for(;;) {
			SAMRecord rec=r.advance();
			if(rec==null) {
				System.err.println("break");
				break;
			}
			System.out.print(rec.getSAMString());
		}
		System.err.println(r.getFileHeader());
		r.close();
	}
}