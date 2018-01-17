package society2;

import jade.core.Agent;
import jade.core.AID;


/**
* 
* 
*  Documentazione relativa alla societa' e non al singolo agente: Questa e' la documentazione della societa' ad agenti...
*     
* 
**/
public class agenteMediatore extends Agent {

	private String names;


	protected void setup (){
		
		addBehaviour(new mediatoreRole(this));
		addBehaviour(new mediatoreRole2(this));
	}

}
