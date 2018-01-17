package society2;

import jade.core.*;
import jade.core.behaviours.*;


/**
* This class has been automatically created by the AgentLang parser.
**/
public class sendingRole extends OneShotBehaviour {

	private int i;
	private int j;


	public  sendingRole (Agent aref){
		super(aref);
		
	}


	protected void action (){
		myAgent.addBehaviour(new serviceProviding(myAgent, myService));
		myAgent.addBehaviour(new serviceProviding2(myAgent, chilogrammi, temperatura));
		
	}



class serviceProviding extends CyclicBehaviour {
public serviceProviding(Agent myAgent, Object myService){
super(aref);
}public void action(){
//Add code here....
}
}

class serviceProviding2 extends CyclicBehaviour {
public serviceProviding2(Agent myAgent, Object chilogrammi, Object temperatura){
super(aref);
}public void action(){
//Add code here....
}
}



}
