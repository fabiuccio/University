import java.util.Vector;

public class Method {
    public static final String PUBLIC = "public", PRIVATE = "private",
                               PROTECTED = "protected";
    private String comment="";
    private String visibility=PUBLIC;
    private String name="";
    private String body="";
    private String returnType="void";
    private Vector types;
    private Vector names;


    public Method (String methodName){
            name = methodName;
            names = new Vector(2);
            types = new Vector(2); 
    }	

    public void addArgument(String type, String name){
            names.add(name);
            types.add(type);
    }

    public void addArgument(String[] types, String[] names){
            for (int i = 0; i < names.length; i++)
                    this.names.add(names[i]);
            for (int i = 0; i < types.length; i++)
              this.types.add(types[i]);				
    }
    
    public void setVisibility(String visibility){
        this.visibility = visibility;
    }
    
    public void setReturnType(String retType){
        this.returnType= retType;
    }
    
    public void setDocumentation (String doc){
        comment=doc;
    }
    
    public void setBody(String body){
        String s="";
        for (int i = 0; i < body.length();i++){
            if (body.charAt(i)=='\n') s+="\n\t\t";
            else s+=body.charAt(i);
        }
        
        this.body = s;
    }
    
     public void addBody(String body){
        String s="";
        if (!body.equals("")) s="\n\t\t";
        
        for (int i = 0; i < body.length();i++){
            if (body.charAt(i)=='\n') s+="\n\t\t";
            else s+=body.charAt(i);
        }
        
        this.body += s;
    }
    
    public String toString(){
        String s="";
        if (comment.equals("")) s+="\n";
        else {
            s+="\t/**\n\t* ";
            for (int i = 0; i < comment.length();i++){
                if (comment.charAt(i)!='\n') s = s + comment.charAt(i);
                else s+="\n\t* ";
            }
            s+="\n\t**/\n";
        }
        
        s+= "\t"+visibility+" "+returnType+" "+ name+" (";
        for (int i = 0; i<Math.min(names.size(), types.size());i++){
                if (i!=0) s+=", ";
                s+= ((String)types.get(i))+" "+((String)names.get(i));
        }
        s+="){";
        if (!body.equals("")){
            s+="\n\t\t";  

            s+=body+"\n";

            s+="\t";
        }
        s+="}";
        return s;
    }
}