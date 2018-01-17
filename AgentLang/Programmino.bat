REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
REM * * * Editare solo le seguenti due righe di codice, con le opportune modifiche specificando
REM * * * i path dove si trova la directory di lavoro TESINA (il root della distribuzione) e
REM * * * il path dove si trova la directory cup:

@SET LOCALE=D:\_AGDIR\Specialistica\Materie\Linguaggi e traduttori\Laboratorio\TESINA\
@SET LOCALE_CUP=D:\_AGDIR\Specialistica\Materie\Linguaggi e traduttori\Laboratorio\

REM * * * tutto ciò che segue non dovrebbe essere modificato.
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
REM * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

@ECHO off
cls
ECHO Sono stati considerati i seguenti percorsi (root della tesina e per cup rispettivamente):
SET LOCALE


@SET CLASSPATH=%LOCALE_CUP%cup;.
@SET CLASSPATH=%CLASSPATH%;%LOCALE%src\BackBonePJ\build\classes;



:start
ECHO.
   echo Cosa vuoi fare?
   echo (1) Compila i files!
   echo (2) Avvia il Parser
   echo (3) Esci dal programma di compilazione
   echo (4) Compila il parser!
   echo.
set choice=
set /p choice=Inserisci il numero relativo all'azione che vuoi eseguire.
if not '%choice%'=='' set choice=%choice:~0,1%
if '%choice%'=='1' goto compila
if '%choice%'=='2' goto avvia
if '%choice%'=='3' goto esci
if '%choice%'=='4' goto compilaparser
ECHO "%choice%" Il tasto premuto non è valido.
ECHO.
goto start
:COMPILA
ECHO.
ECHO Hai scelto di compilare....clicca per compilare
ECHO.
pause
	cd src
	javac *.java -nowarn
ECHO.
ECHO I files sono stati compilati correttamente! Clicca su qualsiasi tasto per tornare al menu iniziale..
ECHO.
pause
cls
goto start
:AVVIA
ECHO.
ECHO Hai scelto di avviare il parser. Clicca per avviare il parser
ECHO.
	cd src
	java parser agent.alang
ECHO .
ECHO Il parser è stato avviato correttamente. Clicca per tornare al menu principale.
pause
ECHO .
cls
goto start
:COMPILAPARSER
ECHO Hai deciso di compilare il parser!
	cd src
	runcup
:ESCI
cls
ECHO .
ECHO Hai deciso di uscire dal programma
ECHO .---..---..---.---..---..---.---..---..---
:end
ECHO .
ECHO Powered by Fabio e Alessandro..
ECHO .---..---..---.---..---..---.---..---..---
pause