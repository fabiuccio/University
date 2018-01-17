import java.util.Vector;

/**
 * Questa classe é utilizzata come supporto per la creazione di una classe (che possa essere o
 * meno un Agent)
 */
public class Class {
    private String name="";
    private String comment="";
    private String nestedClass="";
    private Vector pack;
    private String ext ="";
    private Vector methods;
    private Vector imports;
    private Vector attributes;
    private int type;
    public static final int UNDEFINED=-1, SOCIETY=0, AGENT=1, ROLE=2, TASK=3, COMMUNICATION=4; 
            
    public Class (String className){
        name = className;    
        methods = new Vector(2);
        attributes = new Vector(2);
        pack = new Vector(2);
        imports = new Vector(2);
        type=UNDEFINED;
    }

    public void addPackage(String packageName){
        pack.add(packageName);
    }	
    
    public void addImport(String library){
        imports.add(library);
    }
    public void setDocumentation (String doc){
        comment=doc;
    }
    public void addDocumentation (String doc){
        comment=comment+"\n"+doc;
    }
    public void setNestedClass(String body){
        nestedClass=body;
    }
    public void addNestedClass(String body){
        nestedClass = nestedClass + "\n\n"+body;
    }
    public void setExtends(String extend, String libraryToImport){
        ext = extend;
        if (libraryToImport!=null) imports.add(libraryToImport);
    }
    
    public void addMethod(Method m){
            methods.add(m);
    }
    
    public void addAttribute(Attribute a){
            attributes.add(a);
    }
    
    public String getName(){return name;}
    
    public void setType(int type){
        this.type = type;
    }
        
    public int getType(){return type;}
    
    public String toString(){
        String stringa ="";
        if (pack.size()>0){
            stringa = stringa + "package ";
            
            for (int i = pack.size()-1; i>=0;i--){
                stringa+=(String)pack.get(i);
                if (i!=0) stringa+=".";
            }
            stringa +=";\n";
                        
            
        }
        
        if (imports.size()>0)stringa+="\n"; // Se ci sono import si aggiunge una sola riga
        else stringa+="\n\n";               // altrimenti se non ci sono import si passa direttamente alla doppia riga
        
        for (int i = 0; i < imports.size();i++)
            stringa = stringa+"import "+ ((String) imports.get(i)) + ";\n";
        
        if (imports.size()>0) stringa+="\n\n"; //Se ci sono import la doppia riga si mette qui, altrimenti
                                               //è già stata messa in precedenza...
        
        if (comment.equals("")) stringa+="/**\n* This class has been automatically created by the AgentLang parser.\n**/\n";
        else {
            stringa+="/**\n* ";
            for (int i = 0; i < comment.length();i++){
                if (comment.charAt(i)!='\n') stringa = stringa + comment.charAt(i);
                else stringa+="\n* ";
            }
            stringa+="\n**/\n";
        }
        
        stringa = stringa + "public class "+ name +" ";
        if (!ext.equals("")) stringa+="extends "+ext+" ";
        stringa+="{\n\n";
        
        for (int i = 0; i < attributes.size();i++){
            stringa+=((Attribute) attributes.get(i)).toString()+"\n"; //no righe vuote tra attributi
        }
        
        if (attributes.size()>0) stringa+="\n";
        
        for (int i = 0; i < methods.size();i++){
            stringa+=((Method) methods.get(i)).toString()+"\n\n";
        }
        
        if (!nestedClass.equals("")){
            stringa+=nestedClass;
            stringa+="\n\n";
        }
        stringa+="}\n";
        
        return stringa;
    }
}