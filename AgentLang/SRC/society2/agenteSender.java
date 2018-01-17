package society2;

import jade.core.Agent;
import jade.core.AID;


/**
* 
* documentazione dell'agente agente1
* 	questa e' una documentazione su piu'
* 	linee
* 						
* 
*  Documentazione relativa alla societa' e non al singolo agente: Questa e' la documentazione della societa' ad agenti...
*     
* 
**/
public class agenteSender extends Agent {

	private Persona persona1;
	private int counter;
	private String nomeApplicazione;
	private String idApplicazione;


	protected void setup (){
		
		addBehaviour(new sendingRole(this));
	}

}
