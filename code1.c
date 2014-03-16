#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

#define exit1 1

int fd[2];
int Closingtime;
int Startingtime;
int cpid;
int exit2;
int status=exit;
//
// 	For Sever Response
//
void server_response(int bytes_received,int connected,char recv_data[],struct sockaddr_in server_addr,int addr_len,int option)
{
    while(1)
    {
	if(option==0)
	    bytes_received=recv(connected,recv_data,1024,0);
	else
	    bytes_received=recvfrom(connected,recv_data,1024,0,
		    (struct sockaddr *)&server_addr, &addr_len);
	recv_data[bytes_received] = '\0';
	if(strcmp(recv_data,"EOFile") == 0)break;
	fwrite(recv_data,1,bytes_received,stdout);

    }
}
void send_data_from_client(char send_data[],int connected,struct sockaddr_in server_addr,int option)
{
    if(option==0)
	send(connected, send_data,strlen(send_data), 0);
    else
	sendto(connected, send_data, strlen(send_data), 0,
		(struct sockaddr *)&server_addr, sizeof(struct sockaddr));
}
void client_request(int bytes_received,int connected,char recv_data[],struct sockaddr_in client_addr,int addr_len,int option,int sock)
{
    if(option==0)
	bytes_received=recv(connected,recv_data,1024,0);
    else
	bytes_received=recvfrom(sock,recv_data,1024,0,
		(struct sockaddr *)&client_addr, &addr_len);
    recv_data[bytes_received] = '\0';
}
void server_side_reponse(int connected,char send_data[],int sock,struct sockaddr_in client_addr,struct sockaddr_in server_addr,int option)
{
		if(option==0)
		    write(connected,send_data,1024);
		else
		    sendto(sock, send_data, 1024, 0,
			    (struct sockaddr *)&client_addr, sizeof(struct sockaddr));
}

