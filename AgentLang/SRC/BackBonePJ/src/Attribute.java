public class Attribute {
    public static final String PUBLIC = "public", PRIVATE = "private",
                               PROTECTED = "protected";

    public String getInit() {
        return init;
    }
    private String visibility=PRIVATE;
    private String type="";
    private String name="";
    private String modifier="";
    private String init="";

    public Attribute (String attribType, String attribName){
        name=attribName;
        type=attribType;
    }	
   
    public void setVisibility(String visibility){
        this.visibility = visibility;
    }
    
    public void setFinal(){
        modifier="final";
    }
    
    public void setInit(String initValue){
        init = initValue;
    }
    
    public String toString(){
        String s= "\t"+visibility+ " ";
        if (!modifier.equals("")) s=s+modifier+" ";
        s=s+type+" "+name;
        if (!init.equals("")) s=s+" = "+ init;
                
        s+=";";
        
        return s;
    }
}
