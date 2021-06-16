#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
void error_hadling(char* message); //에러 핸들링

typedef struct bada{ // typedef로 bada 구조체명 설정 
	char* message;
	int* serv_sock;
}bada;

void* from(void* serv_sock){ //데이터 보내기
	int* cf=(int*)serv_sock;	
	while(1){
		char message[100]; 
		printf("\n클라이언트님:  "); // 데이터 입력
		scanf("%s",message);
		write(*cf,message,sizeof(message));
	}
	
}

void* to(void* cf_data){ // 데이터 받기

	bada* data=(bada*)cf_data;
	char* dm=data->message; //바로 가기
	while(1){
		int slen=read(*(data->serv_sock), dm, sizeof(char)*100);	
		if(slen!=-1){	
			printf("\n <- 서버님 : %s\n", dm);
		}	
	}	

}

int main(int argc, char *argv[]){ //메인
	
	int serv_sock;
	struct sockaddr_in serv_addr;
	char message[100];
	int slen;
	
	if(argc!=3){ // IP port 받음
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	} 
	
	serv_sock=socket(PF_INET,SOCK_STREAM,0);
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	connect(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)); // 호스트랑 연결, 정보 serv_addr 주소에 저장
	
	pthread_t p_thread[2];
	int t;
	int status;
	bada cf_data; // cf_data 변수 
	cf_data.message=message; // 구조체에 접근
	cf_data.serv_sock=&serv_sock;
	for(t=0;t<2;t++){
		if(t==0) pthread_create(&p_thread[t],NULL,from,(void*)&serv_sock);
		else if(t==1)pthread_create(&p_thread[t],NULL,to,(void*)&cf_data);
	}
	pthread_join(p_thread[0],(void **)&status);
	pthread_join(p_thread[1],(void **)&status);

	close(serv_sock);
	return 0;
}

void error_handling(char* message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}


