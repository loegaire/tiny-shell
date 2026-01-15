echo ">>> TEST 1: HELP COMMAND"
help

echo ">>> TEST 2: EXTERNAL COMMANDS (date, ls)"
date
ls

echo ">>> TEST 3: ENVIRONMENT VARIABLES (set/get)"
set MY_VAR hello_world
get MY_VAR
set MY_VAR changed_value
get MY_VAR

echo ">>> TEST 4: HISTORY"
history

echo ">>> TEST 5: EXIT"
exit
