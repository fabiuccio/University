package society2;

import jade.lang.acl.MessageTemplate;
import jade.lang.acl.ACLMessage;
import jade.content.lang.Codec;
import jade.content.onto.Ontology;


/**
* This class has been automatically created by the AgentLang parser.
**/
public class comm_btw_agents {

	private String id = "e11982731";
	private String receiver = "";
	private int protocol = 0;
	private String ontology = "";
	private String content = "";


	public  comm_btw_agents (){}


	public  comm_btw_agents (int id, String receiver, int protocol, String ontology, String content){
		setId(id);
		setReceiver(receiver);
		setProtocol(protocol);
		setOntology(ontology);
		setContent(content);
	}


	public void setId (int id){
		this.id=id;
	}


	public void setReceiver (String receiver){
		this.receiver=receiver;
	}


	public void setProtocol (int protocol){
		this.protocol=protocol;
	}


	public void setOntology (String ontology){
		this.ontology=ontology;
	}


	public void setContent (String content){
		this.content=content;
	}


	public int getId (){
		return id;
	}


	public String getReceiver (){
		return receiver;
	}


	public int getProtocol (){
		return protocol;
	}


	public String getOntology (){
		return ontology;
	}


	public String getContent (){
		return content;
	}

}
