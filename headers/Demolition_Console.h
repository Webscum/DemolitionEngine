// Inside of this header is the functionality of the Demolition Engine Console, aka. the IO of commands

typedef struct{
	char* name;
	char* description;
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