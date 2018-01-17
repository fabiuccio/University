@echo Dovresti usare il programmino in corredo a questa tesina per la esecuzione di cup!

@SET LOCALE_CUP=D:\_AGDIR\Specialistica\Materie\Linguaggi e traduttori\Laboratorio\

@SET CLASSPATH=%CLASSPATH%;%LOCALE_CUP%cup;.
java java_cup/Main -expect 100 parser.cup 

pause