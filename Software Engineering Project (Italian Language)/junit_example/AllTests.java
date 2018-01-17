import junit.framework.Test;
import junit.framework.TestSuite;


public class AllTests {
	
	public static Test suite() {

        TestSuite suite = new TestSuite();
	     
        suite.addTestSuite(Test1.class);

       

        return suite;
    }

}
