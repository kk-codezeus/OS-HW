#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Simplifed xv6 shell.

#define MAXARGS 10

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
  int type;          //  ' ' (exec), | (pipe), '<' or '>' for redirection
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // arguments to the command to be exec-ed
};

struct redircmd {
  int type;          // < or > 
  struct cmd *cmd;   // the command to be run (e.g., an execcmd)
  char *file;        // the input/output file
  int mode;          // the mode to open the file with
  int fd;            // the file descriptor number to use for the file
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // left side of pipe
  struct cmd *right; // right side of pipe
};

int fork1(void);  // Fork but exits on failure.
struct cmd *parsecmd(char*);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2], r;
  int pid;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;
	
  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown runcmd\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    //char Path[20] = "/bin/";
   // execv(strcat(Path,ecmd->argv[0]),ecmd->argv);
    //fprintf(stderr, "Not found in /bin/ trying in /usr/bin\n");
    //char userPath[30] = "/usr/bin/";
    execvp(ecmd->argv[0],ecmd->argv);
    fprintf(stderr, "Exec failed...\n");
    // Your code here ...
    break;

  case '>':
	rcmd = (struct redircmd*)cmd;            
	close(1);					// Closing the stdout to point it to the write file
	open(rcmd->file,rcmd->mode,0777);		// Pointing the smallest open fd to new file(in this case 1);
	runcmd(rcmd->cmd);

  case '<':

	rcmd = (struct redircmd*)cmd;
	close(0);					// Closing the stdin 
	open(rcmd->file,rcmd->mode);			// Pointing the smallest open fd to new file to read(in this case 0);
	runcmd(rcmd->cmd);
   /* // Your code here ...
    //printf("redir case\n");
    if(rcmd->type == 60)	//Redirecting input
    {

	    pid = fork();
	    if(pid < 0)
	    {
		    perror("Fork for < not working..... exiting\n");
		    exit(0);
	    }
	    if(pid == 0)
	    {	
		// printf("Here <\n");
	    	close(0);              // closing stdin
	    	//opening lowest fd(here 0) and pointing it to the file to be read
	    	if((rcmd->fd = open(rcmd->file,rcmd->mode)) < 0)
	    	{
		    perror("File not opened for write... exiting\n");
		    exit(0);
	    	}
		runcmd(rcmd->cmd);
    		fprintf(stderr, "command not executed from <\n....exiting");
		exit(0);
	    }
	    else
	    {
		    wait(NULL);
		    
	    }

    }
    if(rcmd->type == 62){	//{Redirecting stdout
   	 pid = fork();
   	 if(pid < 0)
   	 {
	    perror("Fork for < not working..... exiting\n");
	    exit(0);
    	 }
    	 if(pid == 0)
   	 {
		 
	   // printf("Here >\n");
	    close(1);           //closing stdout
	    //opening lowest fd(here 1) and pointing it to the file to create and write
	    if((rcmd->fd = open(rcmd->file,rcmd->mode,0777)) < 0)
	    {
		    perror("File not opened to write\n....exiting");
		    exit(0);
	    }
	    //char* line = rcmd->cmd->argv[1];
	    runcmd(rcmd->cmd);

    		fprintf(stderr, "command not executed from >\n....exiting");
		exit(0);

    	  }
	 else
	 {
		 wait(NULL);
	 }
    }*/	 
    break;

  case '|':
    pcmd = (struct pipecmd*)cmd;
    if(pipe(p) < 0)                         // Creating pipe and checking error
    {
	    fprintf(stderr,"Not able to start a pipe... exiting\n");
	    exit(0);
    }
    pid = fork();			// Forking to run left command and checking
    if(pid < 0)
    {
	    fprintf(stderr,"Fork for first command not working.. exiting\n");
	    exit(0);
    }
    if(pid == 0)	    		//Child process for left command
    {
	    close(p[0]);		//Closing read pipe descriptor for left command
	    dup2(p[1],1);		//Pointing the child's stdout to pipe's write
	    close(p[1]);		//Closing the write pipe descriptor for child
	    runcmd(pcmd->left);
    }
    else				//Parent process for right command
    {
	    close(p[1]);		//Closing write pipe descriptor for right command
	    dup2(p[0],0);		// Pointing child's stdin to pipe's read
	    close(p[0]);		//Closing the read pipe descriptor for child
	    runcmd(pcmd->right);	    //Closing the pipe's write descriptor for future use
    }
    /*pid = fork();
    if(pid < 0)			// Forking to run right command and error checking fork()
    {
	    fprintf(stderr,"Second Fork not working.. exiting\n");
	    exit(0);
    }
    if(pid == 0)	    //Child process for right command
    {
	    //close(p[1]);		//No need to close pipe write descriptor as already done in parent
	    dup2(p[0],0);		//Pointing the right command's stdin to pipe read descriptor
	    close(p[0]);
	    runcmd(pcmd->right);
    }*/
    				

 /*   fprintf(stderr, "pipe not implemented\n");*/
    // Your code here ...
    break;
  }    
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  
  if (isatty(fileno(stdin)))
    fprintf(stdout, "238P$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd, r;

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Clumsy but will have to do for now.
      // Chdir has no effect on the parent if run in the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(stderr, "cannot cd %s\n", buf+3);
      continue;
    }
    if(fork1() == 0)
      runcmd(parsecmd(buf));
    wait(&r);
  }
  exit(0);
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

// make a copy of the characters in the input buffer, starting from s through es.
// null-terminate the copy to make it a string.
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;

  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}
