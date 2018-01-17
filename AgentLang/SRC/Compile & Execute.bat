@echo Dovresti usare il programmino in corredo a questa tesina per la compilazione e l'esecuzione!

@SET LOCALE=D:\_AGDIR\Specialistica\Materie\Linguaggi e traduttori\Laboratorio\TESINA\
@SET LOCALE_CUP=D:\_AGDIR\Specialistica\Materie\Linguaggi e traduttori\Laboratorio\

@SET CLASSPATH=%CLASSPATH%;%LOCALE_CUP%cup;.
@SET CLASSPATH=%CLASSPATH%;%LOCALE%src\BackBonePJ\build\classes;

javac *.java

pause

java parser agent.alang

pause
