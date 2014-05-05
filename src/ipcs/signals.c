#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include "../../include/utils.h"
#include "../../include/rdwrn.h"
#include "../../include/common.h"
#define INVALID -1
#define SIGNALFILES_IPC_DIR "/tmp/so-signalfiles"
#define BUFFER_SIZE 100
#define UNEXPECTED_DELETE_ERROR 2
static char buf[BUFFER_SIZE];
static void __handler(int n);
static string __get_path(char * id);
static int __write_new(char * id) ;
static FILE * __open(char * name, const string mode);
int ipc_connect(int from_id, int to_id);

//inicia el file del server
int ipc_init(int from_id){
	ipc_connect(from_id, INVALID);
	return OK;
}

//Borro el file del server
int ipc_close(int from_id){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
}

//Crea los archivos de from id si no existen
int ipc_connect(int from_id, int to_id){
	char ipcname[20];
	int ret;
	sprintf(ipcname, "%d", from_id);
	FILE * file=__open(ipcname,"r");
	if(file==NULL){
		ret=__write_new(ipcname);
	}
	return ret;
}

//borro el file del cliente 
int ipc_disconnect(int from_id, int to_id){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	if (remove(__get_path(ipcname)) != 0){
		return UNEXPECTED_DELETE_ERROR;
	}
	return OK;
}

//escribe en el archivo de to_id. Mando señal para despertar.
int ipc_send(int from_id, int to_id, void * buf, int len){
	char ipcname[20];
	sprintf(ipcname, "%d", to_id);
	FILE * file=__open(ipcname, "r+w");
	if(file==NULL){
		return ERROR;
	}
	int ret=fwriten(fileno(file),buf,len); //atomico. Si lo interrumpe una señal sigue hasta escribir los n.
	if(ret==-1){
		return ERROR;
	}
	kill(to_id,SIGUSR1);
	return fclose(file);
}


//lee el archivo de from_id
int ipc_rcv(int from_id, void * buf, int len){
	char ipcname[20];
	sprintf(ipcname, "%d", from_id);
	FILE * file=__open(ipcname, "r");
	signal(SIGUSR1,__handler);
	freadn(fileno(file),buf,len);
	return OK;
}

int __write_new(char * id) {
	FILE * file = __open(id, "w+");
	return fclose(file);
}

FILE * __open(char * name, const string mode) {
	return fopen(__get_path(name), mode);
}

string __get_path(char * id) {
	sprintf(buf, "%s/%s", SIGNALFILES_IPC_DIR, id); 
	return buf;
}

void __handler(int n) { 
	;
}