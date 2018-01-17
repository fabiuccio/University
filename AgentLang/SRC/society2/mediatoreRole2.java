package society2;

import jade.core.*;
import jade.core.behaviours.*;


/**
* This class has been automatically created by the AgentLang parser.
**/
public class mediatoreRole2 extends OneShotBehaviour {

	private Boolean bool2;
	private Char charity2;


	public  mediatoreRole2 (Agent aref){
		super(aref);
		
	}


	protected void action (){
		myAgent.addBehaviour(new mediatore2(myAgent, agent1, agent2));
		
	}



class mediatore2 extends CyclicBehaviour {
public mediatore2(Agent myAgent, Object agent1, Object agent2){
super(aref);
}public void action(){
//Add code here....
}
}



}
