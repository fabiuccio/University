import junit.framework.TestCase;
import hcs.dataType.Init_Section;



public class Test1 extends TestCase{

	public void test_1(){
			
		double r_sizes[] = {20.0,34.5}; 
		Init_Section sec1 = new Init_Section(r_sizes,1);
		
		 
		
		assertEquals(2, sec1.getNumber());
		
	}
	
	public void test_2(){
		double r_sizes[] = {32.0,20.6}; 
		Init_Section sec2 = new Init_Section(r_sizes,2);
		assertEquals(1, sec2.getNumber());
	}
}
