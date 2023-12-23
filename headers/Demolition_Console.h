// Inside of this header is the functionality of the Demolition Engine Console, aka. the IO of commands
// Left to rot for now

typedef struct{
	char* name;
	vector arguments;
} command;

char* doCommand(command* cmd){
	switch(cmd->name){
		case "HELP":
			
			return "You used the help Command";
			break;
		case "SELECT":
			return "Select object";
			break;
	}
}

command* constuctCommand(char* givenCommand){
	
	command* output;
}

command* freeCommand(command* givenCommand){
	free(givenCommand);
}