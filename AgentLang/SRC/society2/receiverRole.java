package society2;

import jade.core.*;
import jade.core.behaviours.*;


/**
* This class has been automatically created by the AgentLang parser.
**/

public class receiverRole extends OneShotBehaviour {

	private String i;
	private String j;


	public  receiverRole (Agent aref){
		super(aref);
		
	}


	protected void action (){
		myAgent.addBehaviour(new serviceRequesting(myAgent, myRequest, requestType));
		
	}



class serviceRequesting extends CyclicBehaviour {
public serviceRequesting(Agent myAgent, Object myRequest, Object requestType){
super(aref);
}public void action(){
//Add code here....
}
}



}
