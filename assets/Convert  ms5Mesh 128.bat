for %%i in (%*) do python "C:\Users\Joshua\dev\ds\tools\md5_to_dsma\md5_to_dsma.py" --model %%i --name %%~ni --output "C:\Users\Joshua\dev\ds\Portal-DS\data" --texture "128" "128" --bin --blender-fix
pause