void make_client( int port , int option){
    int connected, bytes_received;  
    char send_data [1024],recv_data[1024];
    char reg[100];
    char fileName[100];
    struct hostent *host;
    struct sockaddr_in server_addr;  
    if(option==0)
	host = gethostbyname("127.0.0.1");
    else
	host= (struct hostent *) gethostbyname((char *)"127.0.0.1");
    /*
		BEGINS

       */
    if(option==0)
    {
	if ((connected = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("Socket");
	    exit(1);
	}
    }
    else
    {
	if ((connected = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("Socket");
	    exit(1);
	}
    }


    server_addr.sin_family = AF_INET;     
    server_addr.sin_port = htons(port);   
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    if(option==0)
	bzero(&(server_addr.sin_zero),8); 

    if(option==0)
	while (connect(connected, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1) ;

    char *input;
    int addr_len = sizeof(struct sockaddr);
    char temp[1024];
    while(1)
    {
	gets(send_data); 

back :		
	strcpy(temp,send_data);
	input = strtok (temp," ");

	if(input==NULL)continue;
	    if(strcmp(input,"IndexGet")==0)	
	    {
		printf ("%s\n",input);
		input = strtok (NULL, " ");
		if(input!=NULL)
		{
		    if(strcmp(input,"ShortList")==0)
		    {
			input = strtok (NULL, " ");
			if(input!=NULL)
			{
			    strcpy(send_data,"IS");
			    strcat(send_data,input);
			    input = strtok (NULL, " ");
			    if(input!=NULL)
			    {
				strcat(send_data," ");
				strcat(send_data,input);
				send_data_from_client(send_data,connected,server_addr,option);
				server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);
				continue;
			    }
			}
			else printf("Wrong Usage. Please Type \"IndexGet ShortList TimeStamps\"	\n");	
		    }

		    else if(strcmp(input,"RegEx")==0)
		    {
			input = strtok (NULL, " ");
			if(input!=NULL)
			{
			    strcpy(send_data,"IR");
			    strcat(send_data,input);
			    printf("send_data : %s\n",send_data);
			    send_data_from_client(send_data,connected,server_addr,option);
			    server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);

			    continue;
			}
			else
			{
			    printf("Give the Proper Regular Expression\n");
			    continue;
			}
		    }


		    else if(strcmp(input,"LongList")==0)
		    {
			strcpy(send_data,"IL");

			send_data_from_client(send_data,connected,server_addr,option);
			server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);

			continue;
		    }
		}
	    }
	    else if(strcmp(input,"Upload") == 0)
	    { 
		input = strtok (NULL, " ");
		char temp_arr[100];
		strcpy(temp_arr,input);
		printf("Permission to Upload ");
		printf("%s\n",temp_arr);	
		strcpy(send_data,"U ");
		strcat(send_data,input);
		send_data_from_client(send_data,connected,server_addr,option);

	    }
	    else if(strcmp(input,"FileHash")==0) 
	    {
		input = strtok (NULL, " ");
		if(input !=NULL)
		{
		    if(strcmp(input,"Verify")==0)
		    {
			input = strtok (NULL, " ");
			if(input!=NULL)
			{
			    strcpy(send_data,"FV");
			    strcat(send_data,input);
			    send_data_from_client(send_data,connected,server_addr,option);
			    server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);
			    continue;
			}
			else
			{
			    printf("Command Verify : FileHash Verify <filename>\n");
			    printf("Try Again \n");
			}

		    }
		    else if(strcmp(input,"CheckAll")==0)
		    {
			strcpy(send_data,"FA");
			send_data_from_client(send_data,connected,server_addr,option);
			server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);
			continue;
		    }
		}
		else
		{    
		    printf("FileHash command Not in Correct Format\n");
		    continue;

		}

	    }

	    else if(strcmp(input,"Download") == 0)
	    {
		input = strtok (NULL, " ");
		if(input!=NULL)
		{
re:			strcpy(send_data,"D ");
			strcat(send_data,input);
			send_data_from_client(send_data,connected,server_addr,option);
			FILE *fp1 = fopen(input,"w");

			memset(recv_data,0,1024);
			server_response(bytes_received,connected,recv_data,server_addr,addr_len,option);
			fclose(fp1);

			bytes_received=read(connected,recv_data,1024);

			char temp2[1024];
			strcpy(temp2,recv_data);
			char * file_hash = strtok(temp2," ");
			if(file_hash == NULL) 
			    file_hash = strtok(NULL," ");
			if(strcmp(file_hash,input) == 0)
			{
			    file_hash = strtok(NULL," ");
			    char filename[100];
			    strcpy(filename,input);
			    char pack[1024],game[100],command_str[1000];

			    strcpy(command_str,"md5sum ");
			    strcat(command_str,filename);
			    strcat(command_str," | awk '{print $1}'> result");
			    system(command_str);
			    int stlen;
			    FILE *f1= fopen("result","r");
			    fgets( pack, 100, f1 );
			    stlen=strlen(pack);
			    pack[stlen-1]='\0';
			    fclose(f1);
			    strcpy(command_str,"rm result");
			    system(command_str);

			    if(strcmp(pack,file_hash) == 0 ){ 
				printf(" Completed The Trasfer of File\n");
			    }
			    else 
			    {
				printf("mds shows error , reuploading the file\n") ;
				goto re;
			    }		
			}
			else
			{
			    printf("ERROR file names dont match\n");
		    	    continue;
			}
		}

		else
		{
		    printf("Download Command Error");
		    continue;
		}

	    }
	    else if(strcmp(input,"Allow") == 0)
	    {	

		close(fd[1]);
		int mybytes;
		char as[100];
		int nbytes = read(fd[0],fileName,sizeof(fileName));
		strcpy(send_data,"Download ");
		strcat(send_data,fileName);
		goto back;
	    }
	    else if(strcmp(input,"Deny") == 0)
	    {
		//close(fd[1]);
		printf("Stopped");
		printf("Proces\n");
	    }
	    else
	    {
		
		if ((strcmp(send_data , "q") == 0 || strcmp(send_data , "Q") == 0) || status ==exit1)
		{
		    send_data_from_client(send_data,connected,server_addr,option);
		    if(status)printf("Session Terminated !! \n");
		    fflush(stdout);
		    close(connected);
		    kill(cpid, SIGTERM);
		    exit(0);
		    break;
		}

		else
		{
		    send_data_from_client(send_data,connected,server_addr,option);
		}
	    }



    }
    return;
}

