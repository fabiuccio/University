SET TESINAPATH=c:\tesina 


cd %TESINAPATH%
build -W
@echo Press a key to install the driver please...
pause
@echo Installing driver...
start minispy.inf
pause
@echo When .inf procedure has finished, press a key to continue
cd user/objc*
cd i*
@echo starting driver.......
net stop minispy
net start minispy
@echo Launching minispy.exe.......
minispy