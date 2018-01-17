package society;

import jade.core.*;
import jade.core.behaviours.*;


/**
* This class has been automatically created by the AgentLang parser.
**/
public class mediatoreRole extends OneShotBehaviour {

	private int a;
	private int b;


	public  mediatoreRole (Agent aref){
		super(aref);
		
	}


	protected void action (){
		myAgent.addBehaviour(new mediatore(myAgent, agent1, agent2));
		
	}



class mediatore extends CyclicBehaviour {
public mediatore(Agent myAgent, Object agent1, Object agent2){
super(aref);
}public void action(){
//Add code here....
}
}



}