void make_server(int port,int option){
    int sock, connected,bytes_received;  
    char send_data [1024],recv_data[1024],reg[100];
    struct sockaddr_in server_addr,client_addr;    
    int sin_size;

    if(option==0)
    {
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	    perror("Socket");
	    exit(1);
	}
    }
    else
    {
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("Socket");
	    exit(1);
	}

    }

    server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(port);     
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    if(option==0)
	bzero(&(server_addr.sin_zero),8); 

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
	    == -1) {
	perror("Unable to bind");
	exit(1);
    }

    if(option==0)
    {
	if (listen(sock, 5) == -1) {
	    perror("Listen");
	    exit(1);
	}
    }
    int addr_len = sizeof(struct sockaddr);
    if(option==0)
	printf("\nTCPServer Waiting for client on port %d\n",port);
    else
	printf("\nUDPServer Waiting for client on port %d\n",port);
    fflush(stdout);



    sin_size = sizeof(struct sockaddr_in);
    connected = accept(sock, (struct sockaddr *)&client_addr,&sin_size);

    printf("\nConnection From (%s , %d)",
	    inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    char * input;
    fflush(stdout);
    char command[1024];
    char lower[100],upper[100],cpy[1000];
    int cnter=0;
    char buff[1000];
    FILE *fpr;
    FILE *fpr2;
    int new_pl=0;
    while (1)
    {

	client_request(bytes_received,connected,recv_data,client_addr,addr_len,option,sock);
    fflush(stdout);


	if (strcmp(recv_data , "quit") == 0 || strcmp(recv_data , "Quit") == 0 || strcmp(recv_data , "Q") == 0 || strcmp(recv_data , "q") == 0)
	{	
	    status=exit1; 
	    printf("Connection Closed\n");
	    close(connected);

	    //exit(0);
	    break;
	}
	else if(recv_data[0]=='F' && recv_data[1]=='A')
	{
	    char cmd_temp[1000]="find . -type f -exec sh -c 'printf \"%s %s \n\" \"$(ls -l --time-style=+%Y%m%d%H%M%S $1 )\" \"$(md5sum $1 | cut -d\" \" -f1)\"' '' '{}    ' '{}' \\; | awk '{print $7, $6, $8}' > result";
	    system(cmd_temp);

	    //file to be sent here checkall

	    FILE *fp = fopen("result","r");
	    if(fp == NULL)
	    {
		printf("wrong file\n");
		continue;
	    }

	    int nbytes;
	    memset(send_data,0,1024);
	    while(1)
	    {	
		if(feof(fp))
		    break;
		nbytes = fread(send_data,1,1024,fp);
		send_data[nbytes] = '\0';
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);
	    }
	    memset(send_data,0,1024);
	    char end[]= "EOFile";
	    strcpy(send_data,end);
	    write(connected,send_data,1024);
	    fclose(fp);

	    continue;
	}
	else if(recv_data[0]=='F' && recv_data[1]=='V')
	{
	    strncpy(reg,(char*)recv_data+2,100);

	    strcpy(command,"openssl md5 ");
	    strcat(command,reg);
	    strcat(command," | cut -d\" \" -f2 > md5");
	    system(command);
	    strcpy(command,"date -r ./");
	    strcat(command,reg);
	    strcat(command," +%Y%m%d%H%M%S > date");
	    system(command);
	    strcpy(command,"paste md5 date > result");
	    system(command);
	    strcpy(command,"rm md5 date");
	    system(command);

	    FILE *fp = fopen("result","r");
	    if(fp == NULL)
	    {
		printf("wrong file\n");
		continue;
	    }

	    memset(send_data,0,1024);
	    int no_bytes;
	    int sen;
	    while(1)
	    {	
		if(feof(fp))
		    break;
		no_bytes = fread(send_data,1,1024,fp);
		send_data[no_bytes] = '\0';
		sen =  write(connected,send_data,1024);
	    }
	    memset(send_data,0,1024);
	    char end[]= "EOFile";
	    strcpy(send_data,end);
	    if(option==0)
		write(connected,send_data,1024);
	    else
		sendto(sock, send_data, 1024, 0,
			(struct sockaddr *)&client_addr, sizeof(struct sockaddr));
	    fclose(fp);
	    continue;
	}

	else if(recv_data[0]=='I' && recv_data[1]=='L')
	{
	    //system("ls -l > longlist");
	    system("ls -l | awk 'BEGIN {print \"\\nName\\t\\t\\t\\t\\t\\tSize\\t\\t\\tTime\"}{printf \"%-40s\\t%-10s\\t%-6s%-6s%-6s\\n \" ,$9,$5,$ 6,$7,$8} END{print \"\\n \\t\\t\\t\\t------Server Response-------\\n\"}' > result");
	    //	    system("find . -printf '%p %TY%Tm%Td%TH%Tm%Tm %k \n' > il");
	    //file to be sent here longlist

	    FILE *fp = fopen("result","r");
	    if(fp == NULL)
	    {
		perror("Error in opening file");
		printf("wrong file\n");
		continue;
	    }
	    memset(send_data,0,1024);
	    int no_bytes,sen;
	    while(1)
	    {	//memset(send_data,0,1024);
		if(feof(fp))
		{
		    break;
		}
		no_bytes = fread(send_data,1,1024,fp);
		send_data[no_bytes] = '\0';
		printf("send_data %s",send_data);
		//   byteR = strlen(data);
		// if(byteR == 0) break

		//	fflush(stdout);
		//void *temp = data;
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);
		//printf("read %d sent %d--------\n",sen,no_bytes);
		//===============================file sent=================================================
	    }
	    memset(send_data,0,1024);
	    char end[]= "EOFile";
	    strcpy(send_data,end);
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);

	    fclose(fp);

	    //strcpy(send_data,"ANS here is the list...... :P"); 
	    //send(connected, send_data,strlen(send_data), 0);

	    continue;
	}
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  SHORTLIST >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TO BE COPIED BEFORE IR
	else if(recv_data[0] == 'I' && recv_data[1]=='S')
	{

	    strncpy(reg,(char*)recv_data+2,100);
	    input = strtok (reg," ");
	    strcpy(lower,input);
	    input = strtok (NULL, " ");
	    strcpy(upper,input);
	    system("ls -l --time-style=+%Y%m%d%H%M%S -t > ls");

	    //prinf("Entered");
	    fpr = fopen( "ls", "r" );
	    fpr2 = fopen( "result", "w" );
	    while ( fgets( buff, 1000, fpr ) != NULL )
	    {	
		if(cnter!=0 && cnter!=1)
		{
		    strcpy(cpy,buff);
		    new_pl=0;
		    input = strtok (buff," ");
		    while (input != NULL)
		    {
			if(new_pl==5)
			{
			    if(strcmp(input,lower)>0 && strcmp(input,upper)<0)
			    {
				fprintf(fpr2,"%s",cpy);
			    }
			}
			new_pl++;
			input = strtok (NULL," ");
		    }
		}
		cnter++;
	    }
	    fclose( fpr );
	    fclose( fpr2 );

	    FILE *fp = fopen("result","r");
	    if(fp == NULL)
	    {
		printf("wrong file\n");
		continue;
	    }

	    memset(send_data,0,1024);
	    int no_bytes,sen;
	    while(1)
	    {	
		if(feof(fp))
		    break;
		no_bytes = fread(send_data,1,1024,fp);
		send_data[no_bytes] = '\0';
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);
	    }
	    memset(send_data,0,1024);
	    char end[]= "EOFile";
	    strcpy(send_data,end);
	    if(option==0)
		write(connected,send_data,1024);
	    else
		sendto(sock, send_data, 1024, 0,
			(struct sockaddr *)&client_addr, sizeof(struct sockaddr));
	    fclose(fp);

	    continue;


	}
	else if(recv_data[0]=='I' && recv_data[1]=='R')
	{
	    memset(send_data,0,1024);
	    strncpy(reg,(char*)recv_data+2,100);
	    strcpy(command,"find . -name \""); 
	    strcat(command,reg); 
	    strcat(command,"\" -exec ls -l {} \\; | awk '{print $9\"\\t\"$5}' | cut -c "" 3- > result"); 
	    system(command);

	    FILE *fp = fopen("result","r");
	    if(fp == NULL)
	    {
		printf("Check");
		printf("Wrong File\n");
		continue;
	    }

	    memset(send_data,0,1024);
	    int no_bytes;
	    int sen;
	    while(1)
	    {	
		if(feof(fp))
		    break;
		no_bytes = fread(send_data,1,1024,fp);
		send_data[no_bytes] = '\0';
		sen =  write(connected,send_data,1024);
	    }
	    memset(send_data,0,1024);
	    char end[]= "EOFile";

	    strcpy(send_data,end);
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);
	    fclose(fp);
	    continue;
	}
	else if(strlen(recv_data)>2 && recv_data[0]=='U' && recv_data[1]==' ')
	{
	    char temp_arr[100];
	    strncpy(temp_arr,(char*)recv_data+2,100);
	    printf("Upload File : %s\n Type \"Allow\" to let them; \"Stop\" to prevent them\n",temp_arr);
	    printf("\"Allow\" to let them; \"Stop\"");
	    printf(" ");
	    close(fd[0]);
	    write(fd[1],temp_arr,strlen(temp_arr)+1);		

	}
	else if(strlen(recv_data)>2 && recv_data[0]=='D' && recv_data[1]==' ')
	{
	    char temp_arr[100];
	    strcpy(temp_arr,recv_data+2);
	    FILE *fp = fopen(temp_arr,"r");
	    if(fp == NULL)
	    {
		perror("Error in opening file");
		printf("wrong file\n");
		continue;
	    }

	    memset(send_data,0,1024);
	    int nbytes,sen,myb1;

	    while(1)
	    {	
		if(feof(fp))
		{
		    break;
		}
		nbytes = fread(send_data,1,1024,fp);
		sen =  write(connected,send_data,nbytes);

	    }
	    memset(send_data,0,1024);

	    char end[]= "EOFile";
	    strcpy(send_data,end);
	    write(connected,send_data,1024);
	    fclose(fp);
	    char filename[100];
	    strcpy(filename,temp_arr);
	    char pack[1024];
	    char command_str[1000];
	    int stlen;
	    strcpy(command_str,"md5sum ");
	    strcat(command_str,filename);
	    strcat(command_str," | awk '{print $2, $1}'> t1");
	    system(command_str);
	    strcpy(command_str,"stat -c%s ");
	    strcat(command_str,filename);
	    strcat(command_str," > t2");
	    system(command_str);
	    strcpy(command_str,"paste t1 t2 | awk '{print $1, $2, $3}' > md5");
	    system(command_str);
	    strcpy(command_str,"rm t1 t2");
	    system(command_str);

	    FILE *f1= fopen("md5","r");
	    fgets( pack, 100, f1 );
	    stlen=strlen(pack);
	    pack[stlen-1]='\0';

	    printf("Packet Info Start --\n");
	    printf("Packet Length  %d\n Packet Header %s\n",strlen(pack),pack);
	    printf("Packet Info End --\n");
	    fclose(f1);
	    strcpy(command_str,"rm md5");
	    system(command_str);

	    memset(send_data,0,1024);
	    strcpy(send_data,pack);
		server_side_reponse(connected,send_data,sock,client_addr,server_addr,option);
	}


	//else{
		
	    //printf("%s\n" , recv_data);
	    //fflush(stdout);
	//}


    }


    close(sock);

}

int main()

{
    int port,port1,option;
    printf("Enter the port on which you want to make server : ");
    scanf("%d",&port);
    printf("Enter the server port to connect at : \n");
    scanf("%d",&port1);
    printf("Enter the client udp(1) or tcp(0) \n");
    scanf("%d",&option);

    pid_t pid=fork();
    cpid = getpid();

    pid = fork();

    if(!pid)
    {
	make_client(port1,option);
    }
    else {

	make_server(port,option);

    }
    return 0;
}

