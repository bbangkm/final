#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
void error_handling(char* message); // 에러 핸들링

typedef struct { // typedef로 Bada 구조체 설정, 안에는 변수
	int* clnt_sock;
	char* message;
}Bada;

void* from(void* clnt_sock){ // 데이터 보내기
	int* cf=(int*)clnt_sock;	
	while(1){
		char message[100]; // 메시지 배열 크기 임의 설정
		printf("\n서버님 : ");
		scanf("%s",message);
		write(*cf,message,sizeof(message));
	}
}

void* to(void* cf_Data){ // 데이터 받기
	Bada* data =(Bada*)cf_Data;
	while(1){
		int slen=read(*(data->clnt_sock),data->message,sizeof(char)*100);	
		if(slen!=-1){	
			printf("\n - 클라이언트님 : %s\n",data->message);
		}		
	}
}

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;
	char message[100];
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;	
	// 포릍 읽기
	printf("포트 읽기\n");
	if(argc!=2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	//에러 설정
	serv_sock=socket(PF_INET, SOCK_STREAM,0);
	if(serv_sock==-1){
		error_handling("socket() error");	
	}
	//주소 설정
	printf("서버 주소 세팅\n");	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	//소켓 바인드
	printf("바인딩 중 \n");\
	
	if(bind(serv_sock,(struct sockaddr*) &serv_addr,sizeof(serv_addr))==-1){
		error_handling("bind() error");
	}
	//소켓 연결요청
	if(listen(serv_sock,5)==-1){
		error_handling("listen() error");
	}	
	printf("기다려 주세요\n");
	// 서버 - 클라이언트 연결 
	clnt_addr_size =sizeof(clnt_addr);
	clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
	if(clnt_sock==-1) error_handling("accept() error");
	
	Bada cf_Data;
	cf_Data.clnt_sock=&clnt_sock;
	cf_Data.message=message;

	pthread_t p_thread[2];	
	int t;
	int status;
	for(t=0;t<2;t++){
		if(t==0) pthread_create(&p_thread[t],NULL,from,(void*)&clnt_sock);
		else if(t==1)pthread_create(&p_thread[t],NULL,to,(void*)&cf_Data);
	}
	
	pthread_join(p_thread[0],(void **)&status);
	pthread_join(p_thread[1],(void **)&status);

	close(clnt_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
