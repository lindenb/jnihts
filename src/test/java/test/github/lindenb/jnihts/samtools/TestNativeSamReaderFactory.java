package test.github.lindenb.jnihts.samtools;

import java.io.File;

import com.github.lindenb.jnihts.samtools.NativeHts;
import com.github.lindenb.jnihts.samtools.NativeSamReader;
import com.github.lindenb.jnihts.samtools.NativeSamReaderFactory;

import htsjdk.samtools.SAMRecord;
import htsjdk.samtools.SAMRecordIterator;
import htsjdk.samtools.SamReader;
import htsjdk.samtools.SamReaderFactory;
import htsjdk.samtools.ValidationStringency;

public class TestNativeSamReaderFactory {
	
	public static void main(String[] args) throws Exception{
		if( !NativeHts.isEnabled()) {
			System.err.println("Cannot load library");
			System.exit(-1);
		}
		System.err.println("OK load library");
		NativeSamReaderFactory factory = new NativeSamReaderFactory();
		String file="/commun/data/users/lindenb/jeter.bam";
		
		
		long now=System.currentTimeMillis();
		for(int i=0;i< 1000;++i)
		{
		NativeSamReader r= factory.open(file);
		for(;;) {
			SAMRecord rec=r.advance();
			if(rec==null) {
				break;
			}
			//if(i==0) System.out.print(rec.getSAMString());
		}
		r.close();
		}
		System.err.println("Native : "+ (System.currentTimeMillis()-now));
		
		now=System.currentTimeMillis();
		SamReaderFactory srf=SamReaderFactory.makeDefault().validationStringency(ValidationStringency.LENIENT);
		for(int i=0;i< 1000;++i)
		{
		SamReader r= srf.open(new File(file));
		SAMRecordIterator iter = r.iterator();
		while(iter.hasNext())
			{
			SAMRecord rec=iter.next();
			
			//if(i==0) System.out.print(rec.getSAMString());
			}
		r.close();
		}
		System.err.println("HTS : "+ (System.currentTimeMillis()-now));
		
		
	}
}