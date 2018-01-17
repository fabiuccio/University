import java.io.*;
import java.util.Vector;

public class BackBone {
    final String PATH;
    private String logPath="";
    public final static String SYNTAX="syntax", SEMANTIC="semantic";
    private Vector <Class> classes;
    private Vector <String[]> errorLog = null;
    private String pack="";
   
    public BackBone() {
        classes = new Vector(2);
        PATH=".\\";
        logPath=".\\";
    }
     
    public BackBone(String packageName) {
        classes = new Vector(2);
        pack=packageName;
        PATH=".\\"+pack+"\\";
        logPath=".\\";
    }
    
    public void addClass(Class c){
        if (!pack.equals("")) c.addPackage(pack);
        classes.add(c);
    }
    
    public void addErrorLog(String errorType, String errorDescription){
        if (errorLog==null){
            errorLog = new Vector();
        }
        errorLog.add(new String[]{errorType, errorDescription});
    }
    public void setErrorLog(Vector <String[]> errorLog){
        this.errorLog = errorLog;
    }
    public Vector <String[]> getErrorLog(){
        return errorLog;
    }
    public String getPackage(){
        return pack;
    }
    
    /**
     *  Settare il package in corsa è un'operazione scoraggiata
     *  perché vi sarebbe una discrasia tra package delle classi
     *  aggiunte prima e dopo la modifica del package.
     *  Se proprio è necessario, creare un metodo ad hoc che tenga
     *  conto dei problemi di sincronizzazione tra i package delle classi.<br>
     *  <br>
     *  Questo metodo al suo interno non fa nulla! occorre l'overriding
     *  per fargli compiere operazioni.<br>
     *
     */
    protected void setPackage(String s){
        
    }
    
    public void toFile(){
        toFile(PATH);
    }
    
    public void toFile(String path){
       if (errorLog!=null)
           try{
                FileWriter fout = new FileWriter(logPath+"Error.log");
                fout.write("* * * Log degli errori nel parsing di AgentLang * * *\n\n");
                fout.write("Sono stati trovati errori durante il parsing del documento. E' stato creato\ncomunque il"+
                           "codice parziale fino al verificarsi dell'errore. Si noti che il\ncodice può risultare incompilabile "+
                           "e/o inconsistente in una o in più delle\nsue parti.\n\nRisolvere dunque i seguenti "+errorLog.size()+" errori:\n\n");
                for (int i = 0; i < errorLog.size();i++){
                    if (errorLog.get(i)[0].equals(SYNTAX)){
                        fout.write("[SYNTAX ERROR]: "+errorLog.get(i)[1]+"\n");
                        System.out.println("[SYNTAX ERROR]: "+errorLog.get(i)[1]);
                    }
                    if (errorLog.get(i)[0].equals(SEMANTIC)){
                        fout.write("[SEMANTIC ERROR]: "+errorLog.get(i)[1]+"\n");
                        System.out.println("[SEMANTIC ERROR]: "+errorLog.get(i)[1]);
                    }
                }
               
                
                fout.flush();
                fout.close();
            }catch(IOException io){
                io.printStackTrace();
            }
       
        try{
            for (int i = 0; i < classes.size();i++){
                Class tempClass = (Class) classes.get(i);
                File f = new File(path);
                if (!f.exists()) f.mkdir();
                FileWriter fout = new FileWriter(path+tempClass.getName()+".java");

                fout.write(tempClass.toString());

                fout.flush();
                fout.close();
            }
        }catch(IOException io){
            io.printStackTrace();
        }
    }
    
    public static void main (String args[]){
        BackBone bb = new BackBone("myagentsociety");
            Class b = new Class ("MioAgente");
            b.setExtends("Agent", "jade.core.Agent");
                Method m = new Method("setup");
                m.addArgument("Agent", "aref");
                m.setVisibility(Method.PROTECTED);
                b.addMethod(m);

                Method m2 = new Method("takeDown");
                m2.setDocumentation("Metodo usato per la esecuzione di azioni prima\ndella morte\ndell'agente.");
                m2.addArgument("Agent", "aref");
                m2.addArgument("String", "packet");
                m2.setVisibility(Method.PROTECTED);
                m2.setBody("//Questo è un body di prova per il takeDown\n//Attenzione!!!!!");
                b.addMethod(m2);

                Method m3 = new Method("getName");
                m3.setVisibility(Method.PUBLIC);
                m3.setReturnType("String");
                b.addMethod(m3);

                Attribute a = new Attribute("int", "number");
                a.setFinal();
                Attribute a2 = new Attribute("String", "nomeAgente");
                a2.setVisibility(Attribute.PROTECTED);
                Attribute a3 = new Attribute("String", "nomeAgente");
                a3.setVisibility(Attribute.PROTECTED);
                a3.setInit("\"bon\"");
                b.addAttribute(a);
                b.addAttribute(a2);
                b.addAttribute(a3);

            Class b2 = new Class ("AgenteAnnidato");
            b2.addPackage("nestedsociety");
            b2.setExtends("Agent", "jade.core.Agent");
            b2.setDocumentation("Questa è invece una classe creata dall'utente\nAlessandro Giambruno. Lode a lui!");
        bb.addClass(b);
        bb.addClass(b2);
        bb.toFile();
   
     }
    
}
