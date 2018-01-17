package society;

import jade.core.Agent;
import jade.core.AID;


/**
* 
* documentazione dell'agente agente ricevitore
* 
*  Documentazione relativa alla societa' e non al singolo agente: Questa e' la documentazione della societa' ad agenti...
*     
* 
**/
public class agenteReceiver extends Agent {

	private Blau ciao;
	private int coco;
	private String baubau;


	protected void setup (){
		
		addBehaviour(new receiverRole(this));
	}

}
